 // Offsets for the BMP header
 #define BITMAP_MAGIC 0x00 // offset 0
 #define BITMAP_SIZE  0x02 // offset 2
 #define BITMAP_OFFSET 0x0A // offset 10
 #define BITMAP_WIDTH  0x12 // offset 18
 #define BITMAP_HEIGHT  0x16 // offset 22
 #define BITMAP_DEPTH  0x1C // offset 28
 #define BITMAP_SIZE_RAW  0x22 // offset 34
 // Magical number for BMP files
 #define BMP_TYPE 0x4D42 // 'BM' in hexadecimal
 // Header sizes
 #define HEADER_SIZE 0x0E // 14 octets
 #define INFO_SIZE 0x28 // 40 octets
 // Constant for the color depth
 #define DEFAULT_DEPTH 0x18 // 24
#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>

#ifndef BMP24_H
#define BMP24_H


typedef struct {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} t_bmp_header;
typedef struct {
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bits;
    uint32_t compression;
    uint32_t imagesize;
    int32_t xresolution;
    int32_t yresolution;
    uint32_t ncolors;
    uint32_t importantcolors;
} t_bmp_info;

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} t_pixel;

typedef struct
{
    float Y;
    float U;
    float V;
}t_pixel_YUV;

typedef struct {
    t_bmp_header header;
    t_bmp_info header_info;
    int width;
    int height;
    int colorDepth;
    t_pixel **data;
} t_bmp24;

t_pixel ** bmp24_allocateDataPixels (int width, int height);
void bmp24_freeDataPixels (t_pixel ** pixels, int height);
t_bmp24 * bmp24_allocate (int width, int height, int colorDepth);
void bmp24_free (t_bmp24 * img);
t_bmp24 * bmp24_loadImage (const char * filename);
void bmp24_saveImage (t_bmp24 * img, const char * filename);

 /*
 * @brief Set the file cursor to the position  in the file ,
 * then read n elements of size  from the file into the buffer.
 * @param position The position from which to read in file.
 * @param buffer The buffer to read the elements into.
 * @param size The size of each element to read.
 * @param n The number of elements to read.
 * @param file The file descriptor to read from.
 * @return void
 */
 void file_rawRead (uint32_t position, void * buffer, uint32_t size, size_t n, FILE * file);
 /*
 * @brief Set the file cursor to the position  in the file ,
 * then write n elements of size  from the buffer into the file.
 * @param position The position from which to write in file.
 * @param buffer The buffer to write the elements from.
 * @param size The size of each element to write.
 * @param n The number of elements to write.
 * @param file The file descriptor to write to.
 * @return void
 */
 void file_rawWrite (uint32_t position, void * buffer, uint32_t size, size_t n, FILE * file);


void bmp24_readPixelValue (t_bmp24 * image, int x, int y, FILE * file);
void bmp24_readPixelData (t_bmp24 * image, FILE * file);
void bmp24_writePixelValue (t_bmp24 * image, int x, int y, FILE * file);
void bmp24_writePixelData (t_bmp24 * image, FILE * file);
void bmp24_negative (t_bmp24 * img);
void bmp24_grayscale (t_bmp24 * img);
void bmp24_brightness (t_bmp24 * img, int value);
float ** createKernel(int size);
void freeKernel(float ** kernel,int size);
t_pixel bmp24_convolution (t_bmp24 * img, int x, int y, float ** kernel, int kernelSize);
void bmp24_applyFilter(t_bmp24 *img, float **kernel, const int kernelSize);
void bmp24_boxBlur(t_bmp24 *img);
void bmp24_gaussianBlur(t_bmp24 *img);
void bmp24_outline(t_bmp24 *img);
void bmp24_emboss(t_bmp24 *img);
void bmp24_sharpen(t_bmp24 *img);
void bmp24_printInfo(t_bmp24 *img);

t_pixel_YUV ** RGB_to_YUV(t_bmp24 * img);
unsigned int  min_arr(int* arr,int n,int N);
unsigned int * bmp24_Computehistogram (t_bmp24 * img);
unsigned int * bmp24_ComputeCDF (unsigned int * hist);
void bmp24_equalize(t_bmp24 *img);


#endif //BMP24_H
