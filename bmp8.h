/**
 * @file bmp8.h
 * @brief Header file for 8-bit grayscale BMP image processing
 * @author Nolann FOTSO, Rafael ISLAM
 * @date 2024-2025
 *
 * This header file defines the structures and function prototypes for processing
 * 8-bit grayscale BMP images. It provides functionality for loading, saving,
 * and applying various filters and transformations to grayscale images including:
 * - Basic operations (negative, brightness, threshold)
 * - Convolution filters (blur, sharpen, etc.)
 * - Histogram equalization
 *
 */

#ifndef BMP8_H
#define BMP8_H

/**
 * @struct t_bmp8
 * @brief Structure representing an 8-bit grayscale BMP image
 *
 * This structure contains all necessary data for a grayscale BMP image:
 * - File header information (54 bytes)
 * - Color table for grayscale values (1024 bytes)
 * - Actual pixel data
 * - Image dimensions and metadata
 */
typedef struct {
    unsigned char header[54];      /**< BMP file header (54 bytes) */
    unsigned char colorTable[1024]; /**< Color table for grayscale (1024 bytes) */
    unsigned char *data;           /**< Pixel data array */
    unsigned int width;            /**< Image width in pixels */
    unsigned int height;           /**< Image height in pixels */
    unsigned int colorDepth;       /**< Color depth (should be 8 for grayscale) */
    unsigned int dataSize;         /**< Size of pixel data in bytes */
} t_bmp8;

/* ============================================================================
 * IMAGE MANAGEMENT FUNCTIONS
 * ============================================================================ */

/**
 * @brief Load an 8-bit grayscale BMP image from file
 * @param filename Path to the BMP file to load
 * @return Pointer to allocated t_bmp8 structure, or NULL on failure
 *
 * Reads a BMP file and loads it into memory. Validates that the file is
 * a valid 8-bit grayscale BMP before loading.
 */
t_bmp8 *bmp8_loadImage(const char *filename);

/**
 * @brief Save an 8-bit grayscale BMP image to file
 * @param filename Path where to save the BMP file
 * @param img Pointer to the image structure to save
 *
 * Writes the complete BMP image (header, color table, and pixel data) to file.
 */
void bmp8_saveImage(const char *filename, t_bmp8 *img);

/**
 * @brief Free memory allocated for an 8-bit BMP image
 * @param img Pointer to the image structure to free
 *
 * Properly deallocates all memory associated with the image structure.
 */
void bmp8_freeImage(t_bmp8 *img);

/**
 * @brief Print image information to console
 * @param img Pointer to the image structure
 *
 * Displays image metadata including dimensions, color depth, and data size.
 */
void bmp8_printInfo(t_bmp8 *img);

/* ============================================================================
 * BASIC IMAGE PROCESSING FUNCTIONS
 * ============================================================================ */

/**
 * @brief Apply negative filter (color inversion) to image
 * @param img Pointer to the image to modify
 *
 * Inverts all pixel values by computing: new_value = 255 - old_value
 */
void bmp8_negative(t_bmp8 *img);

/**
 * @brief Adjust image brightness
 * @param img Pointer to the image to modify
 * @param value Brightness adjustment value (-255 to +255)
 *
 * Adds the specified value to each pixel, clamping results to [0, 255] range.
 * Positive values brighten the image, negative values darken it.
 */
void bmp8_brightness(t_bmp8 *img, int value);

/**
 * @brief Apply threshold filter (black and white conversion)
 * @param img Pointer to the image to modify
 * @param threshold Threshold value (0-255)
 *
 * Converts image to pure black and white: pixels above threshold become white (255),
 * pixels below become black (0).
 */
void bmp8_threshold(t_bmp8 *img, int threshold);

/* ============================================================================
 * CONVOLUTION AND FILTER FUNCTIONS
 * ============================================================================ */

/**
 * @brief Convert linear pixel array to 2D matrix (bonus function)
 * @param img Pointer to the image
 * @return 2D integer array representing the image pixels
 *
 * Utility function to convert the linear pixel data array into a 2D matrix
 * for easier manipulation in some algorithms.
 */
int **list_to_matrix(t_bmp8 *img);

/**
 * @brief Convert 2D matrix back to linear array
 * @param matrix 2D integer array
 * @param n Dimension of the square matrix
 * @return Linear array of pixel values
 *
 * Utility function to convert a 2D matrix back to linear pixel array format.
 */
unsigned char *matrix_to_list(int **matrix, int n);

/**
 * @brief Apply a convolution filter to the image
 * @param img Pointer to the image to modify
 * @param kernel 2D array representing the convolution kernel
 * @param kernelSize Size of the kernel (assumed to be square)
 *
 * Applies a convolution operation using the provided kernel. Common kernels
 * include blur, sharpen, edge detection, etc. Border pixels are left unchanged.
 */
void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize);

/* ============================================================================
 * HISTOGRAM EQUALIZATION FUNCTIONS
 * ============================================================================ */

/**
 * @brief Compute histogram of pixel intensities
 * @param img Pointer to the image
 * @return Array of 256 integers representing frequency of each intensity
 *
 * Calculates the frequency distribution of pixel values (0-255) in the image.
 */
unsigned int *bmp8_computeHistogram(t_bmp8 *img);

/**
 * @brief Find minimum non-zero value in array
 * @param arr Integer array to search
 * @param n Size of the array
 * @param N Maximum possible value (used for initialization)
 * @return Minimum non-zero value found
 *
 * Helper function for histogram equalization to find the minimum non-zero
 * value in the cumulative distribution function.
 */
unsigned int min_arr(int *arr, int n, int N);

/**
 * @brief Compute cumulative distribution function and equalization mapping
 * @param hist Histogram array (256 values)
 * @return Array mapping old pixel values to equalized values
 *
 * Computes the CDF from the histogram and creates a mapping for histogram
 * equalization to improve image contrast.
 */
unsigned int *bmp8_computeCDF(unsigned int *hist);

/**
 * @brief Apply histogram equalization to improve image contrast
 * @param img Pointer to the image to modify
 *
 * Applies histogram equalization algorithm to redistribute pixel intensities
 * and improve overall image contrast and visibility.
 */
void bmp8_equalize(t_bmp8 *img);

#endif //BMP8_H