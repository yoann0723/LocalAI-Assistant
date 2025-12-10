
set(CORE_SOURCES
	common.h
	capture_export.h
	IAudioCapture.h
	# AudioCaptureSDL.h
	AudioCaptureSDL.cpp
)

add_library(capture STATIC ${CORE_SOURCES})

if (CAPTURE_SDL2)
    # SDL2
    find_package(SDL2 REQUIRED)

    string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)

    message(STATUS "SDL2_INCLUDE_DIRS = ${SDL2_INCLUDE_DIRS}")
    message(STATUS "SDL2_LIBRARIES    = ${SDL2_LIBRARIES}")
endif()

target_link_libraries(capture 
	PRIVATE 
	nlohmann_json
)

# Make the core library's headers accessible to its users
target_include_directories(capture 
	PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR}
)

if (CAPTURE_SDL2)
	target_link_libraries(capture PRIVATE ${SDL2_LIBRARIES})
	target_include_directories(capture PRIVATE ${SDL2_INCLUDE_DIRS})
endif()

# To distinguish between core library and its users
target_compile_definitions(capture PRIVATE CAPTURE_BUILD)
