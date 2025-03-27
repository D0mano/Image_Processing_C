#include <stdio.h>
#include <stdlib.h>
#include "bmp8.h"

t_bmp8 *bmp8_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Erreur : Impossible d'ouvrir le fichier %s\n", filename);
        return NULL;
    }

    // Allocation de la structure
    t_bmp8 *img = (t_bmp8 *)malloc(sizeof(t_bmp8));
    if (!img) {
        printf("Erreur : Allocation mémoire échouée\n");
        fclose(file);
        return NULL;
    }

    // Lecture de l'en-tête et des métadonnées
    fread(img->header, 1, 54, file);
    img->width = *(unsigned int *)&img->header[18];
    img->height = *(unsigned int *)&img->header[22];
    img->colorDepth = *(unsigned int *)&img->header[28];
    img->dataSize = *(unsigned int *)&img->header[34];

    if (img->colorDepth != 8) {
        printf("Erreur : L'image n'est pas en niveaux de gris 8 bits\n");
        free(img);
        fclose(file);
        return NULL;
    }

    // Lecture de la table des couleurs (1024 octets)
    fread(img->colorTable, 1, 1024, file);

    // Allocation mémoire et lecture des données de l'image
    img->data = (unsigned char *)malloc(img->dataSize);
    if (!img->data) {
        printf("Erreur : Allocation mémoire échouée pour les données de l'image\n");
        free(img);
        fclose(file);
        return NULL;
    }
    fread(img->data, 1, img->dataSize, file);

    fclose(file);
    return img;
}

void bmp8_saveImage(const char *filename, t_bmp8 *img) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Erreur : Impossible d'écrire dans le fichier %s\n", filename);
        return;
    }

    // Écriture de l'en-tête et des métadonnées
    fwrite(img->header, 1, 54, file);
    fwrite(img->colorTable, 1, 1024, file);
    fwrite(img->data, 1, img->dataSize, file);

    fclose(file);
    printf("Image enregistrée sous : %s\n", filename);
}

void bmp8_freeImage(t_bmp8 *img) {
    if (img) {
        free(img->data);
        free(img);
    }
}

void bmp8_printInfo(t_bmp8 *img) {
    if (!img) {
        printf("Erreur : Image non valide\n");
        return;
    }
    printf("Image Info :\n");
    printf("Width : %u pixels\n", img->width);
    printf("Height : %u pixels\n", img->height);
    printf("Color Depth : %u bits\n", img->colorDepth);
    printf("Data Size : %u bytes\n", img->dataSize);
}
