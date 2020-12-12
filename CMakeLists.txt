cmake_minimum_required(VERSION 3.0.2)

set(CMAKE_VERBOSE_MAKEFILE TRUE)

project(fox-toolkit VERSION 1.6.57)

include(CheckCXXCompilerFlag)
INCLUDE (CheckIncludeFiles)


# Libraries
add_subdirectory(utils)
add_subdirectory(lib)

