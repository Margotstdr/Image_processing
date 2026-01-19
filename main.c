#include <stdio.h>
#include <string.h>
#include "bmp8/bmp8.h"
#include "bmp24/bmp24.h"
#include "histogram/histogram.h"

int main() {
    int choice;
    int value;
    char inputPath[256];
    char outputPath[256];    
    int filterChoice;
    float kernel_box[3][3] = {
        {1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f},
        {1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f},
        {1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f}
    };
    float *kernel_box_ptr[3] = { kernel_box[0], kernel_box[1], kernel_box[2] };

    float kernel_gauss[3][3] = {
        {1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f},
        {2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f},
        {1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f}
    };
    float *kernel_gauss_ptr[3] = { kernel_gauss[0], kernel_gauss[1], kernel_gauss[2] };

    float kernel_sharp[3][3] = {
        { 0.0f, -1.0f, 0.0f }, 
        { -1.0f, 5.0f, -1.0f }, 
        { 0.0f, -1.0f, 0.0f }
    };
    float *kernel_sharp_ptr[3] = { kernel_sharp[0], kernel_sharp[1], kernel_sharp[2] };

    float kernel_outline[3][3] =  {
        { -1.0f, -1.0f, -1.0f }, 
        { -1.0f, 8.0f, -1.0f }, 
        { -1.0f, -1.0f, -1.0f }
    };
    float *kernel_outline_ptr[3] = { kernel_outline[0], kernel_outline[1], kernel_outline[2] };

    float kernel_emboss[3][3] = {
        { -2.0f, -1.0f, 0.0f }, 
        { -1.0f, 1.0f, 1.0f }, 
        { 0.0f, 1.0f, 2.0f }
    };
    float *kernel_emboss_ptr[3] = { kernel_emboss[0], kernel_emboss[1], kernel_emboss[2] };

    int format;
    t_bmp8 *image8 = NULL;
    t_bmp24 *image24 = NULL;

    do {
        printf("Please choose the image format:\n");
        printf("1. 8-bit BMP (black & white)\n");
        printf("2. 24-bit BMP (color)\n");
        scanf("%d", &format);

        if (format == 1) {

            int run8 = 1;
            while (run8) {

                printf("\nPlease choose an option:\n");
                printf("1. Open an image\n");
                printf("2. Save an image\n");
                printf("3. Apply a filter\n");
                printf("4. Display image information\n");
                printf("5. Histogram equalization\n");
                printf("6. Quit\n");
                printf("Your choice: ");
                scanf("%d", &choice);

                switch (choice) {

                    case 1: {
                        
                        printf("\nFile path: ");
                        scanf("%s", inputPath);

                        image8 = bmp8_loadImage(inputPath);
                                        
                        break;
                    }

                    case 2: {
                        
                        printf("\nFile path: ");
                        scanf("%s", outputPath);

                        bmp8_saveImage(outputPath, image8);
                                
                        break;
                    }

                    case 3: {
                        
                        int exitFilters = 0;

                        while (!exitFilters) {
                            printf("\nPlease choose a filter:\n");
                            printf("1. Negative\n");
                            printf("2. Brightness\n");
                            printf("3. Black and white\n");
                            printf("4. Box blur\n");
                            printf("5. Gaussian blur\n");
                            printf("6. Sharpness\n");
                            printf("7. Outline\n");
                            printf("8. Emboss\n");
                            printf("9. Return to the previous menu\n");
                            printf("Your choice: ");
                            scanf("%d", &filterChoice);

                            switch(filterChoice) {

                                case 1: {

                                    bmp8_negative(image8);
                                    printf("\nFilter applied successfully!\n");

                                    exitFilters = 1;
                                    break;
                                }

                                case 2: {

                                    printf("\nEnter brightness adjustment value (positive or negative): ");
                                    scanf("%d", &value);

                                    bmp8_brightness(image8, value);
                                    printf("\nFilter applied successfully!\n");

                                    exitFilters = 1;
                                    break;
                                }

                                case 3: {

                                    // Handle black and white filter for 8-bit BMP
                                    printf("\nEnter threshold value (0-255): ");
                                    scanf("%d", &value);

                                    bmp8_threshold(image8, value);

                                    printf("Filter applied successfully!\n");

                                    exitFilters = 1;
                                    break;
                                }

                                case 4: {//box blur

                                    bmp8_applyFilter(image8, kernel_box_ptr, 3);
                                    printf("\nFilter applied successfully!\n");

                                    exitFilters = 1;
                                    break;
                                }

                                case 5: {//gaussian blur

                                    bmp8_applyFilter(image8, kernel_gauss_ptr, 3);
                                    printf("\nFilter applied successfully!\n");

                                    exitFilters = 1;                                   
                                    break;
                                }

                                case 6: {//sharpness

                                    bmp8_applyFilter(image8, kernel_sharp_ptr, 3);
                                    printf("\nFilter applied successfully!\n");

                                    exitFilters = 1;
                                    break;
                                }

                                case 7: {//outline

                                    bmp8_applyFilter(image8, kernel_outline_ptr, 3);
                                    printf("\nFilter applied successfully!\n");

                                    exitFilters = 1;
                                    break;
                                }

                                case 8: {//emboss

                                    bmp8_applyFilter(image8, kernel_emboss_ptr, 3);
                                    printf("\nFilter applied successfully!\n");
                                    
                                    exitFilters = 1;
                                    break;
                                }

                                case 9: {//return to previous menu
                                    exitFilters = 1;
                                    break;
                                }

                                default: {
                                    printf("Invalid choice.\n");
                                    break;
                                }
                            }
                        }
                        break;
                    }
                    
                    case 4: {

                            bmp8_printInfo(image8);
                            break;
                    }

                    case 5: {

                        if (!image8) {
                            printf("No image loaded. Please open an image first.\n");
                            break;
                        }

                        bmp8_equalize(image8, NULL);
                        printf("\nHistogram equalization applied successfully!\n");

                        break;
                    }

                    case 6: {

                        printf("\nQuitting program.\n");
                        bmp8_free(image8);
                        run8 = 0;

                        break;
                    }

                    default: {
                            printf("\nChoix invalide.\n");
                            break;
                    }
                }

            }
        
            //return 0;

        } else if (format == 2) {

            int run24 = 1;
            while (run24) {
                printf("\nPlease choose an option:\n");
                printf("1. Open an image\n");
                printf("2. Save an image\n");
                printf("3. Apply a filter\n");
                printf("4. Histogram equalization\n");
                printf("5. Quit\n");
                printf("Your choice: ");
                scanf("%d", &choice);

                switch(choice) {

                    case 1: {

                        printf("\nFile path: ");
                        scanf("%s", inputPath);

                        image24 = bmp24_loadImage(inputPath);
                        
                        break;
                    }

                    case 2: {

                        printf("\nFile path: ");
                        scanf("%s", outputPath);

                        bmp24_saveImage(image24, outputPath);
                                
                        break;
                    }

                    case 3: {
                        int exitFilters = 0;

                        while (!exitFilters) {
                            printf("\n Please choose a filter:\n");
                            printf("1. Negative\n");
                            printf("2. Grayscale\n");
                            printf("3. Brightness\n");
                            printf("4. Box blur\n");
                            printf("5. Gaussian blur\n");
                            printf("6. Sharpness\n");
                            printf("7. Outline\n");
                            printf("8. Emboss\n");
                            printf("9. Return to the previous menu\n");
                            printf("Enter your choice: ");
                            scanf("%d", &filterChoice);

                            switch(filterChoice) {

                                case 1: {

                                    bmp24_negative(image24);
                                    printf("\nFilter applied successfully!\n");

                                    exitFilters = 1;
                                    break;
                                }

                                case 2: {

                                    bmp24_grayscale(image24);
                                    printf("\nFilter applied successfully!\n");

                                    exitFilters = 1;
                                    break;
                                }

                                case 3: {

                                    printf("\nEnter brightness adjustment value (positive or negative): ");
                                    scanf("%d", &value);

                                    bmp24_brightness(image24, value);
                                    printf("\nFilter applied successfully!\n");

                                    exitFilters = 1;
                                    break;
                                }

                                case 4: { // box blur

                                    bmp24_applyFilter(image24, kernel_box_ptr, 3);
                                    printf("\nFilter applied successfully!\n");

                                    exitFilters = 1;
                                    break;
                                }

                                case 5: { // gaussian blur

                                    bmp24_applyFilter(image24, kernel_gauss_ptr, 3);
                                    printf("\nFilter applied successfully!\n");

                                    exitFilters = 1;
                                    break;
                                }

                                case 6: { // sharpness

                                    bmp24_applyFilter(image24, kernel_sharp_ptr, 3);
                                    printf("\nFilter applied successfully!\n");

                                    exitFilters = 1;
                                    break;
                                }

                                case 7: { // outline

                                    bmp24_applyFilter(image24, kernel_outline_ptr, 3);
                                    printf("\nFilter applied successfully!\n");

                                    exitFilters = 1;
                                    break;
                                }

                                case 8: { // emboss

                                    bmp24_applyFilter(image24, kernel_emboss_ptr, 3);
                                    printf("\nFilter applied successfully!\n");

                                    exitFilters = 1;
                                    break;
                                }

                                case 9: {//return to previous menu
                                    exitFilters = 1;
                                    break;
                                }

                                default: {
                                    printf("Invalid choice.\n");
                                    break;
                                }
                            }
                        }

                        break;
                    }

                    case 4: {

                        if (!image24) {
                            printf("No image loaded. Please open an image first.\n");
                            break;
                        }

                        bmp24_equalize(image24);
                        printf("\nHistogram equalization applied successfully!\n");

                        break;
                    }

                    case 5: {

                        printf("\nQuitting program.\n");
                        bmp24_free(image24);
                        run24 = 0;

                        break;
                    }

                    default: {
                        printf("Invalid choice.\n");
                        break;
                    }
                }

            }
            
        } else {
            printf("Invalid format choice. Please try again.\n");
        }

    } while (format != 1 && format != 2);

    return 0;
}
