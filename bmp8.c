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
void bmp8_negative(t_bmp8 * img)
{
    for (int i = 0; i < img->width * img->height; i++)
    {
        img->data[i] = 255 - img->data[i];
    }
}

void bmp8_brightness(t_bmp8 * img, int value)
{
    for (int i = 0; i < img->width * img->height; i++)
    {
        if (img->data[i]+ value > 255)
        {
            img->data[i] = 255;
        }
        else if (img->data[i]+value < 0)
        {
            img->data[i] = 0;
        }
        else
        {
            img->data[i] = img->data[i] + value;
        }
    }
}
void bmp8_threshold(t_bmp8 * img, int threshold)
{
    for (int i = 0; i < img->width * img->height; i++)
    {
        if (img->data[i] > threshold)
        {
            img->data[i] = 255;
        }else
        {
            img->data[i] = 0;
        }
    }
}

int** list_to_matrix(t_bmp8 *img)
{
    int **matrix = (int**)malloc(img->width * sizeof(int*));
    for (int i = 0; i < img->width * img->height; i++)
    {
        matrix[i] = (int*)malloc(img->width * sizeof(int));
        for (int j = 0; j < img->width; j++)
        {
            matrix[i][j] = img->data[i*img->width + j];
        }
    }
    return matrix;
}

unsigned char * matrix_to_list(int** matrix,int n)
{
   unsigned char * list =(unsigned char*)malloc(n*n * sizeof(int));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            list[i * n + j] = matrix[i][j];
        }
    }
    return list;
}

//void bmp8_applyFilter(t_bmp8 * img, float ** kernel, int kernelSize)
//{
  //  int width = img->width;
    //int height = img->height;
    //int n = kernelSize/2;
    //for (int x = 1; x < width-2; x++)
//        for (int y = 1; y < height-2; y++)
//        {
//            int tmp = 0;
//            for (int i = -n; i <= n; i++)
//                for (int j = -n; j <= n; j++)
//                    tmp += img->data[(x-i)*width + (y-j)]*kernel[i][j];
//            img->data[x*n+y] = tmp;
//        }
//}
void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize) {
    int width = img->width;
    int height = img->height;
    int n = kernelSize / 2; // Décalage pour parcourir autour du pixel central

    // Créer un tableau temporaire pour stocker le résultat
    unsigned char *temp = malloc(img->dataSize * sizeof(unsigned char));
    if (!temp) {
        printf("Erreur d'allocation mémoire pour le filtre.\n");
        return;
    }

    // Copier l'image d'origine dans le tableau temporaire (par sécurité)
    for (unsigned int i = 0; i < img->dataSize; i++) {
        temp[i] = img->data[i];
    }

    // Appliquer le filtre sur tous les pixels sauf les bords
    for (int y = n; y < height - n; y++) {
        for (int x = n; x < width - n; x++) {
            float sum = 0.0f;

            // Appliquer la convolution
            for (int ky = -n; ky <= n; ky++) {
                for (int kx = -n; kx <= n; kx++) {
                    int imgX = x + kx;
                    int imgY = y + ky;
                    unsigned char pixel = temp[imgY * width + imgX];
                    sum += pixel * kernel[ky + n][kx + n];
                }
            }

            // Clamper la valeur entre 0 et 255
            if (sum < 0) sum = 0;
            if (sum > 255) sum = 255;

            // Affecter la nouvelle valeur au pixel
            img->data[y * width + x] = (unsigned char)(sum);
        }
    }

    free(temp); // Libérer le tableau temporaire
}