# CMakeLists.txt - корневой файл сборки проекта.
# Зубехин Никита
# МК-101

cmake_minimum_required(VERSION 3.10)
project(letnaya_practica_4 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(${CMAKE_SOURCE_DIR}/cmake/MyFunctions.cmake)

add_subdirectory(lib)
add_subdirectory(app)
