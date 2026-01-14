#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "bmp24.h"

t_pixel ** bmp24_allocateDataPixels (int width, int height) {

    t_pixel **pixels = (t_pixel **)malloc(height * sizeof(t_pixel *));

    if (!pixels) {
        printf("Error: Memory allocation failed.\n");
        return NULL;
    }

    for (int i = 0; i < height; i++) {

        pixels[i] = (t_pixel *)malloc(width * sizeof(t_pixel));

        if (!pixels[i]) {
            printf("Error: Memory allocation failed.\n");

            for (int j = 0; j < i; j++) {
                free(pixels[j]);
            }

            free(pixels);
            return NULL;
        }
    }
    return pixels;
}


void bmp24_freeDataPixels(t_pixel **pixels, int height) {

    if (!pixels) {
        return;
    }

    for (int i = 0; i < height; i++) {
        free(pixels[i]);
    }

    free(pixels);
}


t_bmp24 *bmp24_allocate(int width, int height, int colorDepth) {
    t_bmp24 *img = (t_bmp24 *)malloc(sizeof(t_bmp24));
    if (!img) {
        printf("Error: Memory allocation failed.\n");
        return NULL;
    }

    img->data = bmp24_allocateDataPixels(width, height);
    if (!img->data) {
        printf("Error: Memory allocation failed.\n");
        bmp24_freeDataPixels(img->data, height);
        free(img);
        return NULL;
    }

    img->width = width;
    img->height = height;
    img->colorDepth = colorDepth;

    return img;
}


void bmp24_free(t_bmp24 *img) {
    if (!img) {
        return;
    }

    bmp24_freeDataPixels(img->data, img->height);
    free(img);
}


/*
* @brief Set the file cursor to the position position in the file file,
* then read n elements of size size from the file into the buffer.
* @param position The position from which to read in file.
* @param buffer The buffer to read the elements into.
* @param size The size of each element to read.
* @param n The number of elements to read.
* @param file The file descriptor to read from.
* @return void
*/
void file_rawRead (uint32_t position, void * buffer, uint32_t size, size_t n, FILE * file) {
    fseek(file, position, SEEK_SET);
    fread(buffer, size, n, file);
}

/*
* @brief Set the file cursor to the position position in the file file,
* then write n elements of size size from the buffer into the file.
* @param position The position from which to write in file.
* @param buffer The buffer to write the elements from.
* @param size The size of each element to write.
* @param n The number of elements to write.
* @param file The file descriptor to write to.
* @return void
*/
void file_rawWrite (uint32_t position, void * buffer, uint32_t size, size_t n, FILE * file) {
    fseek(file, position, SEEK_SET);
    fwrite(buffer, size, n, file);
}

void bmp24_readPixelValue (t_bmp24 * image, int x, int y, FILE * file) {
    uint32_t pixelDataOffset = image->header.offset;
    uint32_t rowSize = ((image->colorDepth * image->width + 31) / 32) * 4;
    uint32_t pixelOffset = pixelDataOffset + (image->height - 1 - y) * rowSize + x * (image->colorDepth / 8);

    /* BMP stores pixels as BGR; swap to our RGB struct layout. */
    uint8_t bgr[3] = {0};
    file_rawRead(pixelOffset, bgr, sizeof(uint8_t), 3, file);

    t_pixel pixel;
    pixel.red = bgr[2];
    pixel.green = bgr[1];
    pixel.blue = bgr[0];

    image->data[y][x] = pixel;
}


void bmp24_writePixelValue (t_bmp24 * image, int x, int y, FILE * file) {
    uint32_t pixelDataOffset = image->header.offset;
    uint32_t rowSize = ((image->colorDepth * image->width + 31) / 32) * 4;
    uint32_t pixelOffset = pixelDataOffset + (image->height - 1 - y) * rowSize + x * (image->colorDepth / 8);

    uint8_t bgr[3];
    bgr[0] = image->data[y][x].blue;
    bgr[1] = image->data[y][x].green;
    bgr[2] = image->data[y][x].red;

    file_rawWrite(pixelOffset, bgr, sizeof(uint8_t), 3, file);
}


