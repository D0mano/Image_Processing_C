cmake_minimum_required(VERSION 3.30)
project(Image_Processing_C C)

set(CMAKE_C_STANDARD 11)

add_executable(Image_Processing_C main.c
        bmp8.h
        bmp8.c
        bmp24.c
        bmp24.h)
