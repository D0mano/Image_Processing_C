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
  pixels = (t_pixel **)malloc(height * sizeof(t_pixel *));
  for (int i = 0; i < height; i++)
  {
    pixels[i] = (t_pixel *)malloc( width * sizeof(t_pixel));
  }
  if (!pixels) {
    printf("Error allocating memory for pixels\n");
    for (int j = 0; j < height; j++)
      free(pixels[j]);
    free(pixels);
    return NULL;
  };
  return pixels;
}

void bmp24_freeDataPixels (t_pixel ** pixels, int height){
  for (int i = 0; i < height; i++)
    free(pixels[i]);
  free(pixels);
}

t_bmp24 * bmp24_allocate (int width, int height, int colorDepth){
      t_bmp24 *img;
      img = (t_bmp24 *)malloc(sizeof(t_bmp24));
      img->width = width;
      img->height = height;
      img->colorDepth = colorDepth;
      img->data = bmp24_allocateDataPixels(width,height);
      if (!img->data) {
        printf("Error allocating memory for data\n");
        bmp24_freeDataPixels (img->data, img->height);
        return NULL;
      }
      return img;
}
void bmp24_free(t_bmp24 * img){
  bmp24_freeDataPixels(img->data,img->height);
  free(img);
}


void file_rawRead (uint32_t position, void * buffer, uint32_t size, size_t n, FILE * file) {
  fseek(file, position, SEEK_SET);
  fread(buffer, size, n, file);
}

void file_rawWrite (uint32_t position, void * buffer, uint32_t size, size_t n, FILE * file) {
  fseek(file, position, SEEK_SET);
  fwrite(buffer, size, n, file);
}

void bmp24_readPixelValue(t_bmp24 *image, const int x, const int y, FILE *file)
{
  int rowSize = ((image->width * 3 + 3) / 4) * 4;
  uint32_t position = image->header.offset + (image->height - 1 - y) * rowSize + x * 3;


  uint8_t pixelData[3];
  file_rawRead(position, pixelData, sizeof(uint8_t), 3, file);

  image->data[y][x].blue  = pixelData[0];
  image->data[y][x].green = pixelData[1];
  image->data[y][x].red   = pixelData[2];
  /*
  if (x == 0 && y == 0) {
    printf("Pixel[0][0] read: R=%u G=%u B=%u\n",
        image->data[y][x].red,
        image->data[y][x].green,
        image->data[y][x].blue);
  }
  */
}
void bmp24_readPixelData(t_bmp24 *image, FILE *file){
    for (int y = 0; y < image->height; y++) {
      for (int x = 0; x < image->width; x++) {
        bmp24_readPixelValue(image, x, y, file);
      }
    }
  }




void bmp24_writePixelValue(t_bmp24 *image, int x, int y, FILE *file) {
    int rowSize = ((image->width * 3 + 3) / 4) * 4;
    uint32_t position = image->header.offset + (image->height - 1 - y) * rowSize + x * 3;

    uint8_t pixelData[3];
    pixelData[0] = image->data[y][x].blue;
    pixelData[1] = image->data[y][x].green;
    pixelData[2] = image->data[y][x].red;

    file_rawWrite(position, pixelData, sizeof(uint8_t), 3, file);
  }


void bmp24_writePixelData(t_bmp24 *image, FILE *file) {
    for (int y = 0; y < image->height; y++) {
      for (int x = 0; x < image->width; x++) {
        bmp24_writePixelValue(image, x, y, file);
      }
    }
  }

t_bmp24 * bmp24_loadImage (const char * filename){
    FILE *file = fopen(filename, "rb");
    if (!file) {
      printf("Error : Opening of the file impossible %s\n", filename);
      return NULL;
    }

    t_bmp_header header;
    t_bmp_info info;

  file_rawRead(BITMAP_MAGIC, &header.type, sizeof(uint16_t), 1, file);
  file_rawRead(BITMAP_SIZE, &header.size, sizeof(uint32_t), 1, file);
  file_rawRead(BITMAP_OFFSET, &header.offset, sizeof(uint32_t), 1, file);

  file_rawRead(HEADER_SIZE, &info, sizeof(t_bmp_info), 1, file);

    if (header.type!= BMP_TYPE)
    {
      printf("Error : File is not a BMP file\n");
      fclose(file);
      return NULL;
    }
    if (info.bits != 24)
    {
      printf("Error : File is not 24 bit\n");
      fclose(file);
      return NULL;
    }

    t_bmp24 * img = bmp24_allocate(info.width, info.height, info.bits);

    if (!img) {
      printf("Error allocating memory for data\n");
      fclose(file);
      return NULL;
    }
    img->header = header;
    img->header_info = info;

    bmp24_readPixelData(img, file);

    fclose(file);
    bmp24_printInfo(img);
    //printf("Offset to pixel data: %u\n", header.offset);
    //printf("Width: %d, Height: %d, Bits: %d\n", info.width, info.height, info.bits);
    return img;
  }