void bmp24_readPixelData (t_bmp24 * image, FILE * file) {
    uint32_t pixelDataOffset = image->header.offset;
    uint32_t rowSize = ((image->colorDepth * image->width + 31) / 32) * 4;
    uint32_t bytesPerPixel = image->colorDepth / 8;

    uint8_t *row = (uint8_t *)malloc(rowSize);
    if (!row) {
        printf("Error: Memory allocation failed.\n");
        return;
    }

    for (int y = 0; y < image->height; y++) {
        uint32_t rowOffset = pixelDataOffset + (image->height - 1 - y) * rowSize;
        file_rawRead(rowOffset, row, sizeof(uint8_t), rowSize, file);

        for (int x = 0; x < image->width; x++) {
            uint32_t idx = x * bytesPerPixel;

            t_pixel pixel;
            pixel.blue = row[idx];
            pixel.green = row[idx + 1];
            pixel.red = row[idx + 2];

            image->data[y][x] = pixel;
        }
    }

    free(row);
}


void bmp24_writePixelData (t_bmp24 * image, FILE * file) {
    uint32_t pixelDataOffset = image->header.offset;
    uint32_t rowSize = ((image->colorDepth * image->width + 31) / 32) * 4;
    uint32_t bytesPerPixel = image->colorDepth / 8;

    uint8_t *row = (uint8_t *)malloc(rowSize);
    if (!row) {
        printf("Error: Memory allocation failed.\n");
        return;
    }

    for (int y = 0; y < image->height; y++) {
        memset(row, 0, rowSize);

        for (int x = 0; x < image->width; x++) {
            uint32_t idx = x * bytesPerPixel;
            row[idx] = image->data[y][x].blue;
            row[idx + 1] = image->data[y][x].green;
            row[idx + 2] = image->data[y][x].red;
        }

        uint32_t rowOffset = pixelDataOffset + (image->height - 1 - y) * rowSize;
        file_rawWrite(rowOffset, row, sizeof(uint8_t), rowSize, file);
    }

    free(row);
}


t_bmp24 * bmp24_loadImage (const char * filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Error: Could not open file %s for reading.\n", filename);
        return NULL;
    }

    /* Read basic metadata first to size allocations. */
    int32_t width = 0;
    int32_t height = 0;
    uint16_t depth = 0;
    file_rawRead(BITMAP_WIDTH, &width, sizeof(int32_t), 1, file);
    file_rawRead(BITMAP_HEIGHT, &height, sizeof(int32_t), 1, file);
    file_rawRead(BITMAP_DEPTH, &depth, sizeof(uint16_t), 1, file);

    t_bmp_header header;
    file_rawRead(BITMAP_MAGIC, &header, sizeof(t_bmp_header), 1, file);

    t_bmp_info info;
    file_rawRead(HEADER_SIZE, &info, sizeof(t_bmp_info), 1, file);

    if (header.type != BMP_TYPE || info.bits != 24) {
        printf("Error: Unsupported BMP format (expect 24-bit BMP).\n");
        fclose(file);
        return NULL;
    }

    t_bmp24 *image = bmp24_allocate(width, height, depth);
    if (!image) {
        fclose(file);
        printf("Error: Memory allocation failed.\n");
        return NULL;
    }

    image->header = header;
    image->header_info = info;
    image->width = width;
    image->height = height;
    image->colorDepth = depth;

    bmp24_readPixelData(image, file);

    printf("Image loaded successfully !\n");
    fclose(file);
    return image;
}

void bmp24_saveImage (t_bmp24 * img, const char * filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Error: Could not open file %s for writing.\n", filename);
        return;
    }

    if (img == NULL || img->data == NULL) {
        printf("Error: image is NULL.\n");
        fclose(file);
        return;
    }

    uint32_t rowSize = ((img->colorDepth * img->width + 31) / 32) * 4;
    uint32_t imageSize = rowSize * img->height;

    img->header.type = BMP_TYPE;
    img->header.offset = HEADER_SIZE + INFO_SIZE; // 54 bytes
    img->header.size = img->header.offset + imageSize;
    img->header.reserved1 = 0;
    img->header.reserved2 = 0;

    img->header_info.size = INFO_SIZE;
    img->header_info.width = img->width;
    img->header_info.height = img->height;
    img->header_info.planes = 1;
    img->header_info.bits = img->colorDepth;
    img->header_info.compression = 0;
    img->header_info.iamgesize = imageSize;
    // keep existing resolutions / colors if present

    fseek(file, 0, SEEK_SET);
    fwrite(&img->header, sizeof(t_bmp_header), 1, file);
    fwrite(&img->header_info, sizeof(t_bmp_info), 1, file);

    bmp24_writePixelData(img, file);

    fclose(file);
}


