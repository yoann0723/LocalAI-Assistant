
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CORE_SOURCES
    PluginManager.h
	CommonDef.h
	IPlugin.h
	InferenceEngine.h
	# IModelProvider.h
	AIModelHub.h
	PluginManager.cpp
	InferenceEngine.cpp
	AIModelHub.cpp
	RagRetriever.h
	RagRetriever.cpp
	ConversationManager.h
	ConversationManager.cpp
	LLMEngine.h
	LLMEngine.cpp
	Orchestrator.h
	Orchestrator.cpp
	PromptGenerator.h
	PromptGenerator.cpp 
)

# Use shared library to allow dynamic linking
add_library(core SHARED ${CORE_SOURCES})

target_link_libraries(core PRIVATE nlohmann_json model_provider)

# Make the core library's headers accessible to its users
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# To distinguish between core library and its users
target_compile_definitions(core PRIVATE CORE_BUILD)
