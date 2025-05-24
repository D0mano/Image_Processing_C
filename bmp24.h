/**
 * @file bmp24.h
 * @brief Header file for 24-bit color BMP image processing
 * @author Nolann FOTSO, Rafael ISLAM
 * @date 2024-2025
 * 
 * This header file defines the structures and function prototypes for processing
 * 24-bit color BMP images. It provides comprehensive functionality for loading,
 * saving, and applying various filters and transformations to color images including:
 * - Basic operations (negative, brightness, grayscale conversion)
 * - Convolution filters (blur, sharpen, emboss, outline)
 * - Advanced histogram equalization using YUV color space
 * 
 * Part of the "Algorithmic and Data Structures 1" course project at Efrei Paris.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef BMP24_H
#define BMP24_H

/* ============================================================================
 * BMP FILE FORMAT CONSTANTS
 * ============================================================================ */

// Offsets for the BMP header fields
#define BITMAP_MAGIC     0x00  /**< Offset to file type identifier */
#define BITMAP_SIZE      0x02  /**< Offset to file size */
#define BITMAP_OFFSET    0x0A  /**< Offset to pixel data start */
#define BITMAP_WIDTH     0x12  /**< Offset to image width */
#define BITMAP_HEIGHT    0x16  /**< Offset to image height */
#define BITMAP_DEPTH     0x1C  /**< Offset to color depth */
#define BITMAP_SIZE_RAW  0x22  /**< Offset to raw image data size */

// BMP file constants
#define BMP_TYPE         0x4D42 /**< 'BM' in hexadecimal - BMP file identifier */
#define HEADER_SIZE      0x0E   /**< BMP file header size (14 bytes) */
#define INFO_SIZE        0x28   /**< BMP info header size (40 bytes) */
#define DEFAULT_DEPTH    0x18   /**< Default color depth (24 bits) */

/* ============================================================================
 * STRUCTURE DEFINITIONS
 * ============================================================================ */

/**
 * @struct t_bmp_header
 * @brief BMP file header structure (14 bytes)
 * 
 * Contains basic file information including file type, size, and offset
 * to pixel data. This is the first part of any BMP file.
 */
typedef struct {
    uint16_t type;       /**< File type identifier ('BM' = 0x4D42) */
    uint32_t size;       /**< Total file size in bytes */
    uint16_t reserved1;  /**< Reserved field (unused) */
    uint16_t reserved2;  /**< Reserved field (unused) */
    uint32_t offset;     /**< Offset to start of pixel data */
} t_bmp_header;

/**
 * @struct t_bmp_info
 * @brief BMP information header structure (40 bytes)
 * 
 * Contains detailed image information including dimensions, color depth,
 * compression type, and resolution data.
 */
typedef struct {
    uint32_t size;           /**< Size of this header (40 bytes) */
    int32_t width;           /**< Image width in pixels */
    int32_t height;          /**< Image height in pixels */
    uint16_t planes;         /**< Number of color planes (always 1) */
    uint16_t bits;           /**< Bits per pixel (24 for true color) */
    uint32_t compression;    /**< Compression type (0 = uncompressed) */
    uint32_t imagesize;      /**< Size of image data in bytes */
    int32_t xresolution;     /**< Horizontal resolution (pixels/meter) */
    int32_t yresolution;     /**< Vertical resolution (pixels/meter) */
    uint32_t ncolors;        /**< Number of colors in palette (0 for 24-bit) */
    uint32_t importantcolors; /**< Number of important colors */
} t_bmp_info;

/**
 * @struct t_pixel
 * @brief RGB pixel structure for 24-bit color
 * 
 * Represents a single pixel with red, green, and blue color components.
 * Each component is 8 bits (0-255).
 */
typedef struct {
    uint8_t red;    /**< Red color component (0-255) */
    uint8_t green;  /**< Green color component (0-255) */
    uint8_t blue;   /**< Blue color component (0-255) */
} t_pixel;

/**
 * @struct t_pixel_YUV
 * @brief YUV color space pixel representation
 * 
 * Used for histogram equalization in color images. YUV separates
 * luminance (Y) from chrominance (U,V) for better color processing.
 */
typedef struct {
    float Y;  /**< Luminance component */
    float U;  /**< Blue-difference chroma component */
    float V;  /**< Red-difference chroma component */
} t_pixel_YUV;

/**
 * @struct t_bmp24
 * @brief Complete 24-bit BMP image structure
 * 
 * Contains all data needed to represent and manipulate a 24-bit color BMP image,
 * including headers, metadata, and pixel data organized as a 2D array.
 */
typedef struct {
    t_bmp_header header;      /**< BMP file header */
    t_bmp_info header_info;   /**< BMP information header */
    int width;                /**< Image width (convenience copy) */
    int height;               /**< Image height (convenience copy) */
    int colorDepth;           /**< Color depth (convenience copy) */
    t_pixel **data;           /**< 2D array of pixel data [height][width] */
} t_bmp24;

