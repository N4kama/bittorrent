cmake_minimum_required(VERSION 3.0)
project(my-bittorrent LANGUAGES C)

enable_testing()
add_compile_options(-g -Wall -Wextra -Werror -pedantic -std=c99 -D_GNU_SOURCE -D_DEFAULT_SOURCE)

add_compile_options("-fsanitize=address")
link_libraries("-fsanitize=address -ljansson -lssl -lcrypto -lcurl")

set(CMAKE_VERBOSE_MAKEFILE OFF)
add_executable(
        my-bittorrent
        src/main.c
        src/encode/decode.c
        src/encode/encode.c
        src/connect/connect.c
    )

find_library(BENCODE bencode lib)
target_link_libraries(
    my-bittorrent
    ${BENCODE})



add_custom_target(
        check
        COMMENT "Launching test suite"
        DEPENDS my-bittorrent
        COMMAND echo "no test suite"
    )
