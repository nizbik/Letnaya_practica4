# MyFunctions.cmake - вспомогательные функции для CMake.
# Зубехин Никита
# МК-101


if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra)
endif()