void bmp24_printInfo(t_bmp24 *img){
    if (!img) {
      printf("Erreur : Image non valide\n");
      return;
    }
    printf("Image Info :\n");
    printf("Width : %u pixels\n", img->width);
    printf("Height : %u pixels\n", img->height);
    printf("Color Depth : %u bits\n", img->colorDepth);
    printf("Data Size : %u bytes\n", img->header.size);
  }


void bmp24_saveImage(t_bmp24 *img, const char *filename){
    // Vérification de la validité de l'image
    if (!img) {
      printf("Error: Invalid image pointer\n");
      return ;
    }

    // Ouverture du fichier en mode binaire pour écriture
    FILE *file = fopen(filename, "wb");
    if (!file) {
      printf("Error: Opening of the file impossible %s\n", filename);
      return ;
    }


    // Écriture de l'en-tête BMP
    file_rawWrite(BITMAP_MAGIC, &img->header.type, sizeof(uint16_t), 1, file);
    file_rawWrite(BITMAP_SIZE, &img->header.size, sizeof(uint32_t), 1, file);
    file_rawWrite(BITMAP_OFFSET, &img->header.offset, sizeof(uint32_t), 1, file);

    // Écriture des informations sur l'image
    file_rawWrite(HEADER_SIZE, &(img->header_info), sizeof(t_bmp_info), 1, file);

    // Écriture des données de pixels
    bmp24_writePixelData(img, file);

    // Fermeture du fichier
    fclose(file);
    printf("Image successfully saved\n"); // Succès
  }

void bmp24_negative(t_bmp24 *img) {
  for (int i = 0; i < img->height; i++) {
    for (int j = 0; j < img->width; j++) {
      img->data[i][j].red   = 255 - img->data[i][j].red;
      img->data[i][j].green = 255 - img->data[i][j].green;
      img->data[i][j].blue  = 255 - img->data[i][j].blue;
    }
  }
}

// Integration follow-up: Ok - test OK
void bmp24_grayscale (t_bmp24 * img) {
  for (int i = 0; i < img->height; i++) {
    for (int j = 0; j < img->width; j++) {

      uint8_t grayscale = (img->data[i][j].blue+img->data[i][j].green+img->data[i][j].red)/3;

      img->data[i][j].red   = grayscale;
      img->data[i][j].green = grayscale;
      img->data[i][j].blue  = grayscale;
    }
  }
}

// Integration follow-up: Ok - test OK
void bmp24_brightness (t_bmp24 * img,  int value) {
  for (int i = 0; i < img->height; i++) {
    for (int j = 0; j < img->width; j++) {
      if (img->data[i][j].red + value > 255) {
        img->data[i][j].red = 255;
      } else if (img->data[i][j].red + value < 0) {
        img->data[i][j].red = 0;
      } else {
        img->data[i][j].red += value;
      }

      if (img->data[i][j].green + value > 255) {
        img->data[i][j].green = 255;
      } else if (img->data[i][j].green + value < 0) {
        img->data[i][j].green = 0;
      } else {
        img->data[i][j].green += value;
      }

      if (img->data[i][j].blue + value > 255) {
        img->data[i][j].blue = 255;
      } else if (img->data[i][j].blue + value < 0) {
        img->data[i][j].blue = 0;
      } else {
        img->data[i][j].blue += value;
      }
    }
  }
}
float ** createKernel(int size)
{
  float ** kernel = malloc(size * sizeof(float*));
  for (int i = 0; i < size; i++)
  {
    kernel[i] = malloc(size*sizeof(float));
  }
  return kernel;
}

void freeKernel(float ** kernel,int size)
{
  for (int i = 0; i < size; i++)
    free(kernel[i]);
  free(kernel);
}



