cmake_minimum_required(VERSION 3.16)
project(CProject)

set(CMAKE_C_STANDARD 11)

add_executable(CProject
        main.c
        bmp8.c
        bmp24.c
)