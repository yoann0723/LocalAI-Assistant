cmake_minimum_required(VERSION 3.16)
# project(localai)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Common output base: build/bin, build/lib
set(OUTPUT_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set(OUTPUT_LIB_DIR ${CMAKE_BINARY_DIR}/lib)

# Configure output directories per build type
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
string(TOUPPER ${CMAKE_BUILD_TYPE} CONFIG_UP)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UP} ${OUTPUT_BIN_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UP} ${OUTPUT_LIB_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UP} ${OUTPUT_LIB_DIR}/${CMAKE_BUILD_TYPE})

add_subdirectory(log)
add_subdirectory(inference-provider)
add_subdirectory(model-provider)
add_subdirectory(core)

