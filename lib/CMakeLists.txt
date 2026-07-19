# CMake-конфигурация для модуля библиотеки
# Зубехин Никита
# МК-101


add_library(large_blocks STATIC
    src/large_blocks.c
)

target_include_directories(large_blocks PUBLIC include)
