cmake_minimum_required(VERSION 3.16)
project(ai-assistant)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Make nlohmann_json available as an interface library so that it can be linked to other targets
add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json
    INTERFACE ${PROJECT_SOURCE_DIR}/vendor/nlohmann
)

add_library(tl INTERFACE)
target_include_directories(tl
    INTERFACE ${PROJECT_SOURCE_DIR}/vendor/tl
)

# Add a convienience function to deploy Qt libraries after building a target
function(deploy_qt target)
  add_custom_command(TARGET ${target}
    POST_BUILD
    COMMAND "${CMAKE_COMMAND}"
      "-DCONFIG=$<CONFIGURATION>"
      "-DQT_DIR=${QTDIR}"
      "-DDEV_OUTPUT_DIR=${DEV_OUTPUT_DIR}"
      "-DTARGET=${target}"
      "-DOS_WINDOWS=${OS_WINDOWS}"
      "-DOS_MACOS=${OS_MACOS}"
      "-DQML_DIR=${SOURCE_DIR}/frontend/qml"
      -P "${SOURCE_DIR}/cmake/deploy_qt.cmake"
    VERBATIM)
endfunction()

# Common output base: build/bin, build/lib
set(OUTPUT_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set(OUTPUT_LIB_DIR ${CMAKE_BINARY_DIR}/lib)

# Configure output directories per build type
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
string(TOUPPER ${CMAKE_BUILD_TYPE} CONFIG_UP)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UP} ${OUTPUT_BIN_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UP} ${OUTPUT_LIB_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UP} ${OUTPUT_LIB_DIR}/${CMAKE_BUILD_TYPE})

add_subdirectory(capture)
add_subdirectory(inference-provider)
add_subdirectory(model-provider)
add_subdirectory(core)
add_subdirectory(frontend)
add_subdirectory(plugins/simple_plugin)

