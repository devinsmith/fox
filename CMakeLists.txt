cmake_minimum_required(VERSION 3.10)

set(CMAKE_VERBOSE_MAKEFILE TRUE)

project(fox-toolkit VERSION 1.7.75)

include(CheckCXXCompilerFlag)
INCLUDE (CheckIncludeFiles)

# Libraries
add_subdirectory(utils)
add_subdirectory(lib)

