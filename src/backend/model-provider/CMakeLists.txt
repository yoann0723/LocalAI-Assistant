
set(CORE_SOURCES
	IModelProvider.h
	IModelProvider.cpp
	IVisionProvider.cpp
	IVisionProvider.h
	ILLMProvider.cpp
	ILLMProvider.h
	IASRProvider.cpp
	IASRProvider.h
	IEmbeddingProvider.cpp
	IEmbeddingProvider.h
	ICustomModelProvider.h
	ICustomModelProvider.cpp
	ThreadPool.h
	ThreadPool.cpp
	# embedding/EmbeddingProviderSbert.h
	# embedding/EmbeddingProviderSbert.cpp
	# LLM/LLMProviderLlama.h
	# LLM/LLMProviderLlama.cpp
	# ASR/ASRProviderWhisper.h
	# ASR/ASRProviderWhisper.cpp
	# vision/VisionProvider.h
	# vision/VisionProvider.cpp
)

add_library(model_provider STATIC ${CORE_SOURCES})

target_link_libraries(model_provider 
	PRIVATE 
	nlohmann_json
	tl
	inference_provider)

# Make the core library's headers accessible to its users
target_include_directories(model_provider PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# To distinguish between core library and its users
target_compile_definitions(model_provider PRIVATE MODEL_PROVIDER_BUILD)