void bmp24_negative (t_bmp24 * img) {
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            img->data[y][x].red = 255 - img->data[y][x].red;
            img->data[y][x].green = 255 - img->data[y][x].green;
            img->data[y][x].blue = 255 - img->data[y][x].blue;
        }
    }
}


void bmp24_grayscale (t_bmp24 * img) {
    if (img == NULL) {
        printf("Error: image is NULL.\n");
        return;
    }

    if (img->data == NULL) {
        printf("Error: image data is NULL.\n");
        return;
    }

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            t_pixel *p = &img->data[y][x];
            uint8_t gray = (uint8_t)(0.299f * p->red +
                                     0.587f * p->green +
                                     0.114f * p->blue);
            p->red = gray;
            p->green = gray;
            p->blue = gray;
        }
    }
}


void bmp24_brightness (t_bmp24 * img, int adjustment) {
    if (img == NULL) {
        printf("Error: image is NULL.\n");
        return;
    }

    if (img->data == NULL) {
        printf("Error: image data is NULL.\n");
        return;
    }

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int r = img->data[y][x].red + adjustment;
            int g = img->data[y][x].green + adjustment;
            int b = img->data[y][x].blue + adjustment;

            if (r > 255) {
                r = 255;
            } else if (r < 0) {
                r = 0;
            }

            if (g > 255) {
                g = 255;
            } else if (g < 0) {
                g = 0;
            }

            if (b > 255) {
                b = 255;
            } else if (b < 0) {
                b = 0;
            }

            img->data[y][x].red = (uint8_t)r;
            img->data[y][x].green = (uint8_t)g;
            img->data[y][x].blue = (uint8_t)b;
        }
    }
}


t_pixel bmp24_convolution (t_bmp24 * img, int x, int y, float ** kernel, int kernelSize) {

    t_pixel result = {0, 0, 0};
    if (img == NULL || img->data == NULL || kernel == NULL || kernelSize <= 0) {
        return result;
    }

    int half = kernelSize / 2;
    float redSum = 0.0f;
    float greenSum = 0.0f;
    float blueSum = 0.0f;

    for (int ky = -half; ky <= half; ky++) {
        int ny = y + ky;
        if (ny < 0 || ny >= img->height) {
            continue;
        }

        for (int kx = -half; kx <= half; kx++) {
            int nx = x + kx;
            if (nx < 0 || nx >= img->width) {
                continue;
            }

            float weight = kernel[ky + half][kx + half];
            t_pixel p = img->data[ny][nx];

            redSum += p.red * weight;
            greenSum += p.green * weight;
            blueSum += p.blue * weight;
        }
    }

    if (redSum < 0.0f) redSum = 0.0f;
    if (redSum > 255.0f) redSum = 255.0f;
    if (greenSum < 0.0f) greenSum = 0.0f;
    if (greenSum > 255.0f) greenSum = 255.0f;
    if (blueSum < 0.0f) blueSum = 0.0f;
    if (blueSum > 255.0f) blueSum = 255.0f;

    result.red = (uint8_t)redSum;
    result.green = (uint8_t)greenSum;
    result.blue = (uint8_t)blueSum;

    return result;
}


void bmp24_applyFilter(t_bmp24 * img, float ** kernel, int kernelSize) {
    if (img == NULL || img->data == NULL || kernel == NULL || kernelSize <= 0) {
        printf("Error: Invalid parameters for filter application.\n");
        return;
    }

    t_pixel **source = bmp24_allocateDataPixels(img->width, img->height);
    if (!source) {
        printf("Error: could not allocate temporary buffer.\n");
        return;
    }

    size_t rowBytes = (size_t)img->width * sizeof(t_pixel);
    for (int y = 0; y < img->height; y++) {
        memcpy(source[y], img->data[y], rowBytes);
    }

    t_bmp24 copy = *img;
    copy.data = source;

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            img->data[y][x] = bmp24_convolution(&copy, x, y, kernel, kernelSize);
        }
    }

    bmp24_freeDataPixels(source, img->height);
}
