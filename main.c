#include <stdio.h>
#include "bmp8.h"

int main() {
    t_bmp8 *image = NULL;
    int choice;
    int value;
    char filepath[256];
    int filterChoice;
    float val;
    float matrix[3][3];
    float *kernel[3];

    // Load the image
    image = bmp8_loadImage("barbara_gray.bmp");
    if (image == NULL) {
        printf("Failed to load image.\n");
        return 1; // Exit with error
    }

    printf("Original Image Information:\n");
    bmp8_printInfo(image);

    do {
        printf("Please choose an option:\n");
        printf("1. Open an image\n");
        printf("2. Save an image\n");
        printf("3.Apply a filter\n");
        printf("4. Display image information\n");
        printf("5. Quit\n");
        printf("Your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: 
                printf("File path: ");
                scanf("%s", filepath);

                image = bmp8_loadImage(filepath);

                break;

            case 2: 
                printf("File path: ");
                scanf("%s", filepath);

                bmp8_saveImage(filepath, image);

                break;

            case 3: 
                do {
                    printf("Please choose a filter:\n");
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
                            printf("Filter applied successfully!\n");

                            break;

                        case 2: 

                            printf("Enter brightness adjustment value (positive or negative): ");
                            scanf("%d", &value);

                            bmp8_brightness(image, value);
                            printf("Filter applied successfully!\n");

                            break;

                        case 3: 

                            printf("Enter threshold value (0-255): ");
                            scanf("%d", &value);

                            bmp8_threshold(image, value);
                            printf("Filter applied successfully!\n");

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
                            printf("Filter applied successfully!\n");

                            break;

                        case 5: //gaussian blur
                            
                            val = 1.0f / 16.0F;

                            float matrix[3][3] = {
                                { 1*val, 2*val, 1*val },
                                { 2*val, 4*val, 2*val },
                                { 1*val, 2*val, 1*val }
                            };

                            kernel[0] = matrix[0];
                            kernel[1] = matrix[1];
                            kernel[2] = matrix[2];

                            bmp8_applyFilter(image, kernel, 3);
                            printf("Filter applied successfully!\n");
                            
                            break;

                        case 6: //sharpness

                            float matrix[3][3] = {
                                {0.0f, -1.0f, -0.0f},
                                {-1.0f, 5.0f, -1.0f},
                                {0.0f, -1.0f, 0.0f}
                            };

                            kernel[0] = matrix[0];
                            kernel[1] = matrix[1];
                            kernel[2] = matrix[2];

                            bmp8_applyFilter(image, kernel, 3);
                            printf("Filter applied successfully!\n");

                            break;

                        case 7: //outline
                            
                            float matrix[3][3] = {
                                {-1.0f, -1.0f, -1.0f},
                                {-1.0f, 8.0f, -1.0f},
                                {-1.0f, -1.0f, -1.0f}
                            };

                            kernel[0] = matrix[0];
                            kernel[1] = matrix[1];
                            kernel[2] = matrix[2];

                            bmp8_applyFilter(image, kernel, 3);
                            printf("Filter applied successfully!\n");

                            break;

                        case 8: //emboss

                            float matrix[3][3] = {
                                {-2.0f, -1.0f, 0.0f},
                                {-1.0f, 1.0f, 1.0f},
                                {0.0f, 1.0f, 2.0f}
                            };

                            kernel[0] = matrix[0];
                            kernel[1] = matrix[1];
                            kernel[2] = matrix[2];

                            bmp8_applyFilter(image, kernel, 3);
                            printf("Filter applied successfully!\n");

                            break;

                        case 9: //return to previous menu
                            break;

                        default:

                            printf("Invalid choice.\n");

                            break;
                    }

                }while (filterChoice != 9);
                
                break;

            case 4: 

                bmp8_printInfo(image);

                break;

            case 5: 

                printf("Quitting program.\n");
                bmp8_free(image);

                break;

            default: 
                    printf("Choix invalide.\n");
                    break;
            

        }

    } while (choice != 5);

    bmp8_free(image);

    return 0;
}