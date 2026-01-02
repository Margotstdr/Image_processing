#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bmp8.h"

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
    unsigned int img_long = fread(img->header, sizeof(unsigned char), 54, file);

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

    unsigned int dataOffset = *(unsigned int*)&img->header[10];
    unsigned int paletteSize = (dataOffset > 54) ? dataOffset - 54 : 0;

    if (paletteSize > 0) {
        size_t paletteRead = paletteSize > sizeof(img->colorTable) ? sizeof(img->colorTable) : paletteSize;
        fread(img->colorTable, sizeof(unsigned char), paletteRead, file);
        if (paletteSize > paletteRead) {
            fseek(file, dataOffset, SEEK_SET);
        }
    }

    unsigned int bytesPerPixel = img->colorDepth / 8;
    unsigned int rowSize = ((img->width * bytesPerPixel) + 3) & ~3; // rows are 4-byte aligned
    unsigned int computedDataSize = rowSize * img->height;

    if (img->dataSize == 0 || img->dataSize < computedDataSize) {
        img->dataSize = computedDataSize;
    }

    img->data = (unsigned char*)malloc(img->dataSize);

    if (!img->data) {
        printf("Error: Memory allocation for image data failed.\n");
        free(img);
        fclose(file);
        return NULL;
    }

    fseek(file, dataOffset, SEEK_SET);
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

    unsigned int dataOffset = *(unsigned int*)&img->header[10];
    unsigned int paletteSize = (dataOffset > 54) ? dataOffset - 54 : 0;
    unsigned int bytesPerPixel = img->colorDepth / 8;
    unsigned int rowSize = ((img->width * bytesPerPixel) + 3) & ~3;
    img->dataSize = rowSize * img->height;

    unsigned int fileSize = dataOffset + img->dataSize;
    memcpy(&img->header[2], &fileSize, sizeof(unsigned int));
    memcpy(&img->header[34], &img->dataSize, sizeof(unsigned int));

    fwrite(img->header, sizeof(unsigned char), 54, file);

    if (paletteSize > 0) {
        fwrite(img->colorTable, sizeof(unsigned char), paletteSize, file);
    }

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

    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = 255 - img->data[i];
    }

}


void bmp8_brightness(t_bmp8* img, int value) {

    for (unsigned int i = 0; i < img->dataSize; i++) {

        unsigned int pixel = img->data[i] + value;

        if (pixel > 255) {
            pixel = 255;
        } else if (pixel < 0) {
            pixel = 0;
        }

        img->data[i] = (unsigned char)pixel;
    }

}


void bmp8_threshold(t_bmp8* img, int threshold) {

    for (unsigned int i = 0; i < img->dataSize; i++) {
        
        if (img->data[i] >= threshold) {
            img->data[i] = 255;
        } else {
            img->data[i] = 0;
        }
    }
}


void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize) {
    // 1. CONVERSION EN INT (Crucial pour éviter le bug des boucles infinies)
    int w = (int)img->width;
    int h = (int)img->height;
    int n = kernelSize / 2;

    // 2. BUFFER TEMPORAIRE
    unsigned char *tempData = (unsigned char *)malloc(img->dataSize);
    if (tempData == NULL) return;
    
    // On copie l'image originale (pour garder les bords intacts)
    memcpy(tempData, img->data, img->dataSize);

    // 3. CONVOLUTION
    // On utilise bien des 'int' partout
    for (int y = n; y < h - n; y++) {
        for (int x = n; x < w - n; x++) {
            
            float sum = 0.0f;

            // Boucles du noyau (kernel) avec 'int' obligatoirement
            for (int ky = -n; ky <= n; ky++) {
                for (int kx = -n; kx <= n; kx++) {
                    
                    // Calcul des indices
                    int currentY = y + ky;
                    int currentX = x + kx;
                    
                    // Récupération des valeurs
                    int pixelIdx = (currentY * w) + currentX;
                    unsigned char pixelVal = img->data[pixelIdx];
                    float kernelVal = kernel[ky + n][kx + n];

                    // Accumulation
                    sum += pixelVal * kernelVal;
                }
            }

            // 4. SATURATION (Clamping)
            if (sum < 0.0f) sum = 0.0f;
            if (sum > 255.0f) sum = 255.0f;

            // 5. ÉCRITURE DU RÉSULTAT
            // C'est cette ligne qui manquait ou qui ne s'exécutait pas !
            tempData[y * w + x] = (unsigned char)sum;
        }
    }

    // 6. COPIE FINALE
    memcpy(img->data, tempData, img->dataSize);
    free(tempData);
}
