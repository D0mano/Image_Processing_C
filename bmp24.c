/**
 * @file bmp24.c
 * @authors Nolann FOTSO, Rafael ISLAM
 * @brief 24-bit colorscale BMP image processing library
 *
 * This file contains all the functions necessary for loading, processing, and saving
 * 24-bit colorscale BMP images. It provides basic image operations like negative filter,
 * brightness adjustment, gray scaling, convolution filters, and histogram equalization.
 *
 */

#include "bmp24.h"
#include <math.h>
#include "bmp8.h"

// ========================================
// BMP FILE FORMAT CONSTANTS
// ========================================
// These define the byte offsets for different fields in the BMP file header
#define BITMAP_MAGIC    0x00 // offset 0  - Magic number location ('BM')
#define BITMAP_SIZE     0x02 // offset 2  - Total file size in bytes
#define BITMAP_OFFSET   0x0A // offset 10 - Offset to start of pixel data
#define BITMAP_WIDTH    0x12 // offset 18 - Image width in pixels
#define BITMAP_HEIGHT   0x16 // offset 22 - Image height in pixels
#define BITMAP_DEPTH    0x1C // offset 28 - Bits per pixel (color depth)
#define BITMAP_SIZE_RAW 0x22 // offset 34 - Size of raw pixel data

// BMP file identification and format constants
#define BMP_TYPE        0x4D42 // 'BM' in hexadecimal - identifies BMP files
#define HEADER_SIZE     0x0E   // 14 bytes - size of main BMP header
#define INFO_SIZE       0x28   // 40 bytes - size of info header
#define DEFAULT_DEPTH   0x18   // 24 bits - default color depth for true color

// ========================================
// MEMORY MANAGEMENT FUNCTIONS
// ========================================


t_pixel ** bmp24_allocateDataPixels (int width, int height){
    t_pixel **pixels;

    // First, allocate array of row pointers (one pointer per row)
    pixels = (t_pixel **)malloc(height * sizeof(t_pixel *));

    // Then allocate each individual row (width pixels per row)
    for (int i = 0; i < height; i++)
    {
        pixels[i] = (t_pixel *)malloc( width * sizeof(t_pixel));
    }

    // Check if allocation failed and clean up if necessary
    if (!pixels) {
        printf("Error allocating memory for pixels\n");
        // Free any already allocated rows
        for (int j = 0; j < height; j++)
            free(pixels[j]);
        free(pixels);
        return NULL;
    };
    return pixels;
}


void bmp24_freeDataPixels (t_pixel ** pixels, int height){
    // Free each row
    for (int i = 0; i < height; i++)
        free(pixels[i]);
    // Free the array of row pointers
    free(pixels);
}


t_bmp24 * bmp24_allocate (int width, int height, int colorDepth){
    t_bmp24 *img;

    // Allocate memory for the main image structure
    img = (t_bmp24 *)malloc(sizeof(t_bmp24));

    // Initialize basic properties
    img->width = width;
    img->height = height;
    img->colorDepth = colorDepth;

    // Allocate the 2D pixel data array
    img->data = bmp24_allocateDataPixels(width, height);

    // Check if pixel data allocation failed
    if (!img->data) {
        printf("Error allocating memory for data\n");
        bmp24_freeDataPixels (img->data, img->height);
        return NULL;
    }
    return img;
}


void bmp24_free(t_bmp24 * img){
    bmp24_freeDataPixels(img->data, img->height);
    free(img);
}

// ========================================
// FILE I/O HELPER FUNCTIONS
// ========================================


void file_rawRead (uint32_t position, void * buffer, uint32_t size, size_t n, FILE * file) {
    fseek(file, position, SEEK_SET);  // Move to specified position
    fread(buffer, size, n, file);     // Read data into buffer
}


void file_rawWrite (uint32_t position, void * buffer, uint32_t size, size_t n, FILE * file) {
    fseek(file, position, SEEK_SET);   // Move to specified position
    fwrite(buffer, size, n, file);     // Write data from buffer
}

