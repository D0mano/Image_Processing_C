//
// Created by njfot on 20/03/2025.
//

#ifndef BMP8_H
#define BMP8_H

typedef struct {
    unsigned char header[54];
    unsigned char colorTable[1024];
    unsigned char * data;
    unsigned int width;
    unsigned int height;
    unsigned int colorDepth;
    unsigned int dataSize;
} t_bmp8;

t_bmp8 * bmp8_loadImage(const char * filename);
void bmp8_saveImage(const char * filename, t_bmp8 * img);

void bmp8_freeImage(t_bmp8 * img);
void bmp8_printInfo(t_bmp8 * img);

void bmp8_negative(t_bmp8 * img);
void bmp8_brightness(t_bmp8 * img, int value);
void bmp8_threshold(t_bmp8 * img, int threshold);
int ** list_to_matrix(t_bmp8 *img); // bonus
unsigned char * matrix_to_list(int ** matrix,int n);
void bmp8_applyFilter(t_bmp8 * img, float ** kernel, int kernelSize);
unsigned int * bmp8_computeHistogram(t_bmp8 * img);
unsigned int  min_arr24(int* arr,int n,int N);
unsigned int * bmp8_computeCDF(unsigned int * hist);
void bmp8_equalize(t_bmp8 * img);





#endif //BMP8_H
