#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <windows.h>
#include "bmp8.h"
#include "bmp24.h"
#define PATH "..//"

float **createBoxBlurKernel() {
    int size = 3;
    float **kernel = malloc(size * sizeof(float *));
    if (!kernel) return NULL;

    for (int i = 0; i < size; i++) {
        kernel[i] = malloc(size * sizeof(float));
        if (!kernel[i]) return NULL;
        for (int j = 0; j < size; j++) {
            kernel[i][j] = 1.0f / 9.0f;
        }
    }
    return kernel;
}
float **createGaussianBlurKernel() {
    int size = 3;
    float values[3][3] = {
        {1, 2, 1},
        {2, 4, 2},
        {1, 2, 1}
    };

    float **kernel = malloc(size * sizeof(float *));
    for (int i = 0; i < size; i++) {
        kernel[i] = malloc(size * sizeof(float));
        for (int j = 0; j < size; j++) {
            kernel[i][j] = values[i][j] / 16.0f;
        }
    }
    return kernel;
}
float **createSharpenKernel() {
    int size = 3;
    float values[3][3] = {
        { 0, -1,  0},
        {-1,  5, -1},
        { 0, -1,  0}
    };

    float **kernel = malloc(size * sizeof(float *));
    for (int i = 0; i < size; i++) {
        kernel[i] = malloc(size * sizeof(float));
        for (int j = 0; j < size; j++) {
            kernel[i][j] = values[i][j];
        }
    }
    return kernel;
}
float **createEmbossKernel() {
    int size = 3;
    float values[3][3] = {
        {-2, -1, 0},
        {-1,  1, 1},
        { 0,  1, 2}
    };

    float **kernel = malloc(size * sizeof(float *));
    for (int i = 0; i < size; i++) {
        kernel[i] = malloc(size * sizeof(float));
        for (int j = 0; j < size; j++) {
            kernel[i][j] = values[i][j];
        }
    }
    return kernel;
}
float **createOutlineKernel() {
    int size = 3;
    float values[3][3] = {
        {-1, -1, -1},
        {-1,  8, -1},
        {-1, -1, -1}
    };

    float **kernel = malloc(size * sizeof(float *));
    for (int i = 0; i < size; i++) {
        kernel[i] = malloc(size * sizeof(float));
        for (int j = 0; j < size; j++) {
            kernel[i][j] = values[i][j];
        }
    }
    return kernel;
}





// Prototypes of the menu functions
void displayMainMenu();
void displayBMP8Menu();
void displayBMP24Menu();
void displayFilters8Menu();
void displayFilters24Menu();
void handleBMP8();
void handleBMP24();
void applyFiltersBMP8(t_bmp8* img);
void applyFiltersBMP24(t_bmp24* img);
void clearScreen();
void pauseScreen();

// Global variables for the loaded images
t_bmp8* imageBMP8 = NULL;
t_bmp24* imageBMP24 = NULL;

// Function to clear the screen
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Function to pause the program
void pauseScreen() {
    printf("\nClick on Enter to continue...");
    getchar();
    while (getchar() != '\n'); // Vider le buffer
}

// Main menu
void displayMainMenu() {
    printf("*========================================*\n");
    printf("|            BMP IMAGES MANAGER          |\n");
    printf("|========================================|\n");
    printf("|  1. Images BMP 8 bits (Grayscale)      |\n");
    printf("|  2. Images BMP 24 bits (Color)         |\n");
    printf("|  0. Quit                               |\n");
    printf("*========================================*\n");
    printf("Your choice: ");
}

// Menu for 8-bit BMP
void displayBMP8Menu() {
    printf("*========================================*\n");
    printf("|           MENU BMP 8 BITS              |\n");
    printf("|========================================|\n");
    printf("|  1. Load an Image                      |\n");
    printf("|  2. Save an Image                      |\n");
    printf("|  3. Display information                |\n");
    printf("|  4. Negative                           |\n");
    printf("|  5. Change Brightness                  |\n");
    printf("|  6. Threshold                          |\n");
    printf("|  7. Horizontal flip                    |\n");
    printf("|  8. Vertical flip                      |\n");
    printf("|  9. Apply filters                      |\n");
    printf("| 10. Histogram equalization             |\n");
    printf("| 11. Compute histogram                  |\n");
    printf("|  0. Back to main menu                  |\n");
    printf("*========================================*\n");
    printf("Your choice: ");
}

