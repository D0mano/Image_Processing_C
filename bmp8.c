/**
 * @file bmp8.c
 * @authors Nolann FOTSO, Rafael ISLAM
 * @brief 8-bit grayscale BMP image processing library
 * 
 * This file contains all the functions necessary for loading, processing, and saving
 * 8-bit grayscale BMP images. It provides basic image operations like negative filter,
 * brightness adjustment, thresholding, convolution filters, and histogram equalization.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "bmp8.h"


t_bmp8 *bmp8_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Erreur : Impossible d'ouvrir le fichier %s\n", filename);
        return NULL;
    }

    // Allocate memory for the image structure
    t_bmp8 *img = (t_bmp8 *)malloc(sizeof(t_bmp8));
    if (!img) {
        printf("Erreur : Allocation mémoire échouée\n");
        fclose(file);
        return NULL;
    }

    // Read BMP header (54 bytes) and extract metadata
    fread(img->header, 1, 54, file);
    img->width = *(unsigned int *)&img->header[18];        // Width at offset 18
    img->height = *(unsigned int *)&img->header[22];       // Height at offset 22
    img->colorDepth = *(unsigned int *)&img->header[28];   // Bits per pixel at offset 28
    img->dataSize = img ->height * img->width;             // Total pixels

    // Validate that this is an 8-bit grayscale image
    if (img->colorDepth != 8) {
        printf("Erreur : L'image n'est pas en niveaux de gris 8 bits\n");
        free(img);
        fclose(file);
        return NULL;
    }

    // Read color table (256 colors * 4 bytes = 1024 bytes)
    // Each entry contains Blue, Green, Red, Reserved bytes
    fread(img->colorTable, 1, 1024, file);

    // Allocate memory for pixel data and read it
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
        printf("Erreur : Impossible d'ecrire dans le fichier %s\n", filename);
        return;
    }

    // Write BMP header, color table, and pixel data in sequence
    fwrite(img->header, 1, 54, file);      // BMP header
    fwrite(img->colorTable, 1, 1024, file); // Color palette
    fwrite(img->data, 1, img->dataSize, file); // Pixel data

    fclose(file);
    printf("Image enregistree sous : %s\n", filename);
}


void bmp8_freeImage(t_bmp8 *img) {
    if (img) {
        free(img->data);  // Free pixel data array
        free(img);        // Free main structure
    }
}


void bmp8_printInfo(t_bmp8 *img) {
    if (!img) {
        printf("Erreur : Image non valide\n");
        return;
    }
    printf("Image Info :\n");
    printf("Width : %d pixels\n", img->width);
    printf("Height : %d pixels\n", img->height);
    printf("Color Depth : %d bits\n", img->colorDepth);
    printf("Data Size : %d bytes\n", img->dataSize);
}


void bmp8_negative(t_bmp8 * img)
{
    for (int i = 0; i < img->width * img->height; i++)
    {
        img->data[i] = 255 - img->data[i];  // Invert pixel value
    }
}


void bmp8_brightness(t_bmp8 * img, int value)
{
    for (int i = 0; i < img->width * img->height; i++)
    {
        // Clamp values to valid range [0, 255]
        if (img->data[i]+ value > 255)
        {
            img->data[i] = 255;  // Cap at maximum brightness
        }
        else if (img->data[i]+value < 0)
        {
            img->data[i] = 0;    // Cap at minimum brightness
        }
        else
        {
            img->data[i] = img->data[i] + value;  // Apply brightness change
        }
    }
}


void bmp8_threshold(t_bmp8 * img, int threshold)
{
    for (int i = 0; i < img->width * img->height; i++)
    {
        if (img->data[i] > threshold)
        {
            img->data[i] = 255;  // Set to white
        }else
        {
            img->data[i] = 0;    // Set to black
        }
    }
}


int** list_to_matrix(t_bmp8 *img)
{
    // BUG: Should allocate img->height rows, not img->width
    int **matrix = (int**)malloc(img->width * sizeof(int*));
    for (int i = 0; i < img->width * img->height; i++)  // This loop condition is incorrect
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
            list[i * n + j] = matrix[i][j];  // Flatten 2D to 1D
        }
    }
    return list;
}


void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize) {
    int width = img->width;
    int height = img->height;
    int n = kernelSize / 2;  // Half kernel size for centering

    // Create temporary buffer to avoid modifying source during processing
    unsigned char *temp = malloc(img->dataSize * sizeof(unsigned char));
    if (!temp) {
        printf("Error allocating memory for the filter.\n");
        return;
    }

    // Copy original image data to temporary buffer
    for (unsigned int i = 0; i < img->dataSize; i++) {
        temp[i] = img->data[i];
    }

    // Apply convolution to all pixels except border pixels
    for (int y = n; y < height - n; y++) {
        for (int x = n; x < width - n; x++) {
            float sum = 0.0f;

            // Perform convolution operation
            for (int ky = -n; ky <= n; ky++) {
                for (int kx = -n; kx <= n; kx++) {
                    int imgX = x + kx;  // Source pixel X coordinate
                    int imgY = y + ky;  // Source pixel Y coordinate
                    unsigned char pixel = temp[imgY * width + imgX];
                    // Multiply pixel value by corresponding kernel value
                    sum += pixel * kernel[ky + n][kx + n];
                }
            }

            // Clamp result to valid pixel range [0, 255]
            if (sum < 0) sum = 0;
            if (sum > 255) sum = 255;

            // Set the new pixel value
            img->data[y * width + x] = (unsigned char)(sum);
        }
    }

    free(temp); // Free temporary buffer
}


unsigned int * bmp8_computeHistogram(t_bmp8 * img)
{
    // Initialize histogram array with zeros (256 possible intensity values)
    unsigned int * hist = (unsigned int *)calloc(256,sizeof(unsigned int));
    
    // Count frequency of each pixel intensity
    for (int i = 0; i < img->width * img->height; i++)
        hist[img->data[i]] ++;

    return hist;
}


unsigned int  min_arr(int* arr,int n,int N)
{
    unsigned int min = N;
    for (int i = 1; i < n; i++)  // Start from index 1, skip first element
    {
        if (arr[i] < min && arr[i] != 0)
            min = arr[i];
    }
    return min;
}


unsigned int * bmp8_computeCDF(unsigned int * hist)
{
    unsigned int * cdf = malloc(256*sizeof(unsigned int));
    unsigned int sum = 0;
    int N;
    
    // Compute cumulative distribution function
    for (int i = 0; i < 256; i++)
    {
        sum += hist[i];
        cdf[i] = sum;  // CDF[i] = sum of hist[0] to hist[i]
    }
    
    N = cdf[255];  // Total number of pixels
    unsigned int cdf_min = min_arr(cdf,256,N);  // Minimum non-zero CDF value
    
    // Create equalization mapping using histogram equalization formula
    unsigned int * hist_eq = malloc(256 * sizeof(unsigned int));
    for (int i = 0; i < 256; i++)
    {
        // Apply equalization: scale CDF to [0, 255] range
        hist_eq[i] = round((float)(cdf[i] - cdf_min) / (N - cdf_min) * 255);
    }
    
    free(hist);  // Free original histogram
    return hist_eq;  // Return equalization mapping
}


void bmp8_equalize(t_bmp8 * img)
{
    // Compute histogram and equalization mapping
    unsigned int * hist = bmp8_computeHistogram(img);
    unsigned int * hist_eq = bmp8_computeCDF(hist);
    
    // Apply equalization mapping to all pixels
    for (int i = 0; i < img->dataSize; i++)
        img->data[i] = hist_eq[img->data[i]];  // Map old intensity to new intensity
        
    free(hist);
    free(hist_eq);
}