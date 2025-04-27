#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bmp24.h"

t_bmp24* bmp24_loadImage(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Error: Cannot open file %s\n", filename);
        return NULL;
    }

    t_bmp24* img = (t_bmp24*)malloc(sizeof(t_bmp24));
    if (!img) {
        printf("Error: Memory allocation failed.\n");
        fclose(file);
        return NULL;
    }

    fread(img->header, sizeof(unsigned char), 54, file);
    img->width = *(unsigned int*)&img->header[18];
    img->height = *(unsigned int*)&img->header[22];
    img->colorDepth = *(unsigned short*)&img->header[28];
    img->dataSize = img->width * img->height * 3;

    if (img->colorDepth != 24) {
        printf("Error: Image is not 24-bit color.\n");
        free(img);
        fclose(file);
        return NULL;
    }

    img->data = (unsigned char*)malloc(img->dataSize);
    if (!img->data) {
        printf("Error: Memory allocation for image data failed.\n");
        free(img);
        fclose(file);
        return NULL;
    }

    fread(img->data, sizeof(unsigned char), img->dataSize, file);

    fclose(file);
    return img;
}

void bmp24_saveImage(const char* filename, t_bmp24* img) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("Error: Cannot open file %s for writing\n", filename);
        return;
    }

    fwrite(img->header, sizeof(unsigned char), 54, file);
    fwrite(img->data, sizeof(unsigned char), img->dataSize, file);

    fclose(file);
}

void bmp24_free(t_bmp24* img) {
    if (img) {
        if (img->data) {
            free(img->data);
        }
        free(img);
    }
}

void bmp24_printInfo(t_bmp24* img) {
    printf("Image Info:\n");
    printf("Width: %u\n", img->width);
    printf("Height: %u\n", img->height);
    printf("Color Depth: %u\n", img->colorDepth);
    printf("Data Size: %u\n", img->dataSize);
}

void bmp24_negative(t_bmp24* img) {
    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = 255 - img->data[i];
    }
}

void bmp24_brightness(t_bmp24* img, int value) {
    for (unsigned int i = 0; i < img->dataSize; i++) {
        int pixel = img->data[i] + value;
        if (pixel > 255) pixel = 255;
        if (pixel < 0) pixel = 0;
        img->data[i] = (unsigned char)pixel;
    }
}

void bmp24_threshold(t_bmp24* img, int threshold) {
    for (unsigned int i = 0; i < img->dataSize; i += 3) {
        int avg = (img->data[i] + img->data[i+1] + img->data[i+2]) / 3;
        unsigned char color = (avg >= threshold) ? 255 : 0;
        img->data[i] = color;
        img->data[i+1] = color;
        img->data[i+2] = color;
    }
}

void bmp24_applyFilter(t_bmp24* img, float** kernel, int kernelSize) {
    int n = kernelSize / 2;
    unsigned char* temp = (unsigned char*)malloc(img->dataSize);
    if (!temp) {
        printf("Error: Memory allocation failed for filter.\n");
        return;
    }
    memcpy(temp, img->data, img->dataSize);

    for (unsigned int y = 1; y < img->height - 1; y++) {
        for (unsigned int x = 1; x < img->width - 1; x++) {
            for (int c = 0; c < 3; c++) { // For each color channel
                float sum = 0.0f;
                for (int ky = -n; ky <= n; ky++) {
                    for (int kx = -n; kx <= n; kx++) {
                        int idx = ((y + ky) * img->width + (x + kx)) * 3 + c;
                        sum += temp[idx] * kernel[ky + n][kx + n];
                    }
                }
                int value = (int)roundf(sum);
                if (value > 255) value = 255;
                if (value < 0) value = 0;
                img->data[(y * img->width + x) * 3 + c] = (unsigned char)value;
            }
        }
    }

    free(temp);
}