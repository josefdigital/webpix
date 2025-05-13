cmake_minimum_required(VERSION 3.30)
project(webpix C)

set(CMAKE_C_STANDARD 11)
set(BREW_ROOT "/opt/homebrew/Cellar")

add_executable(webpix main.c)

target_include_directories(
        webpix
        PRIVATE
        ${BREW_ROOT}/libpng/1.6.45/include
        ${BREW_ROOT}/jpeg-turbo/3.1.0/include
)

target_link_libraries(
        webpix
        PRIVATE
        ${BREW_ROOT}/libpng/1.6.45/lib/libpng.dylib
        ${BREW_ROOT}/jpeg-turbo/3.1.0/lib/libjpeg.dylib
)