cmake_minimum_required (VERSION 3.14)

project (ZXingWasm)

include (${CMAKE_CURRENT_SOURCE_DIR}/../../zxing.cmake)
zxing_add_package_stb()

set (CMAKE_CXX_STANDARD 20)

option (BUILD_WRITERS "Build with writer support (encoders)" ON)
option (BUILD_READERS "Build with reader support (decoders)" ON)

set(BUILD_EMSCRIPTEN_ENVIRONMENT "web,worker" CACHE STRING "Optimize build for given emscripten runtime environment (web/node/shell/worker)")

if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

add_definitions ("-s DISABLE_EXCEPTION_CATCHING=0")

add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/../../core ${CMAKE_BINARY_DIR}/ZXing)

set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --bind -s ENVIRONMENT=${BUILD_EMSCRIPTEN_ENVIRONMENT} -s DISABLE_EXCEPTION_CATCHING=0 -s FILESYSTEM=0 -s EXPORT_ES6=1 -s MODULARIZE=1 -s MIN_SAFARI_VERSION=130000 -s DYNAMIC_EXECUTION=0 -s EXPORT_NAME=ZXing -s EXPORTED_FUNCTIONS=\"['_malloc', '_free']\" -s ALLOW_MEMORY_GROWTH=1")

if (BUILD_READERS AND BUILD_WRITERS)
    add_executable (zxing_full BarcodeReader.cpp BarcodeWriter.cpp)
    target_link_libraries (zxing_full ZXing::ZXing stb::stb)
endif()

if (BUILD_READERS)
    add_executable (zxing_reader BarcodeReader.cpp)
    target_link_libraries (zxing_reader ZXing::ZXing stb::stb)
endif()

if (BUILD_WRITERS)
    add_executable (zxing_writer BarcodeWriter.cpp )
    target_link_libraries (zxing_writer ZXing::ZXing stb::stb)
endif()

