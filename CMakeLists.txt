cmake_minimum_required(VERSION 3.0)
project(my-bittorrent LANGUAGES C)

enable_testing()
add_compile_options(-Wall -Wextra -Werror -pedantic -std=c99)

add_compile_options("-fsanitize=address")
link_libraries("-fsanitize=address")

set(CMAKE_VERBOSE_MAKEFILE OFF)

add_executable(
        my-bittorrent
        src/main.c
    )

add_custom_target(
        check
        COMMENT "Launching test suite"
        DEPENDS my-bittorrent
        COMMAND echo "no test suite"
    )
