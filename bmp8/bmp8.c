#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bmp8/bmp8.h"

t_bmp8* bmp8_loadImage(const char* filename) {

    FILE* file = fopen(filename, "rb");

    if (!file) {
        printf("Error: Cannot open file %s\n", filename);
        return NULL;
    }

    t_bmp8* img = (t_bmp8*)malloc(sizeof(t_bmp8));

    if (!img) {
        printf("Error: Memory allocation failed.\n");
        fclose(file);
        return NULL;
    }

    // fread lit des blocs d’octets depuis un fichier ouvert avec FILE*.
    int img_long = fread(img->header, sizeof(unsigned char), 54, file);

    if (img_long != 54 || img->header[0] != 'B' || img->header[1] != 'M') {
        printf("Error: not an BMP file\n");
        free(img);
        fclose(file);
        return NULL;
    }

    img->width = *(unsigned int*)&img->header[18];
    img->height = *(unsigned int*)&img->header[22];
    img->colorDepth = *(unsigned int*)&img->header[28];
    img->dataSize = *(unsigned int*)&img->header[34];

    fread(img->colorTable, sizeof(unsigned char), 1024, file);

    img->data = (unsigned char*)malloc(img->dataSize);

    if (!img->data) {
        printf("Error: Memory allocation for image data failed.\n");
        free(img);
        fclose(file);
        return NULL;
    }

    fread(img->data, sizeof(unsigned char), img->dataSize, file);

    printf("Image loaded successfully !\n");

    fclose(file);
    return img;
}


void bmp8_saveImage(const char* filename, t_bmp8* img) {

    FILE* file = fopen(filename, "wb");

    if (!file) {
        printf("Error: Cannot open file %s for writing\n", filename);
        return;
    }

    fwrite(img->header, sizeof(unsigned char), 54, file);
    fwrite(img->colorTable, sizeof(unsigned char), 1024, file);
    fwrite(img->data, sizeof(unsigned char), img->dataSize, file);

    printf("Image saved successfully !\n");

    fclose(file);
}


void bmp8_free(t_bmp8* img) {

    if (img) {

        if (img->data) {
            free(img->data);
        }

        free(img);
    }
}


void bmp8_printInfo(t_bmp8* img) {
    printf("Image Info:\n");
    printf("    Width: %u\n", img->width);
    printf("    Height: %u\n", img->height);
    printf("    Color Depth: %u\n", img->colorDepth);
    printf("    Data Size: %u\n", img->dataSize);
}


void bmp8_negative(t_bmp8* img) {

    for (int i = 0; i < img->dataSize; i++) {
        img->data[i] = 255 - img->data[i];
    }

}


void bmp8_brightness(t_bmp8* img, int value) {

    for (int i = 0; i < img->dataSize; i++) {

        int pixel = img->data[i] + value;

        if (pixel > 255) {
            pixel = 255;
        } else if (pixel < 0) {
            pixel = 0;
        }

        img->data[i] = (unsigned char)pixel;
    }

}


void bmp8_threshold(t_bmp8* img, int threshold) {

    for (int i = 0; i < img->dataSize; i++) {
        
        if (img->data[i] >= threshold) {
            img->data[i] = 255;
        } else {
            img->data[i] = 0;
        }
    }
}


void bmp8_applyFilter(t_bmp8* img, float** kernel, int kernelSize) {

    // 1. Récupération des dimensions
    unsigned int w = img->width;
    unsigned int h = img->height;

    int n = kernelSize / 2;

    unsigned char* temp = (unsigned char*)malloc(img->dataSize);

    if (temp == NULL) {
        printf("Error: Memory allocation failed for filter.\n");
        return;
    }

    memcpy(temp, img->data, img->dataSize); 

    // 3. Parcours de l'image (convolution)
    // Selon la consigne : on ignore les bords.
    // On commence à y=n et on s'arrête avant h-n.
    for (int y = n; y < h - n; y++) {

        for (int w_idx = n; w_idx < w - n; w_idx++) {

            float sum = 0.0f;

            // Parcours du noyau (kernel)
            // Les indices varient de -n à +n comme demandé
            for (int ky = -n; ky <= n; ky++) {

                for (int kx = -n; kx <= n; kx++) {

                    // -- Récupération du pixel voisin --
                    // Index dans le tableau 1D : (ligne * largeur) + colonne
                    int pixel_idx = ((y + ky) * w) + (w_idx + kx);
                    char pixel_val = img->data[pixel_idx];

                    // -- Récupération de la valeur du kernel --
                    // Le tableau C commence à 0, donc on décale l'index mathématique (-n devient 0)
                    float kernel_val = kernel[ky + n][kx + n];

                    sum += pixel_val * kernel_val;
                }
            }

            if (sum > 255) sum = 255;

            if (sum < 0) sum = 0;

            temp[y * w + w_idx] = (char) sum;
        }
    }

    memcpy(img->data, temp, img->dataSize);

    free(temp);
}