/* ============================================================================
 * MEMORY MANAGEMENT FUNCTIONS
 * ============================================================================ */

/**
 * @brief Allocate memory for 2D pixel array
 * @param width Image width in pixels
 * @param height Image height in pixels
 * @return Pointer to allocated 2D pixel array, or NULL on failure
 * 
 * Allocates a 2D array of t_pixel structures for storing image data.
 */
t_pixel **bmp24_allocateDataPixels(int width, int height);

/**
 * @brief Free memory allocated for 2D pixel array
 * @param pixels Pointer to 2D pixel array to free
 * @param height Height of the array (needed for proper deallocation)
 * 
 * Properly deallocates a 2D pixel array including all rows.
 */
void bmp24_freeDataPixels(t_pixel **pixels, int height);

/**
 * @brief Allocate and initialize a complete BMP24 structure
 * @param width Image width in pixels
 * @param height Image height in pixels
 * @param colorDepth Color depth (should be 24)
 * @return Pointer to allocated t_bmp24 structure, or NULL on failure
 * 
 * Creates a complete BMP24 structure with allocated pixel data array.
 */
t_bmp24 *bmp24_allocate(int width, int height, int colorDepth);

/**
 * @brief Free all memory associated with a BMP24 structure
 * @param img Pointer to BMP24 structure to free
 * 
 * Completely deallocates a BMP24 structure including pixel data.
 */
void bmp24_free(t_bmp24 *img);

/* ============================================================================
 * FILE I/O FUNCTIONS
 * ============================================================================ */

/**
 * @brief Load a 24-bit BMP image from file
 * @param filename Path to the BMP file to load
 * @return Pointer to loaded t_bmp24 structure, or NULL on failure
 * 
 * Reads and validates a 24-bit BMP file, loading all headers and pixel data
 * into memory. Performs format validation before loading.
 */
t_bmp24 *bmp24_loadImage(const char *filename);

/**
 * @brief Save a 24-bit BMP image to file
 * @param img Pointer to BMP24 structure to save
 * @param filename Path where to save the BMP file
 * 
 * Writes a complete BMP file including headers and pixel data.
 */
void bmp24_saveImage(t_bmp24 *img, const char *filename);

/**
 * @brief Print detailed image information to console
 * @param img Pointer to BMP24 structure
 * 
 * Displays comprehensive image metadata for debugging and information.
 */
void bmp24_printInfo(t_bmp24 *img);

/* ============================================================================
 * LOW-LEVEL FILE ACCESS FUNCTIONS
 * ============================================================================ */

/**
 * @brief Read raw data from specific file position
 * @param position File position to read from
 * @param buffer Buffer to store read data
 * @param size Size of each element to read
 * @param n Number of elements to read
 * @param file File pointer to read from
 * 
 * Seeks to specified position and reads raw binary data.
 */
void file_rawRead(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file);

/**
 * @brief Write raw data to specific file position
 * @param position File position to write to
 * @param buffer Buffer containing data to write
 * @param size Size of each element to write
 * @param n Number of elements to write
 * @param file File pointer to write to
 * 
 * Seeks to specified position and writes raw binary data.
 */
void file_rawWrite(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file);

/* ============================================================================
 * PIXEL-LEVEL I/O FUNCTIONS
 * ============================================================================ */

/**
 * @brief Read a single pixel value from file
 * @param image Pointer to BMP24 structure
 * @param x X coordinate of pixel
 * @param y Y coordinate of pixel
 * @param file File pointer to read from
 * 
 * Reads RGB values for a specific pixel, handling BMP row padding.
 */
void bmp24_readPixelValue(t_bmp24 *image, int x, int y, FILE *file);

/**
 * @brief Read all pixel data from file
 * @param image Pointer to BMP24 structure
 * @param file File pointer to read from
 * 
 * Reads all pixel data from file into the image structure.
 */
void bmp24_readPixelData(t_bmp24 *image, FILE *file);

/**
 * @brief Write a single pixel value to file
 * @param image Pointer to BMP24 structure
 * @param x X coordinate of pixel
 * @param y Y coordinate of pixel
 * @param file File pointer to write to
 * 
 * Writes RGB values for a specific pixel, handling BMP row padding.
 */
void bmp24_writePixelValue(t_bmp24 *image, int x, int y, FILE *file);

/**
 * @brief Write all pixel data to file
 * @param image Pointer to BMP24 structure
 * @param file File pointer to write to
 * 
 * Writes all pixel data from image structure to file.
 */
void bmp24_writePixelData(t_bmp24 *image, FILE *file);

/* ============================================================================
 * BASIC IMAGE PROCESSING FUNCTIONS
 * ============================================================================ */

