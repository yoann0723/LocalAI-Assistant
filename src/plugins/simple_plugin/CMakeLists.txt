
set(SIMPLE_PLUGIN_SOURCES
    simple_plugin.h
	simple_plugin.cpp
)

add_library(simple_plugin SHARED ${SIMPLE_PLUGIN_SOURCES})

# Include localai headers
target_include_directories(simple_plugin PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include)

target_link_libraries(simple_plugin PRIVATE nlohmann_json localai)
