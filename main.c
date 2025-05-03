#include <stdio.h>
#include <stdlib.h>

#include "bmp8.h"
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

    // Charger une image
    t_bmp8 *image = bmp8_loadImage("lena_gray.bmp");
    if (!image) {
        return 1;
    }
    // Afficher les informations de l'image
    bmp8_printInfo(image);
    bmp8_applyFilter(image, createEmbossKernel(), 3);
    // Sauvegarder l'image sous un autre nom
    bmp8_saveImage("output.bmp", image);


    bmp8_freeImage(image);

    return 0;
}
