
set(CORE_SOURCES
	IModelProvider.h
	IModelProvider.cpp
	embedding/EmbeddingProviderSbert.h
	embedding/EmbeddingProviderSbert.cpp
	LLM/LLMProviderLlama.h
	LLM/LLMProviderLlama.cpp
	ASR/ASRProviderWhisper.h
	ASR/ASRProviderWhisper.cpp
	vision/VisionProvider.h
	vision/VisionProvider.cpp
)

add_library(model_provider STATIC ${CORE_SOURCES})

# link the ggml library for Llama models from specific path
target_link_libraries(model_provider 
	PRIVATE 
	nlohmann_json
	tl
	# Whisper.cpp library
	${CMAKE_CURRENT_SOURCE_DIR}/../deps/whisper.cpp/lib/whisper.lib

	# Llama.cpp libraries
	${CMAKE_CURRENT_SOURCE_DIR}/../deps/llama.cpp/lib/llama.lib

	# GGML libraries
	${CMAKE_CURRENT_SOURCE_DIR}/../deps/ggml/lib/ggml-base.lib
	${CMAKE_CURRENT_SOURCE_DIR}/../deps/ggml/lib/ggml-cpu.lib
	${CMAKE_CURRENT_SOURCE_DIR}/../deps/ggml/lib/ggml.lib
	${CMAKE_CURRENT_SOURCE_DIR}/../deps/ggml/lib/ggml-cuda.lib)

target_include_directories(model_provider PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/../deps/whisper.cpp/include
	${CMAKE_CURRENT_SOURCE_DIR}/../deps/llama.cpp/include
	${CMAKE_CURRENT_SOURCE_DIR}/../deps/ggml/include)

# Make the core library's headers accessible to its users
target_include_directories(model_provider PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# To distinguish between core library and its users
target_compile_definitions(model_provider PRIVATE MODEL_PROVIDER_BUILD)