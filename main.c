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
            kernel[i][j] = 1.0f / 9.0f; // Tous les coefficients valent 1/9
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
    bmp8_negative(image);
    // Sauvegarder l'image sous un autre nom
    bmp8_saveImage("output.bmp", image);


    bmp8_freeImage(image);

    return 0;
}
