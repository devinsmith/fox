cmake_minimum_required(VERSION 3.10)

set(CMAKE_VERBOSE_MAKEFILE TRUE)

project(fox-toolkit VERSION 1.7.81)

include(CheckCXXCompilerFlag)
INCLUDE (CheckIncludeFiles)

# Libraries
add_subdirectory(utils)
add_subdirectory(lib)

# Sample applications and tests
add_subdirectory(adie)
add_subdirectory(calculator)
add_subdirectory(chart)
add_subdirectory(controlpanel)
# glviewer currently broken, need better CMake gl tests
#add_subdirectory(glviewer)
add_subdirectory(pathfinder)
add_subdirectory(shutterbug)
add_subdirectory(tests)

