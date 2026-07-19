# CMakeLists.txt - настройки сборки основной программы.
# Зубехин Никита
# МК-101

add_executable(letnaya_practica_4 main.c)

target_link_libraries(letnaya_practica_4 PRIVATE large_blocks)