// Menu for 24-bit BMP
void displayBMP24Menu() {
    printf("*========================================*\n");
    printf("|           MENU BMP 24 BITS             |\n");
    printf("|========================================|\n");
    printf("|  1. Load an Image                      |\n");
    printf("|  2. Save an Image                      |\n");
    printf("|  3. Display information                |\n");
    printf("|  4. Negative                           |\n");
    printf("|  5. Grayscale                          |\n");
    printf("|  6. Change Brightness                  |\n");
    printf("|  7. Flip horizontally                  |\n");
    printf("|  8. Flip vertically                    |\n");
    printf("|  9. Apply filters                      |\n");
    printf("| 10. Histogram equalization             |\n");
    printf("|  0. Back to main menu                  |\n");
    printf("*========================================*\n");
    printf("Your choice: ");
}

// Filter menu for 8-bit BMP
void displayFilters8Menu() {
    printf("*========================================*\n");
    printf("|        FILTERS BMP 8 BITS              |\n");
    printf("|========================================|\n");
    printf("|  1. Box Blur                           |\n");
    printf("|  2. Gaussian Blur                      |\n");
    printf("|  3. Sharpen                            |\n");
    printf("|  4. Emboss                             |\n");
    printf("|  5. Outline                            |\n");
    printf("|  0. Back                               |\n");
    printf("*========================================*\n");
    printf("Your choice: ");
}

// Filter menu for 24-bit BMP
void displayFilters24Menu() {
    printf("*========================================*\n");
    printf("|        FILTERS BMP 24 BITS             |\n");
    printf("|========================================|\n");
    printf("|  1. Box Blur                           |\n");
    printf("|  2. Gaussian Blur                      |\n");
    printf("|  3. Sharpen                            |\n");
    printf("|  4. Emboss                             |\n");
    printf("|  5. Outline                            |\n");
    printf("|  6. Sepia tone                         |\n");
    printf("|  0. Back                               |\n");
    printf("*========================================*\n");
    printf("Your choice: ");
}

// Function to handle 8-bit BMP images
void handleBMP8() {
    int choix;
    char filename[256];
    int valeur;


    do {
        char path[256] = PATH;
        clearScreen();
        if (imageBMP8) {
            printf(" 8 bits Image Loaded: %dx%d pixels\n\n", imageBMP8->width, imageBMP8->height);
        } else {
            printf("No 8 bits Image Loaded\n\n");
        }

        displayBMP8Menu();
        scanf("%d", &choix);
        getchar(); // Consume the '\n'

        switch(choix) {
            case 1: // Load image

                printf("Filename to load: ");
                fgets(filename, sizeof(filename), stdin);
                filename[strcspn(filename, "\n")] = 0; // Remove the '\n'
                strcat(path,filename);

                if (imageBMP8) {
                    bmp8_freeImage(imageBMP8);
                }
                imageBMP8 = bmp8_loadImage(path);

                if (imageBMP8) {
                    printf("Image successfully loaded!\n");
                } else {
                    printf("Error loading the image.\n");
                }
                pauseScreen();
                break;

            case 2: // Save
                if (!imageBMP8) {
                    printf("No image loaded!\n");
                    pauseScreen();
                    break;
                }
                printf("Output file name: ");
                fgets(filename, sizeof(filename), stdin);
                filename[strcspn(filename, "\n")] = 0;
                strcat(path,filename);
                bmp8_saveImage(path, imageBMP8);
                pauseScreen();
                break;

            case 3: // Information
                if (!imageBMP8) {
                    printf("No image loaded!\n");
                } else {
                    bmp8_printInfo(imageBMP8);
                }
                pauseScreen();
                break;

            case 4: // Negative
                if (!imageBMP8) {
                    printf("No image loaded!\n");
                } else {
                    bmp8_negative(imageBMP8);
                    printf("Negative applied successfully!\n");
                }
                pauseScreen();
                break;

            case 5: // Brightness
                if (!imageBMP8) {
                    printf("No image loaded!\n");
                    pauseScreen();
                    break;
                }
                printf("Brightness value (-255 to 255): ");
                scanf("%d", &valeur);
                bmp8_brightness(imageBMP8, valeur);
                printf("Brightness adjusted by %d units!\n", valeur);
                pauseScreen();
                break;

            case 6: // Threshold
                if (!imageBMP8) {
                    printf("No image loaded!\n");
                    pauseScreen();
                    break;
                }
                printf("Threshold value (0-255): ");
                scanf("%d", &valeur);
                bmp8_threshold(imageBMP8, valeur);
                printf("Threshold applied with value %d!\n", valeur);
                pauseScreen();
                break;
            case 7: // Horizontal flip
                if (!imageBMP8)
                {
                    printf("No image loaded!\n");
                    pauseScreen();
                    break;
                }
                bmp8_horizontalFlip(imageBMP8);
                printf("Horizontal Flip applied");
                pauseScreen();
                break;
            case 8: // Vertical flip
                if (!imageBMP8)
                {
                    printf("No image loaded!\n");
                    pauseScreen();
                    break;
                }
                bmp8_verticalFlip(imageBMP8);
                printf("Vertical Flip applied");
                pauseScreen();
                break;
            case 9: // Filters
                if (!imageBMP8) {
                    printf("No image loaded!\n");
                    pauseScreen();
                } else {
                    applyFiltersBMP8(imageBMP8);
                }
                break;

            case 10: // Equalization
                if (!imageBMP8) {
                    printf("No image loaded!\n");
                } else {
                    bmp8_equalize(imageBMP8);
                    printf("Histogram equalization applied!\n");
                }
                pauseScreen();
                break;

            case 11: // Histogram
                if (!imageBMP8) {
                    printf("No image loaded!\n");
                } else {
                    unsigned int* hist = bmp8_computeHistogram(imageBMP8);
                    printf("Histogram calculated. Here are the first 10 values:\n");
                    for (int i = 0; i < 10; i++) {
                        printf("Niveau %d: %u pixels\n", i, hist[i]);
                    }
                    free(hist);
                }
                pauseScreen();
                break;

            case 0:
                break;

            default:
                printf("Invalid choice!\n");
                pauseScreen();
        }
    } while (choix != 0);
}

