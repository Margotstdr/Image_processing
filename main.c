#include <stdio.h>
#include "bmp8.h"

int main() {
    t_bmp8 *image = NULL;
    int choice;
    int value;

    // Load the image
    image = bmp8_loadImage("barbara_gray.bmp");
    if (image == NULL) {
        printf("Failed to load image.\n");
        return 1; // Exit with error
    }

    printf("Original Image Information:\n");
    bmp8_printInfo(image);

    // Display the menu
    printf("\nChoose an option:\n");
    printf("1. Apply Negative filter\n");
    printf("2. Adjust Brightness\n");
    printf("3. Apply Threshold (black & white)\n");
    printf("4. Quit\n");
    printf("Your choice: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            bmp8_negative(image);
        printf("Negative filter applied.\n");
        bmp8_saveImage("barbara_gray_negative.bmp", image);
        printf("Saved as barbara_gray_negative.bmp\n");
        break;
        case 2:
            printf("Enter brightness adjustment value (positive or negative): ");
        scanf("%d", &value);
        bmp8_brightness(image, value);
        printf("Brightness adjusted.\n");
        bmp8_saveImage("barbara_gray_brightness.bmp", image);
        printf("Saved as barbara_gray_brightness.bmp\n");
        break;
        case 3:
            printf("Enter threshold value (0-255): ");
        scanf("%d", &value);
        bmp8_threshold(image, value);
        printf("Threshold filter applied.\n");
        bmp8_saveImage("barbara_gray_threshold.bmp", image);
        printf("Saved as barbara_gray_threshold.bmp\n");
        break;
        case 4:
            printf("Quitting program.\n");
        bmp8_free(image);
        return 0;
        default:
            printf("Invalid choice.\n");
        bmp8_free(image);
        return 1;
    }

    // Free the memory
    bmp8_free(image);
    printf("Memory freed. Program finished.\n");

    return 0; // Success
}