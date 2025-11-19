
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CORE_SOURCES
    PluginManager.h
	IPlugin.h
	InferenceEngine.h
	IModelProvider.h
	AIModelHub.h
	PluginManager.cpp
	InferenceEngine.cpp
	AIModelHub.cpp
)

add_library(core STATIC ${CORE_SOURCES})

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
