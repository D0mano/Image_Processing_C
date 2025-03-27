#include <stdio.h>
#include "bmp8.h"

int main() {
    // Charger une image
    t_bmp8 *image = bmp8_loadImage("lena_gray.bmp");
    if (!image) {
        return 1;
    }

    // Afficher les informations de l'image
    bmp8_printInfo(image);

    // Sauvegarder l'image sous un autre nom
    bmp8_saveImage("output.bmp", image);


    bmp8_freeImage(image);

    return 0;
}