// Integration follow-up: Ok - test OK
t_pixel bmp24_convolution ( t_bmp24 * img,  int x,  int y, float ** kernel,  int kernelSize) {
  int kernelCenter = (kernelSize - 1) / 2;
  float red = 0.0f, green = 0.0f, blue = 0.0f;

  for (int i = 0; i < kernelSize; i++) {
    for (int j = 0; j < kernelSize; j++) {
      int xi = x + j - kernelCenter;
      int yi = y + i - kernelCenter;

      t_pixel p = img->data[yi][xi];

      red += p.red * kernel[i][j];
      green += p.green * kernel[i][j];
      blue += p.blue * kernel[i][j];
    }
  }

  t_pixel finalPixel;

  if (red > 255) {
    finalPixel.red = 255;
  } else if (red < 0) {
    finalPixel.red = 0;
  } else {
    finalPixel.red = red;
  }

  if (green > 255) {
    finalPixel.green = 255;
  } else if (green < 0) {
    finalPixel.green = 0;
  } else {
    finalPixel.green = green;
  }

  if (blue > 255) {
    finalPixel.blue = 255;
  } else if (blue < 0) {
    finalPixel.blue = 0;
  } else {
    finalPixel.blue = blue;
  }

  return finalPixel;
}


// Integration follow-up: Ok - test OK
void bmp24_applyFilter(t_bmp24 *img, float **kernel, const int kernelSize) {
  t_pixel** filterData = bmp24_allocateDataPixels(img->width, img->height);
  int kernelCenter = (kernelSize - 1) / 2;

  for (int y = kernelCenter; y < img->height - kernelCenter; y++) {
    for (int x = kernelCenter; x < img->width - kernelCenter; x++) {
      filterData[y][x] = bmp24_convolution(img, x, y, kernel, kernelSize);
    }
  }

  bmp24_freeDataPixels(img->data, img->height);
  img->data = filterData;
}



// Integration follow-up: Ok - test OK
void bmp24_boxBlur(t_bmp24 *img) {
  int kernelSize = 3;
  float **box_blur = createKernel(kernelSize);

  for (int i = 0; i < kernelSize; i++) {
    for (int j = 0; j < kernelSize; j++) {
      box_blur[i][j] = 1.0f / 9.0f;
    }
  }

  bmp24_applyFilter(img, box_blur, kernelSize);

  freeKernel(box_blur, kernelSize);
}

// Integration follow-up: Ok - test OK
void bmp24_gaussianBlur(t_bmp24 *img) {
  int kernelSize = 3;
  float **gaussian_blur = createKernel(kernelSize);

  float values_gaussian[3][3] = {
    {1/16.0f, 1/8.0f, 1/16.0f},
    {1/8.0f,  1/4.0f, 1/8.0f},
    {1/16.0f, 1/8.0f, 1/16.0f}
  };
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      gaussian_blur[i][j] = values_gaussian[i][j];

  bmp24_applyFilter(img, gaussian_blur, kernelSize);

  freeKernel(gaussian_blur, kernelSize);
}

// Integration follow-up: Ok - test OK
void bmp24_outline(t_bmp24 *img) {
  int kernelSize = 3;
  float **outline = createKernel(kernelSize);

  float values_outline[3][3] = {
    {-1, -1, -1},
    {-1,  8, -1},
    {-1, -1, -1}
  };

  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      outline[i][j] = values_outline[i][j];

  bmp24_applyFilter(img, outline, kernelSize);

  freeKernel(outline, kernelSize);
}

// Integration follow-up: Ok - test OK
void bmp24_emboss(t_bmp24 *img) {
  int kernelSize = 3;
  float **emboss = createKernel(kernelSize);

  float values_emboss[3][3] = {
    {-2, -1, 0},
    {-1, 1, 1},
    {0, 1, 2}
  };

  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      emboss[i][j] = values_emboss[i][j];

  bmp24_applyFilter(img, emboss, kernelSize);

  freeKernel(emboss, kernelSize);
}

// Integration follow-up: Ok -  test OK
void bmp24_sharpen(t_bmp24 *img) {
  int kernelSize = 3;
  float **sharpen = createKernel(kernelSize);

  float values_sharpen[3][3] = {
    {0,  -1, 0},
    {-1,  5, -1},
    {0,  -1, 0}
  };

  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      sharpen[i][j] = values_sharpen[i][j];

  bmp24_applyFilter(img, sharpen, kernelSize);

  freeKernel(sharpen, kernelSize);
}



