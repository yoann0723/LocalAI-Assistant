
set(INFERENCE_PROVIDER_SOURCES
	IInferenceProvider.h
	IInferenceProvider.cpp
	llama/LlamaInferenceProvider.h
	llama/LlamaInferenceProvider.cpp
	onnxruntime/OrtInferenceProvider.h
	onnxruntime/OrtInferenceProvider.cpp
	whisper/WhisperASRProvider.cpp
	whisper/WhisperASRProvider.h
)

add_library(inference_provider STATIC ${INFERENCE_PROVIDER_SOURCES})

add_definitions(-DGGML_SHARED)
add_definitions(-DLLAMA_SHARED)
add_definitions(-DORT_DLL_IMPORT)

set(ONNX_DEPS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../deps/onnxruntime)

target_link_libraries(inference_provider 
	PRIVATE
	tl
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
	${ONNX_DEPS_DIR}/lib/onnxruntime.lib)

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
target_include_directories(inference_provider PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# To distinguish between core library and its users
target_compile_definitions(inference_provider PRIVATE INFERENCE_PROVIDER_BUILD)