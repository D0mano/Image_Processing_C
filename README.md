# 📸 Image Processing in C

## 📄 Description

This project is a full-featured image processing program written in C. It supports operations on BMP images in both 8-bit grayscale and 24-bit color formats. The program is structured to allow modular development and execution via a command-line interface, enabling users to load images, apply a variety of filters, and save the processed results.

The project was completed as part of the "Algorithmic and Data Structures 1" course (TI202I) at Efrei Paris, academic year 2024-2025.

## 🛠️ Features

### Part 1: 8-bit Grayscale Image Processing

* Load and save 8-bit BMP images
* Negative filter (color inversion)
* Brightness adjustment
* Thresholding (black and white conversion)
* Convolution filters:

    * Box Blur
    * Gaussian Blur
    * Outline
    * Emboss
    * Sharpen

### Part 2: 24-bit Color Image Processing

* Load and save 24-bit BMP images
* Negative filter
* Grayscale conversion
* Brightness adjustment
* Same convolution filters as grayscale, applied to RGB channels

### Part 3: Histogram Equalization

* Compute image histogram for 8-bit images
* Compute and normalize the cumulative distribution function (CDF)
* Equalize grayscale images
* Equalize color images by converting to YUV, adjusting the Y (luminance) component, and converting back to RGB

### Bonus 
* The possibilities to make a mirrored image on the x and y axis 
* New filters :
  * Sepia tone
  * Sobel x
  * Sobel y
  * Motion blur

## 📁 Project Structure

```
d0mano-image_processing_c/
├── bmp24.c                    # 24-bit image processing functions
├── bmp24.h
├── bmp8.c                     # 8-bit image processing functions
├── bmp8.h
├── CMakeLists.txt             # CMake build configuration
├── flowers_color.bmp          # Sample 24-bit color image
├── lena_color.bmp             # Additional test image (color)
├── lena_gray.bmp              # Additional test image (grayscale)
├── barbara_gray.bmp           # Sample 8-bit grayscale image
├── main.c                     # Main program and CLI interface
├── output.bmp                 # Output file for processed image
└── cmake-build-debug/         # CMake build directory (ignored)
```

## 🧑‍🤝‍🧑 Authors

* **Nolann FOTSO**
* **Rafael ISLAM**

## 📚 Notes

* BMP format is used for its simplicity and uncompressed structure.
* The project is built using C standard library functions only.
* Image data is manipulated using raw byte and structure access based on BMP header specifications.

## 🐞 Known Issues

Inconsistent `printf` output after `scanf` calls. During the use of the user interface, we observed that some `printf` statements may not appear immediately after `scanf`inputs. This issue is related to the way standard input/output buffering works in C. Specifically, the output buffer may not flush as expected after a `scanf`, causing some printed messages to be delayed or skipped on screen.


---

Thank you for reviewing our work!
