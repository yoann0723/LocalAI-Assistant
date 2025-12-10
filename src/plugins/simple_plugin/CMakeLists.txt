
set(SIMPLE_PLUGIN_SOURCES
    simple_plugin.h
	simple_plugin.cpp
)

add_library(simple_plugin SHARED ${SIMPLE_PLUGIN_SOURCES})

target_link_libraries(simple_plugin PRIVATE nlohmann_json core)