// Function to apply filters to 8-bit BMP images
void applyFiltersBMP8(t_bmp8* img) {
    int choix;
    float** kernel;

    do {
        clearScreen();
        displayFilters8Menu();
        scanf("%d", &choix);

        switch(choix) {
            case 1: // Box Blur
                kernel = createKernel(3);
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 3; j++) {
                        kernel[i][j] = 1.0f / 9.0f;
                    }
                }
                bmp8_applyFilter(img, kernel, 3);
                freeKernel(kernel, 3);
                printf("Box blur applied!\n");
                pauseScreen();
                break;

            case 2: // Gaussian Blur
                kernel = createKernel(3);
                float gaussian[3][3] = {{1/16.0f, 2/16.0f, 1/16.0f},
                                       {2/16.0f, 4/16.0f, 2/16.0f},
                                       {1/16.0f, 2/16.0f, 1/16.0f}};
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 3; j++) {
                        kernel[i][j] = gaussian[i][j];
                    }
                }
                bmp8_applyFilter(img, kernel, 3);
                freeKernel(kernel, 3);
                printf("Gaussian blur applied!\n");
                pauseScreen();
                break;

            case 3: // Sharpen
                kernel = createKernel(3);
                float sharpen[3][3] = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 3; j++) {
                        kernel[i][j] = sharpen[i][j];
                    }
                }
                bmp8_applyFilter(img, kernel, 3);
                freeKernel(kernel, 3);
                printf("Sharpening applied!\n");
                pauseScreen();
                break;

            case 4: // Emboss
                kernel = createKernel(3);
                float emboss[3][3] = {{-2, -1, 0}, {-1, 1, 1}, {0, 1, 2}};
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 3; j++) {
                        kernel[i][j] = emboss[i][j];
                    }
                }
                bmp8_applyFilter(img, kernel, 3);
                freeKernel(kernel, 3);
                printf("Emboss effect applied!\n");
                pauseScreen();
                break;

            case 5: // Outline
                kernel = createKernel(3);
                float outline[3][3] = {{-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1}};
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 3; j++) {
                        kernel[i][j] = outline[i][j];
                    }
                }
                bmp8_applyFilter(img, kernel, 3);
                freeKernel(kernel, 3);
                printf("Outline detection applied!\n");
                pauseScreen();
                break;

            case 0:
                break;

            default:
                printf("Invalid choice!\n");
                pauseScreen();
        }
    } while (choix != 0);
}

