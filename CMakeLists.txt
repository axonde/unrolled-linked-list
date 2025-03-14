cmake_minimum_required(VERSION 3.12)

project(
    unrolled_list
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)

include_directories(lib)

add_subdirectory(bin)

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

enable_testing()
add_subdirectory(tests)
