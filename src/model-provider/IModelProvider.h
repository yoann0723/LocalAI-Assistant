#pragma once
#include <future>
#include <string>
#include <vector>
#include <span>
#include <string_view>
#include <expected.hpp>
#include "../core/core_c_api.h"
//#include "model_provider_export.h"

namespace ModelProvider {

	template<typename T>
	using Result = tl::expected<T, LocalAI_ErrorCode>;

	class ModelProviderBase {
	public:
		virtual ~ModelProviderBase() = default;

		virtual Result<void> initialize(std::string_view model_path, Model_Params params) { return {}; };
		virtual Result<void> updateParams(Model_Params params) { return {}; };
		virtual std::string name() { return ""; };
		virtual void unInitialize() {};
	};

	class ILLMProvider: public ModelProviderBase {
	public:
		virtual ~ILLMProvider() = default;
		virtual Result<std::string> generate(std::string_view prompt, size_t max_len) { 
			return std::string(); 
		};
	};

	class IEmbeddingProvider: public ModelProviderBase {
	public:
		virtual ~IEmbeddingProvider() = default;
		virtual Result<std::vector<float>> embedText(std::string_view text) {
			return std::vector<float>();
		};
	};

	struct ImageView {
		std::span<const uint8_t> data;
		int width;
		int height;
		int channels;
		std::string_view format;
	};

	class IVisionProvider: public ModelProviderBase {
	public:
		virtual ~IVisionProvider() = default;
		virtual Result<std::string> visionInfer(const ImageView& image) {
			return std::string();
		};
	};

	class IASRProvider: public ModelProviderBase {
	public:
		virtual ~IASRProvider() = default;
		virtual Result<std::string> transcribe(std::span<const float> samples) {
			return std::string();
		};
	};

    std::unique_ptr<ModelProviderBase> createLLMProvider();
    std::unique_ptr<ModelProviderBase> createEmbeddingProvider();
    std::unique_ptr<ModelProviderBase> createASRProvider();
    std::unique_ptr<ModelProviderBase> createVisionProvider();
}