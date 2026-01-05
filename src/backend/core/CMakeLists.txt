
set(CORE_SOURCES
    PluginManager.h
	core_export.h
	core_common.h
	# localai_c_api.h
	localai_c_api.cpp
	# localai_cxx_api.h
	LocalAIContext.h
	LocalAIContext.cpp
	# LocalAI_ModelInfo.cpp
	CoreStatus.cpp
	# IPlugin.h
	# InferenceEngine.h
	# InferenceEngine.cpp
	# IModelProvider.h
	PluginManager.cpp
	AIModelHub.h
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
	Pipeline.h
	Pipeline.cpp
	ASREngine.h
	ASREngine.cpp
	# ThreadPool.h
	# ThreadPool.cpp
	# ModelBridge.h
	# ModelBridge.cpp
)

# Use shared library to allow dynamic linking
add_library(localai SHARED ${CORE_SOURCES})

set(ONNX_DEPS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../deps/onnxruntime)

target_link_libraries(localai 
	PRIVATE 
	nlohmann_json
	tl
	model_provider
	localai_api
	# Whisper.cpp library
	${CMAKE_CURRENT_SOURCE_DIR}/../deps/whisper.cpp/lib/whisper.lib

	# Llama.cpp libraries
	${CMAKE_CURRENT_SOURCE_DIR}/../deps/llama.cpp/lib/llama.lib

	# GGML libraries
	${CMAKE_CURRENT_SOURCE_DIR}/../deps/ggml/lib/ggml-base.lib
	${CMAKE_CURRENT_SOURCE_DIR}/../deps/ggml/lib/ggml-cpu.lib
	${CMAKE_CURRENT_SOURCE_DIR}/../deps/ggml/lib/ggml.lib
	${CMAKE_CURRENT_SOURCE_DIR}/../deps/ggml/lib/ggml-cuda.lib
	
	# onnxruntime library
	${ONNX_DEPS_DIR}/lib/onnxruntime.lib
)

target_include_directories(inference_provider PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/../deps/whisper.cpp/include
	${CMAKE_CURRENT_SOURCE_DIR}/../deps/llama.cpp/include
	${CMAKE_CURRENT_SOURCE_DIR}/../deps/ggml/include)

# if (ONNX_RUNTIME_SUPPORT)
if(1)
	target_include_directories(inference_provider PRIVATE 
	"${ONNX_DEPS_DIR}/include"                           # Pre-built package
    "${ONNX_DEPS_DIR}/include/core/session") # Windows local install
endif()

# Make the core library's headers accessible to its users
# target_include_directories(localai 
# 	PUBLIC 
# 	${CMAKE_CURRENT_SOURCE_DIR}
# )

# To distinguish between core library and its users
target_compile_definitions(localai PRIVATE CORE_BUILD)
