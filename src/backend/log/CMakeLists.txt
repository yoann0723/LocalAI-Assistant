
set(LOCALAI_LOG_SOURCES
    Log.h
	Log.cpp
)

add_library(log STATIC ${LOCALAI_LOG_SOURCES})

target_include_directories(log 
	PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR}
)

# To distinguish between core library and its users
target_compile_definitions(log PRIVATE LOCALAI_LOG_BUILD)
