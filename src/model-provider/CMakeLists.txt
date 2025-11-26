set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CORE_SOURCES
	IModelProvider.h
	IModelProvider.cpp
	embedding/EmbeddingProviderSbert.h
	embedding/EmbeddingProviderSbert.cpp
	LLM/LLMProviderLlama.h
	LLM/LLMProviderLlama.cpp
	STT/STTProviderWhisper.h
	STT/STTProviderWhisper.cpp
	vision/VisionProvider.h
	vision/VisionProvider.cpp
)

add_library(model_provider SHARED ${CORE_SOURCES})

target_link_libraries(model_provider PRIVATE nlohmann_json)

# Make the core library's headers accessible to its users
target_include_directories(model_provider PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# To distinguish between core library and its users
target_compile_definitions(model_provider PRIVATE MODEL_PROVIDER_BUILD)