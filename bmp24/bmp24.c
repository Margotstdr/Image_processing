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