/**
 * @brief Apply negative filter (color inversion) to image
 * @param img Pointer to image to modify
 * 
 * Inverts all color components: new_value = 255 - old_value for each RGB channel.
 */
void bmp24_negative(t_bmp24 *img);

/**
 * @brief Convert color image to grayscale
 * @param img Pointer to image to modify
 * 
 * Converts color image to grayscale using simple averaging method:
 * gray = (R + G + B) / 3 for each pixel.
 */
void bmp24_grayscale(t_bmp24 *img);

/**
 * @brief Adjust image brightness
 * @param img Pointer to image to modify
 * @param value Brightness adjustment value (-255 to +255)
 * 
 * Adds specified value to each RGB component, clamping to [0, 255] range.
 */
void bmp24_brightness(t_bmp24 *img, int value);

/* ============================================================================
 * CONVOLUTION AND KERNEL FUNCTIONS
 * ============================================================================ */

/**
 * @brief Create a square convolution kernel
 * @param size Size of the square kernel
 * @return Pointer to allocated 2D float array
 * 
 * Allocates memory for a square convolution kernel matrix.
 */
float **createKernel(int size);

/**
 * @brief Free memory allocated for convolution kernel
 * @param kernel Pointer to kernel to free
 * @param size Size of the kernel
 * 
 * Properly deallocates a 2D kernel array.
 */
void freeKernel(float **kernel, int size);

/**
 * @brief Apply convolution operation to a single pixel
 * @param img Pointer to image
 * @param x X coordinate of center pixel
 * @param y Y coordinate of center pixel
 * @param kernel Convolution kernel
 * @param kernelSize Size of the kernel
 * @return Computed pixel value after convolution
 * 
 * Performs convolution operation on a single pixel using the provided kernel.
 */
t_pixel bmp24_convolution(t_bmp24 *img, int x, int y, float **kernel, int kernelSize);

/**
 * @brief Apply convolution filter to entire image
 * @param img Pointer to image to modify
 * @param kernel Convolution kernel
 * @param kernelSize Size of the kernel
 * 
 * Applies convolution filter to all applicable pixels in the image.
 */
void bmp24_applyFilter(t_bmp24 *img, float **kernel, const int kernelSize);

/* ============================================================================
 * PREDEFINED FILTER FUNCTIONS
 * ============================================================================ */

/**
 * @brief Apply box blur filter to image
 * @param img Pointer to image to modify
 * 
 * Applies a 3x3 box blur (averaging) filter for smooth blurring effect.
 */
void bmp24_boxBlur(t_bmp24 *img);

/**
 * @brief Apply Gaussian blur filter to image
 * @param img Pointer to image to modify
 * 
 * Applies a 3x3 Gaussian blur filter for natural-looking blur effect.
 */
void bmp24_gaussianBlur(t_bmp24 *img);

/**
 * @brief Apply outline (edge detection) filter to image
 * @param img Pointer to image to modify
 * 
 * Applies an edge detection filter to highlight image outlines and edges.
 */
void bmp24_outline(t_bmp24 *img);

/**
 * @brief Apply emboss filter to image
 * @param img Pointer to image to modify
 * 
 * Applies emboss filter to create a raised, 3D-like appearance.
 */
void bmp24_emboss(t_bmp24 *img);

/**
 * @brief Apply sharpen filter to image
 * @param img Pointer to image to modify
 * 
 * Applies sharpening filter to enhance image details and edges.
 */
void bmp24_sharpen(t_bmp24 *img);

/* ============================================================================
 * COLOR SPACE CONVERSION AND HISTOGRAM FUNCTIONS
 * ============================================================================ */

/**
 * @brief Convert RGB image to YUV color space
 * @param img Pointer to RGB image
 * @return 2D array of YUV pixels
 * 
 * Converts RGB color space to YUV for better luminance-based processing.
 * Used primarily for histogram equalization in color images.
 */
t_pixel_YUV **RGB_to_YUV(t_bmp24 *img);


/**
 * @brief Compute histogram of luminance values
 * @param img Pointer to color image
 * @return Array of 256 integers representing luminance frequency distribution
 * 
 * Computes histogram based on Y (luminance) component in YUV color space.
 */
unsigned int *bmp24_computeHistogram(t_bmp24 *img);

/**
 * @brief Compute cumulative distribution function for histogram equalization
 * @param hist Histogram array
 * @return Mapping array for histogram equalization
 * 
 * Computes CDF and creates mapping for histogram equalization of color images.
 */
unsigned int *bmp24_computeCDF(unsigned int *hist);

/**
 * @brief Apply histogram equalization to color image
 * @param img Pointer to image to modify
 * 
 * Applies histogram equalization by converting to YUV, equalizing the Y component,
 * and converting back to RGB for improved contrast in color images.
 */
void bmp24_equalize(t_bmp24 *img);

#endif //BMP24_H