#include <stdio.h>
#include "bmp8/bmp8.h"

int main() {
    t_bmp8 *image = NULL;
    int choice;
    int value;
    char filepath[256];
    int filterChoice;
    float val;
    float matrix[3][3];
    float *kernel[3];


    do {
        printf("\nPlease choose an option:\n");
        printf("1. Open an image\n");
        printf("2. Save an image\n");
        printf("3. Apply a filter\n");
        printf("4. Display image information\n");
        printf("5. Quit\n");
        printf("Your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: 
                printf("\nFile path: ");
                scanf("%s", filepath);

                image = bmp8_loadImage(filepath);

                break;

            case 2: 
                printf("\nFile path: ");
                scanf("%s", filepath);

                bmp8_saveImage(filepath, image);

                break;

            case 3: 
            {
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

                        case 1: 

                            bmp8_negative(image);
                            printf("\nFilter applied successfully!\n");
                            exitFilters = 1;

                            break;

                        case 2: 

                            printf("\nEnter brightness adjustment value (positive or negative): ");
                            scanf("%d", &value);

                            bmp8_brightness(image, value);
                            printf("\nFilter applied successfully!\n");
                            exitFilters = 1;

                            break;

                        case 3: 

                            printf("\nEnter threshold value (0-255): ");
                            scanf("%d", &value);

                            bmp8_threshold(image, value);
                            printf("Filter applied successfully!\n");
                            exitFilters = 1;

                            break;

                        case 4: //box blur

                            val = 1.0f / 9.0f;

                            for(int i = 0; i < 3; i++) {
                                for(int j = 0; j < 3; j++) {

                                    matrix[i][j] = val;
                                }
                            }

                            kernel[0] = matrix[0];
                            kernel[1] = matrix[1];
                            kernel[2] = matrix[2];

                            bmp8_applyFilter(image, kernel, 3);
                            printf("\nFilter applied successfully!\n");
                            exitFilters = 1;

                            break;

                        case 5: //gaussian blur
                            
                            val = 1.0f / 16.0f;

                            matrix[0][0] = 1.0f * val;
                            matrix[0][1] = 2.0f * val;
                            matrix[0][2] = 1.0f * val;

                            matrix[1][0] = 2.0f * val;
                            matrix[1][1] = 4.0f * val;
                            matrix[1][2] = 2.0f * val;

                            matrix[2][0] = 1.0f * val;
                            matrix[2][1] = 2.0f * val;
                            matrix[2][2] = 1.0f * val;

                            kernel[0] = matrix[0];
                            kernel[1] = matrix[1];
                            kernel[2] = matrix[2];

                            bmp8_applyFilter(image, kernel, 3);
                            printf("\nFilter applied successfully!\n");
                            exitFilters = 1;
                            
                            break;

                        case 6: //sharpness

                            matrix[0][0] = 0.0f; 
                            matrix[0][1] = -1.0f;
                            matrix[0][2] = 0.0f;

                            matrix[1][0] = -1.0f;
                            matrix[1][1] = 5.0f;
                            matrix[1][2] = -1.0f;

                            matrix[2][0] = 0.0f;
                            matrix[2][1] = -1.0f;
                            matrix[2][2] = 0.0f;

                            kernel[0] = matrix[0];
                            kernel[1] = matrix[1];
                            kernel[2] = matrix[2];

                            bmp8_applyFilter(image, kernel, 3);
                            printf("\nFilter applied successfully!\n");
                            exitFilters = 1;

                            break;

                        case 7: //outline

                            matrix[0][0] = -1.0f;
                            matrix[0][1] = -1.0f;
                            matrix[0][2] = -1.0f;

                            matrix[1][0] = -1.0f;
                            matrix[1][1] = 8.0f;
                            matrix[1][2] = -1.0f;

                            matrix[2][0] = -1.0f;
                            matrix[2][1] = -1.0f;
                            matrix[2][2] = -1.0f;

                            kernel[0] = matrix[0];
                            kernel[1] = matrix[1];
                            kernel[2] = matrix[2];

                            bmp8_applyFilter(image, kernel, 3);
                            printf("\nFilter applied successfully!\n");
                            exitFilters = 1;

                            break;

                        case 8: //emboss

                            matrix[0][0] = -2.0f;
                            matrix[0][1] = -1.0f;
                            matrix[0][2] = 0.0f;

                            matrix[1][0] = -1.0f;
                            matrix[1][1] = 1.0f;
                            matrix[1][2] = 1.0f;

                            matrix[2][0] = 0.0f;
                            matrix[2][1] = 1.0f;
                            matrix[2][2] = 2.0f;

                            kernel[0] = matrix[0];
                            kernel[1] = matrix[1];
                            kernel[2] = matrix[2];

                            bmp8_applyFilter(image, kernel, 3);
                            printf("\nFilter applied successfully!\n");
                            exitFilters = 1;

                            break;

                        case 9: //return to previous menu
                            exitFilters = 1;
                            break;

                        default:

                            printf("Invalid choice.\n");

                            break;
                    }

                }
            }
                
                break;

            case 4: 

                bmp8_printInfo(image);

                break;

            case 5: 

                printf("\nQuitting program.\n");
                bmp8_free(image);

                break;

            default: 
                    printf("\nChoix invalide.\n");
                    break;
        }

    } while (choice != 5);

    bmp8_free(image);

    return 0;
}