// ========================================
// PIXEL DATA I/O FUNCTIONS
// ========================================


void bmp24_readPixelValue(t_bmp24 *image, const int x, const int y, FILE *file)
{
    // Calculate row size including padding (BMP rows are padded to 4-byte boundaries)
    int rowSize = ((image->width * 3 + 3) / 4) * 4;

    // Calculate file position: start of pixel data + row offset + column offset
    // Note: (height - 1 - y) flips Y coordinate since BMP stores bottom-to-top
    uint32_t position = image->header.offset + (image->height - 1 - y) * rowSize + x * 3;

    // Read 3 bytes (BGR) from file
    uint8_t pixelData[3];
    file_rawRead(position, pixelData, sizeof(uint8_t), 3, file);

    // Store in RGB order (convert from file's BGR order)
    image->data[y][x].blue  = pixelData[0];  // Blue stays first
    image->data[y][x].green = pixelData[1];  // Green stays middle
    image->data[y][x].red   = pixelData[2];  // Red was last in file

    /* Debug code - uncomment to see first pixel value
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
    // Calculate row size with padding
    int rowSize = ((image->width * 3 + 3) / 4) * 4;

    // Calculate file position (flip Y coordinate for BMP format)
    uint32_t position = image->header.offset + (image->height - 1 - y) * rowSize + x * 3;

    // Convert RGB to BGR format for BMP file
    uint8_t pixelData[3];
    pixelData[0] = image->data[y][x].blue;   // Blue first in file
    pixelData[1] = image->data[y][x].green;  // Green second
    pixelData[2] = image->data[y][x].red;    // Red last in file

    // Write 3 bytes to file
    file_rawWrite(position, pixelData, sizeof(uint8_t), 3, file);
}


void bmp24_writePixelData(t_bmp24 *image, FILE *file) {
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            bmp24_writePixelValue(image, x, y, file);
        }
    }
}

// ========================================
// BMP FILE LOADING AND SAVING
// ========================================


t_bmp24 * bmp24_loadImage (const char * filename){
    // Open file in binary read mode
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Error : Opening of the file impossible %s\n", filename);
        return NULL;
    }

    // Declare structures to hold header information
    t_bmp_header header;
    t_bmp_info info;

    // Read main BMP header fields
    file_rawRead(BITMAP_MAGIC, &header.type, sizeof(uint16_t), 1, file);     // Magic number
    file_rawRead(BITMAP_SIZE, &header.size, sizeof(uint32_t), 1, file);      // File size
    file_rawRead(BITMAP_OFFSET, &header.offset, sizeof(uint32_t), 1, file);  // Pixel data offset

    // Read info header (contains image dimensions and format info)
    file_rawRead(HEADER_SIZE, &info, sizeof(t_bmp_info), 1, file);

    // Validate that this is actually a BMP file
    if (header.type != BMP_TYPE)
    {
        printf("Error : File is not a BMP file\n");
        fclose(file);
        return NULL;
    }

    // Validate that this is 24-bit color depth
    if (info.bits != 24)
    {
        printf("Error : File is not 24 bit\n");
        fclose(file);
        return NULL;
    }

    // Allocate image structure with dimensions from file
    t_bmp24 * img = bmp24_allocate(info.width, info.height, info.bits);

    if (!img) {
        printf("Error allocating memory for data\n");
        fclose(file);
        return NULL;
    }

    // Store header information in image structure
    img->header = header;
    img->header_info = info;

    // Read all pixel data from file
    bmp24_readPixelData(img, file);

    fclose(file);
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
    // Validate image pointer
    if (!img) {
        printf("Error: Invalid image pointer\n");
        return ;
    }

    // Open file in binary write mode
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Error: Opening of the file impossible %s\n", filename);
        return ;
    }

    // Write BMP header
    file_rawWrite(BITMAP_MAGIC, &img->header.type, sizeof(uint16_t), 1, file);
    file_rawWrite(BITMAP_SIZE, &img->header.size, sizeof(uint32_t), 1, file);
    file_rawWrite(BITMAP_OFFSET, &img->header.offset, sizeof(uint32_t), 1, file);

    // Write image info header
    file_rawWrite(HEADER_SIZE, &(img->header_info), sizeof(t_bmp_info), 1, file);

    // Write all pixel data
    bmp24_writePixelData(img, file);

    fclose(file);
    printf("Image successfully saved\n");
}

// ========================================
// BASIC IMAGE PROCESSING FUNCTIONS
// ========================================


void bmp24_negative(t_bmp24 *img) {
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            // Invert each color channel by subtracting from maximum value (255)
            img->data[i][j].red   = 255 - img->data[i][j].red;
            img->data[i][j].green = 255 - img->data[i][j].green;
            img->data[i][j].blue  = 255 - img->data[i][j].blue;
        }
    }
}


void bmp24_grayscale (t_bmp24 * img) {
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            // Calculate average of RGB values
            uint8_t grayscale = (img->data[i][j].blue + img->data[i][j].green + img->data[i][j].red) / 3;

            // Set all color channels to the same gray value
            img->data[i][j].red   = grayscale;
            img->data[i][j].green = grayscale;
            img->data[i][j].blue  = grayscale;
        }
    }
}


void bmp24_brightness (t_bmp24 * img,  int value) {
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            // Adjust red channel with clamping to valid range [0, 255]
            if (img->data[i][j].red + value > 255) {
                img->data[i][j].red = 255;
            } else if (img->data[i][j].red + value < 0) {
                img->data[i][j].red = 0;
            } else {
                img->data[i][j].red += value;
            }

            // Adjust green channel with clamping
            if (img->data[i][j].green + value > 255) {
                img->data[i][j].green = 255;
            } else if (img->data[i][j].green + value < 0) {
                img->data[i][j].green = 0;
            } else {
                img->data[i][j].green += value;
            }

            // Adjust blue channel with clamping
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

// ========================================
// CONVOLUTION AND FILTERING FUNCTIONS
// ========================================


float ** createKernel(int size)
{
    // Allocate array of row pointers
    float ** kernel = malloc(size * sizeof(float*));

    // Allocate each row
    for (int i = 0; i < size; i++)
    {
        kernel[i] = malloc(size * sizeof(float));
    }
    return kernel;
}


void freeKernel(float ** kernel, int size)
{
    for (int i = 0; i < size; i++)
        free(kernel[i]);
    free(kernel);
}


t_pixel bmp24_convolution ( t_bmp24 * img,  int x,  int y, float ** kernel,  int kernelSize) {
    int kernelCenter = (kernelSize - 1) / 2;  // Center position of kernel
    float red = 0.0f, green = 0.0f, blue = 0.0f;

    // Iterate through all positions in kernel
    for (int i = 0; i < kernelSize; i++) {
        for (int j = 0; j < kernelSize; j++) {
            // Calculate corresponding image coordinates
            int xi = x + j - kernelCenter;
            int yi = y + i - kernelCenter;

            // Get pixel at calculated position
            t_pixel p = img->data[yi][xi];

            // Multiply pixel values by kernel weights and accumulate
            red   += p.red   * kernel[i][j];
            green += p.green * kernel[i][j];
            blue  += p.blue  * kernel[i][j];
        }
    }

    // Create result pixel with clamping to valid range [0, 255]
    t_pixel finalPixel;

    finalPixel.red = (red > 255) ? 255 : ((red < 0) ? 0 : (uint8_t)red);
    finalPixel.green = (green > 255) ? 255 : ((green < 0) ? 0 : (uint8_t)green);
    finalPixel.blue = (blue > 255) ? 255 : ((blue < 0) ? 0 : (uint8_t)blue);

    return finalPixel;
}


void bmp24_applyFilter(t_bmp24 *img, float **kernel, const int kernelSize) {
    // Allocate temporary storage for filtered image
    t_pixel** filterData = bmp24_allocateDataPixels(img->width, img->height);
    int kernelCenter = (kernelSize - 1) / 2;

    // Process only pixels where full kernel fits (skip edges)
    for (int y = kernelCenter; y < img->height - kernelCenter; y++) {
        for (int x = kernelCenter; x < img->width - kernelCenter; x++) {
            // Apply convolution at this pixel
            filterData[y][x] = bmp24_convolution(img, x, y, kernel, kernelSize);
        }
    }

    // Replace original data with filtered data
    bmp24_freeDataPixels(img->data, img->height);
    img->data = filterData;
}

// ========================================
// SPECIFIC FILTER IMPLEMENTATIONS
// ========================================


void bmp24_boxBlur(t_bmp24 *img) {
    int kernelSize = 3;
    float **box_blur = createKernel(kernelSize);

    // Fill kernel with equal weights (1/9 for each position)
    for (int i = 0; i < kernelSize; i++) {
        for (int j = 0; j < kernelSize; j++) {
            box_blur[i][j] = 1.0f / 9.0f;
        }
    }

    bmp24_applyFilter(img, box_blur, kernelSize);
    freeKernel(box_blur, kernelSize);
}


void bmp24_gaussianBlur(t_bmp24 *img) {
    int kernelSize = 3;
    float **gaussian_blur = createKernel(kernelSize);

    // Gaussian kernel weights (center weighted)
    float values_gaussian[3][3] = {
        {1/16.0f, 1/8.0f, 1/16.0f},
        {1/8.0f,  1/4.0f, 1/8.0f},
        {1/16.0f, 1/8.0f, 1/16.0f}
    };

    // Copy values to kernel
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            gaussian_blur[i][j] = values_gaussian[i][j];

    bmp24_applyFilter(img, gaussian_blur, kernelSize);
    freeKernel(gaussian_blur, kernelSize);
}


void bmp24_outline(t_bmp24 *img) {
    int kernelSize = 3;
    float **outline = createKernel(kernelSize);

    // Edge detection kernel (Laplacian)
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


void bmp24_emboss(t_bmp24 *img) {
    int kernelSize = 3;
    float **emboss = createKernel(kernelSize);

    // Emboss kernel (simulates directional lighting)
    float values_emboss[3][3] = {
        {-2, -1, 0},
        {-1,  1, 1},
        { 0,  1, 2}
    };

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            emboss[i][j] = values_emboss[i][j];

    bmp24_applyFilter(img, emboss, kernelSize);
    freeKernel(emboss, kernelSize);
}


void bmp24_sharpen(t_bmp24 *img) {
    int kernelSize = 3;
    float **sharpen = createKernel(kernelSize);

    // Sharpening kernel (enhances center pixel relative to neighbors)
    float values_sharpen[3][3] = {
        { 0, -1,  0},
        {-1,  5, -1},
        { 0, -1,  0}
    };

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            sharpen[i][j] = values_sharpen[i][j];

    bmp24_applyFilter(img, sharpen, kernelSize);
    freeKernel(sharpen, kernelSize);
}

// ========================================
// COLOR SPACE CONVERSION AND HISTOGRAM FUNCTIONS
// ========================================


t_pixel_YUV ** RGB_to_YUV(t_bmp24 * img)
{
    // Allocate YUV pixel array
    t_pixel_YUV ** YUV = (t_pixel_YUV**)malloc(img->height * sizeof(t_pixel_YUV*));
    for (int i = 0; i < img->height; i++)
        YUV[i] = (t_pixel_YUV *)malloc(img->width * sizeof(t_pixel_YUV));

    // Convert each pixel using standard RGB to YUV conversion formulas
    for (int i = 0; i < img->height; i++)
        for (int j = 0; j < img->width; j++)
        {
            // Y (luminance)
            YUV[i][j].Y = 0.299 * img->data[i][j].red + 0.587 * img->data[i][j].green + 0.114 * img->data[i][j].blue;

            // U (blue-yellow chrominance)
            YUV[i][j].U = -0.14713 * img->data[i][j].red - 0.28886* img->data[i][j].green+ 0.436 * img->data[i][j].blue;

            // V (red-cyan chrominance)
            YUV[i][j].V = 0.625 * img->data[i][j].red - 0.51419 * img->data[i][j].green - 0.10001 * img->data[i][j].blue;
        }
    return YUV;
}


unsigned int * bmp24_computeHistogram(t_bmp24 * img)
{
    // Initialize histogram array to zeros (256 possible brightness values)
    unsigned int * hist = calloc(256 ,sizeof(unsigned int));

    // Convert image to YUV to get luminance values
    t_pixel_YUV ** YUV = RGB_to_YUV(img);

    // Count frequency of each brightness level
    for (int i = 0; i < img->height ; i++)
        for (int j = 0; j < img->width; j++)
        {
            // Round Y value and increment corresponding histogram bin
            hist[(int)round(YUV[i][j].Y)]++;
        }
    for (int i = 0; i < img->height; i++) free(YUV[i]);
    free(YUV);
    return hist;
}


unsigned int * bmp24_computeCDF(unsigned int * hist)
{
    // Compute cumulative sum (CDF)
  unsigned int * cdf = malloc(256 * sizeof(unsigned int));
  unsigned int sum = 0;
  for (int i = 0; i < 256; i++){
      sum += hist[i];          // Add current histogram value to running sum
      cdf[i] = sum;           // Store cumulative sum
  }

    // Calculate equalization mapping
  unsigned int N = cdf[255];              // Total number of pixels
  unsigned int cdf_min = min_arr(cdf,256,N);        // Minimum non-zero CDF value
  unsigned int * hist_eq = malloc(256 * sizeof(unsigned int));
  for (int i = 0; i < 255; i++)
  {
    hist_eq[i] = round((float)(cdf[i] - cdf_min) / (N - cdf_min) * 255);
  }
  return hist_eq;
}

void bmp24_equalize(t_bmp24 *img) {
    // Compute the histogram of the image based on Y (luminance) component
    unsigned * hist = bmp24_computeHistogram(img);

    // Compute the equalized histogram using CDF
    unsigned int * hist_eq = bmp24_computeCDF(hist);

    // Convert RGB pixels to YUV color space to isolate brightness
    t_pixel_YUV ** YUV = RGB_to_YUV(img);

    // For each pixel in the image
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            // Get the original luminance value and ensure it's in [0, 255]
            int y_val = (int)round(YUV[y][x].Y);
            if (y_val < 0) y_val = 0;
            if (y_val > 255) y_val = 255;

            // Map to new equalized luminance using histogram equalization
            float new_Y = (float)hist_eq[y_val];

            // Retrieve U and V chrominance values
            float U = YUV[y][x].U;
            float V = YUV[y][x].V;

            // Convert equalized YUV back to RGB using standard conversion formulas
            int r = round(new_Y + 1.13983 * V);
            int g = round(new_Y - 0.39465 * U - 0.58060 * V);
            int b = round(new_Y + 2.03211 * U);

            // Clamp RGB values to the [0, 255] range
            if (r < 0) r = 0; if (r > 255) r = 255;
            if (g < 0) g = 0; if (g > 255) g = 255;
            if (b < 0) b = 0; if (b > 255) b = 255;

            // Update the pixel in the image with the equalized color
            img->data[y][x].red = (uint8_t)r;
            img->data[y][x].green = (uint8_t)g;
            img->data[y][x].blue = (uint8_t)b;
        }
    }

    // Free all allocated memory
    free(hist_eq);
    free(hist);
    for (int i = 0; i < img->height; i++) free(YUV[i]);
    free(YUV);
}