// Function to handle 24-bit BMP images
void handleBMP24() {
    int choix;
    char filename[256];
    int valeur;

    do {
        char path[256] = PATH;
        clearScreen();
        if (imageBMP24) {
            printf("24-bits Image Loaded: %dx%d pixels\n\n", imageBMP24->width, imageBMP24->height);
        } else {
            printf("No 24-bit image loaded\n\n");
        }

        displayBMP24Menu();
        scanf("%d", &choix);
        getchar(); // Consume the '\n'

        switch(choix) {
            case 1: // Load image
                printf("Filename to load: ");
                fgets(filename, sizeof(filename), stdin);
                filename[strcspn(filename, "\n")] = 0;
                strcat(path, filename);

                if (imageBMP24) {
                    bmp24_free(imageBMP24);
                }
                imageBMP24 = bmp24_loadImage(path);

                if (imageBMP24) {
                    printf("Image loaded successfully!\n");
                } else {
                    printf("Error loading the image.\n");
                }
                pauseScreen();
                break;

            case 2: // Save
                if (!imageBMP24) {
                    printf("No image loaded!\n");
                    pauseScreen();
                    break;
                }
                printf("Output filename: ");
                fgets(filename, sizeof(filename), stdin);
                filename[strcspn(filename, "\n")] = 0;
                strcat(path, filename);
                bmp24_saveImage(imageBMP24, path);
                pauseScreen();
                break;

            case 3: // Information
                if (!imageBMP24) {
                    printf("No image loaded!\n");
                } else {
                    bmp24_printInfo(imageBMP24);
                }
                pauseScreen();
                break;

            case 4: // Négatif
                if (!imageBMP24) {
                    printf("No image loaded!\n");
                } else {
                    bmp24_negative(imageBMP24);
                    printf("Negative applied successfully!\n");
                }
                pauseScreen();
                break;

            case 5: // Grayscale
                if (!imageBMP24) {
                    printf("No image loaded!\n");
                } else {
                    bmp24_grayscale(imageBMP24);
                    printf("Converted to grayscale successfully!\n");
                }
                pauseScreen();
                break;

            case 6: // Brightness
                if (!imageBMP24) {
                    printf("No image loaded!\n");
                    pauseScreen();
                    break;
                }
                printf("Brightness value (-255 to 255): ");
                scanf("%d", &valeur);
                bmp24_brightness(imageBMP24, valeur);
                printf("Brightness adjusted by %d units!\n", valeur);
                pauseScreen();
                break;
            case 7: // Horizontal flip
                if (!imageBMP24)
                {
                    printf("No image loaded!\n");
                    pauseScreen();
                    break;
                }
                bmp24_horizontalFlip(imageBMP24);
                printf("Horizontal flip applied successfully");
                pauseScreen();
                break;
        case 8: // Vertical flip
            if (!imageBMP24)
            {
                printf("No image loaded!\n");
                pauseScreen();
                break;
            }
            bmp24_verticalFlip(imageBMP24);
            printf("Vertical flip applied successfully");
            pauseScreen();
            break;


            case 9: // Filters
                if (!imageBMP24) {
                    printf("No image loaded!\n");
                    pauseScreen();
                } else {
                    applyFiltersBMP24(imageBMP24);
                }
                break;

            case 10: // Equalization
                if (!imageBMP24) {
                    printf("No image loaded!\n");
                } else {
                    bmp24_equalize(imageBMP24);
                    printf("Histogram equalization applied!\n");
                }
                pauseScreen();
                break;

            case 0:
                break;

            default:
                printf("Invalid choice!\n");
                pauseScreen();
        }
    } while (choix != 0);
}

// Function to apply filters to 24-bit BMP images
void applyFiltersBMP24(t_bmp24* img) {
    int choix;

    do {
        clearScreen();
        displayFilters24Menu();
        scanf("%d", &choix);

        switch(choix) {
            case 1: // Box Blur
                bmp24_boxBlur(img);
                printf("Box blur applied!\n");
                pauseScreen();
                break;

            case 2: // Gaussian Blur
                bmp24_gaussianBlur(img);
                printf("Gaussian blur applied!\n");
                pauseScreen();
                break;

            case 3: // Sharpen
                bmp24_sharpen(img);
                printf("Sharpening applied!\n");
                pauseScreen();
                break;

            case 4: // Emboss
                bmp24_emboss(img);
                printf("Emboss effect applied!\n");
                pauseScreen();
                break;

            case 5: // Outline
                bmp24_outline(img);
                printf("Outline detection applied!\n");
                pauseScreen();
                break;

            case 6: // Sepia tone
                bmp24_sepia(img);
                printf("Sepia tone effect applied!\n");
                pauseScreen();
                break;

            case 0:
                break;

            default:
                printf("Invalid choice!\n");
                pauseScreen();
        }
    } while (choix != 0);
}

// Main function
int main() {
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "");
    int choix;

    printf("Initializing BMP image manager...\n");

    do {
        clearScreen();
        displayMainMenu();
        scanf("%d", &choix);

        switch(choix) {
            case 1:
                handleBMP8();
                break;

            case 2:
                handleBMP24();
                break;

            case 0:
                printf("Closing the program...\n");
                break;

            default:
                printf("Invalid choice!\n");
                pauseScreen();
        }
    } while (choix != 0);

    // Memory deallocation
    if (imageBMP8) {
        bmp8_freeImage(imageBMP8);
    }
    if (imageBMP24) {
        bmp24_free(imageBMP24);
    }

    printf("Program completed successfully!\n");
    return 0;
}
