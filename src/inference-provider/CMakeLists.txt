
set(INFERENCE_PROVIDER_SOURCES
	IInferenceProvider.h
	llama/LlamaInferenceProvider.h
	llama/LlamaInferenceProvider.cpp
	onnxruntime/OrtInferenceProvider.h
	onnxruntime/OrtInferenceProvider.cpp
)

add_library(inference_provider STATIC ${INFERENCE_PROVIDER_SOURCES})

#if (ONNX_RUNTIME_SUPPORT)
if(1)
	if(NOT ONNXRUNTIME_ROOTDIR)
	  if(WIN32)
	    set(ONNXRUNTIME_ROOTDIR "C:/Program Files/onnxruntime")
	else()
	    set(ONNXRUNTIME_ROOTDIR "/usr/local")
	  endif()
	endif()
endif()

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
	${CMAKE_CURRENT_SOURCE_DIR}/../deps/ggml/lib/ggml-cuda.lib)

target_include_directories(inference_provider PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/../deps/whisper.cpp/include
	${CMAKE_CURRENT_SOURCE_DIR}/../deps/llama.cpp/include
	${CMAKE_CURRENT_SOURCE_DIR}/../deps/ggml/include)

# Make the core library's headers accessible to its users
target_include_directories(inference_provider PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# if (ONNX_RUNTIME_SUPPORT)
if(1)
	target_link_libraries(inference_provider PRIVATE "${ONNXRUNTIME_ROOTDIR}/lib")
	target_include_directories(inference_provider PRIVATE 
	"${ONNXRUNTIME_ROOTDIR}/include"                           # Pre-built package
    "${ONNXRUNTIME_ROOTDIR}/include/onnxruntime"               # Linux local install to /usr/local
    "${ONNXRUNTIME_ROOTDIR}/include/onnxruntime/core/session") # Windows local install
endif()

# To distinguish between core library and its users
target_compile_definitions(inference_provider PRIVATE INFERENCE_PROVIDER_BUILD)