//
// Created by njfot on 27/04/2025.
//

#include "bmp24.h"
// Offsets for the BMP header
#define BITMAP_MAGIC    0x00 // offset 0
#define BITMAP_SIZE    0x02 // offset 2
#define BITMAP_OFFSET    0x0A // offset 10
#define BITMAP_WIDTH    0x12 // offset 18
#define BITMAP_HEIGHT    0x16 // offset 22
#define BITMAP_DEPTH    0x1C // offset 28
#define BITMAP_SIZE_RAW    0x22 // offset 34
// Magical number for BMP files
#define BMP_TYPE    0x4D42 // 'BM' in hexadecimal
// Header sizes
#define HEADER_SIZE    0x0E // 14 octets
#define INFO_SIZE    0x28 // 40 octets
// Constant for the color depth
#define DEFAULT_DEPTH    0x18 // 24

t_pixel ** bmp24_allocateDataPixels (int width, int height){
  t_pixel **pixels;
  pixels = (t_pixel **)malloc(width * height * sizeof(t_pixel *));
  if (!pixels) {
    printf("Error allocating memory for pixels\n");
    return NULL;
  }
   return pixels;
}

void bmp24_freeDataPixels (t_pixel ** pixels, int height){
  free(pixels);
}

t_bmp24 * bmp24_allocate (int width, int height, int colorDepth){
  t_bmp24 *img;
  img = (t_bmp24 *)malloc(sizeof(t_bmp24));
  img->width = width;
  img->height = height;
  img->colorDepth = colorDepth;
  img->data = bmp24_allocateDataPixels (width, height);
  if (!img->data) {
    printf("Error allocating memory for data\n");
    bmp24_freeDataPixels (img->data, img->height);
    return NULL;
  }
  return img;
}
void bmp24_free(t_bmp24 * img){
  free(img->data);
  free(img);
}

t_bmp24 * bmp24_loadImage (const char * filename){
  FILE *file = fopen(filename, "rb");
  if (!file) {
    printf("Error : Opening of the file impossible %s\n", filename);
    return NULL;
    }
    t_bmp_header header;
    file_rawRead(BITMAP_MAGIC, &header, sizeof(t_bmp_header), 1, file);
    t_bmp_info info;
    file_rawRead(HEADER_SIZE, &info, sizeof(t_bmp_info), 1, file);
}
