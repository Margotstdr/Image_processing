#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "histogram.h"

static unsigned char clamp_to_byte(double value) {
    if (value < 0.0) return 0;
    if (value > 255.0) return 255;
    return (unsigned char)lround(value);
}

unsigned int * bmp8_computeHistogram(t_bmp8 *img) {
    if (!img || !img->data) {
        printf("Error: invalid image provided for histogram computation.\n");
        return NULL;
    }

    unsigned int *histogram = (unsigned int *)calloc(256, sizeof(unsigned int));
    if (!histogram) {
        printf("Error: Memory allocation failed for histogram.\n");
        return NULL;
    }

    if (img->colorDepth != 8) {
        printf("Error: bmp8_computeHistogram expects an 8-bit image.\n");
        free(histogram);
        return NULL;
    }

    unsigned int bytesPerPixel = img->colorDepth / 8;
    unsigned int rowSize = ((img->width * bytesPerPixel) + 3) & ~3; // rows are 4-byte aligned

    for (unsigned int y = 0; y < img->height; y++) {
        unsigned int rowOffset = y * rowSize;
        for (unsigned int x = 0; x < img->width; x++) {
            unsigned char pixelValue = img->data[rowOffset + x];
            histogram[pixelValue]++;
        }
    }

    return histogram;
}


unsigned int * bmp8_computeCDF(unsigned int *hist) {
    if (!hist) {
        printf("Error: invalid histogram provided for CDF computation.\n");
        return NULL;
    }

    unsigned int *hist_eq = (unsigned int *)calloc(256, sizeof(unsigned int));
    if (!hist_eq) {
        printf("Error: Memory allocation failed for histogram equalization.\n");
        return NULL;
    }

    unsigned long long cdf[256] = {0};
    unsigned long long cumulative = 0;
    unsigned long long cdf_min = 0;

    for (int i = 0; i < 256; i++) {
        cumulative += hist[i];
        cdf[i] = cumulative;
        if (cdf_min == 0 && cdf[i] != 0) {
            cdf_min = cdf[i];
        }
    }

    unsigned long long totalPixels = cumulative;
    if (totalPixels == 0) {
        // Empty histogram; mapping stays zero.
        return hist_eq;
    }

    if (cdf_min == totalPixels) {
        // All pixels share the same intensity; keep values unchanged.
        for (int i = 0; i < 256; i++) {
            hist_eq[i] = (unsigned int)i;
        }
        return hist_eq;
    }

    double denom = (double)(totalPixels - cdf_min);
    for (int i = 0; i < 256; i++) {
        double normalized = ((double)cdf[i] - (double)cdf_min) / denom;
        double mapped = round(normalized * 255.0);
        if (mapped < 0.0) mapped = 0.0;
        if (mapped > 255.0) mapped = 255.0;
        hist_eq[i] = (unsigned int)mapped;
    }

    return hist_eq;
}

void bmp8_equalize(t_bmp8 *img, unsigned int *hist_eq) {
    if (!img || !img->data) {
        printf("Error: invalid image provided for equalization.\n");
        return;
    }

    if (img->colorDepth != 8) {
        printf("Error: bmp8_equalize expects an 8-bit image.\n");
        return;
    }

    unsigned int *local_hist = NULL;
    unsigned int *local_hist_eq = NULL;

    unsigned int *mapping = hist_eq;
    if (!mapping) {
        local_hist = bmp8_computeHistogram(img);
        if (!local_hist) {
            return;
        }
        local_hist_eq = bmp8_computeCDF(local_hist);
        free(local_hist);
        if (!local_hist_eq) {
            return;
        }
        mapping = local_hist_eq;
    }

    unsigned int bytesPerPixel = img->colorDepth / 8;
    unsigned int rowSize = ((img->width * bytesPerPixel) + 3) & ~3;

    for (unsigned int y = 0; y < img->height; y++) {
        unsigned int rowOffset = y * rowSize;
        for (unsigned int x = 0; x < img->width; x++) {
            unsigned char oldVal = img->data[rowOffset + x];
            unsigned int mapped = mapping[oldVal];
            if (mapped > 255) mapped = 255;
            img->data[rowOffset + x] = (unsigned char)mapped;
        }
    }

    free(local_hist_eq);
}

void bmp24_equalize(t_bmp24 *img) {
    if (!img || !img->data) {
        printf("Error: invalid image provided for equalization.\n");
        return;
    }

    if (img->colorDepth != 24) {
        printf("Error: bmp24_equalize expects a 24-bit image.\n");
        return;
    }

    int width = img->width;
    int height = img->height;
    size_t pixelCount = (size_t)width * (size_t)height;

    double *Y = (double *)malloc(pixelCount * sizeof(double));
    double *U = (double *)malloc(pixelCount * sizeof(double));
    double *V = (double *)malloc(pixelCount * sizeof(double));
    if (!Y || !U || !V) {
        printf("Error: Memory allocation failed for YUV buffers.\n");
        free(Y);
        free(U);
        free(V);
        return;
    }

    unsigned int hist[256] = {0};

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            size_t idx = (size_t)y * (size_t)width + (size_t)x;
            t_pixel p = img->data[y][x];

            double yVal = 0.299 * p.red + 0.587 * p.green + 0.114 * p.blue;
            double uVal = -0.14713 * p.red - 0.28886 * p.green + 0.436 * p.blue;
            double vVal = 0.615 * p.red - 0.51499 * p.green - 0.10001 * p.blue;

            Y[idx] = yVal;
            U[idx] = uVal;
            V[idx] = vVal;

            int yIndex = (int)lround(yVal);
            if (yIndex < 0) yIndex = 0;
            if (yIndex > 255) yIndex = 255;
            hist[yIndex]++;
        }
    }

    unsigned int uniqueLevels = 0;
    for (int i = 0; i < 256; i++) {
        if (hist[i] != 0) {
            uniqueLevels++;
        }
    }

    if (uniqueLevels <= 1) {
        // Flat image; nothing to equalize.
        free(Y);
        free(U);
        free(V);
        return;
    }

    unsigned int *mapping = bmp8_computeCDF(hist);
    if (!mapping) {
        free(Y);
        free(U);
        free(V);
        return;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            size_t idx = (size_t)y * (size_t)width + (size_t)x;

            int yIndex = (int)lround(Y[idx]);
            if (yIndex < 0) yIndex = 0;
            if (yIndex > 255) yIndex = 255;

            double newY = (double)mapping[yIndex];
            double newR = newY + 1.13983 * V[idx];
            double newG = newY - 0.39465 * U[idx] - 0.58060 * V[idx];
            double newB = newY + 2.03211 * U[idx];

            img->data[y][x].red = clamp_to_byte(newR);
            img->data[y][x].green = clamp_to_byte(newG);
            img->data[y][x].blue = clamp_to_byte(newB);
        }
    }

    free(mapping);
    free(Y);
    free(U);
    free(V);
}
