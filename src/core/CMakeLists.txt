
set(CORE_SOURCES
    PluginManager.h
	core_export.h
	core_common.h
	core_c_api.h
	core_c_api.cpp
	LocalAIContext.cpp
	LocalAI_ModelInfo.cpp
	CoreStatus.cpp
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
	ThreadPool.h
	ThreadPool.cpp
	# ModelBridge.h
	# ModelBridge.cpp
)

# Use shared library to allow dynamic linking
add_library(core SHARED ${CORE_SOURCES})

target_link_libraries(core 
	PRIVATE 
	nlohmann_json
	tl
	model_provider
)

# Make the core library's headers accessible to its users
target_include_directories(core 
	PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR}
)

# To distinguish between core library and its users
target_compile_definitions(core PRIVATE CORE_BUILD)
