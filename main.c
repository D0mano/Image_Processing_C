#include <stdio.h>
#include <stdlib.h>
#include "bmp8.h"
#include "bmp24.h"

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



int main() {
    printf("Starting program...\n");

    t_bmp24* image2 = bmp24_loadImage("..\\flowers_color.bmp");
    if (!image2) {
        printf("Failed to load image\n");
        return 1;
    }
    bmp24_printInfo(image2);
    bmp24_sharpen(image2);

    bmp24_saveImage(image2, "..\\output3.bmp");


    // Libérer la mémoire
    bmp24_free(image2);

    printf("Program completed successfully\n");
    return 0;
}
