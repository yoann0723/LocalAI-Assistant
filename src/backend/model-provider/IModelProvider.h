#pragma once
#include <future>
#include <string>
#include <vector>
#include <span>
#include <optional>
#include <string_view>
#include <expected.hpp>
#include <memory>
#include "localai_c_api.h"
#include "../common/common.hpp"
//#include "../inference-provider/IInferenceProvider.h"
//#include "model_provider_export.h"

enum class ModelCapability {
	TextGeneration,
	Embedding,
	Vision,
	ASR,
	TextToSpeech,
	TextToImage,
	Custom
};

enum class BuildInModelType {
	LLM,
	Embedding,
	Vision,
	ASR,
	TTS,
	SD
};

template<typename T>
using Result = tl::expected<T, LocalAI_ErrorCode>;

using LLMCallback = std::function<void(const LLMOutput* result, const Status& status)>;

class IModelProvider {
public:
	virtual ~IModelProvider() = default;

	virtual const char * name() const = 0;

	virtual ModelCapability capability() const = 0;

	virtual Status initialize(
		const char* model_path, 
		const Model_Params &params) = 0;

	virtual Status updateParams(
		const Model_Params &params) = 0;

	virtual void unInitialize() = 0;
};

class IASRProvider : public IModelProvider {
public:
	virtual bool vadSample(std::vector<float> &samples, int sample_rate, int last_ms) = 0;
	virtual Status transcribe(std::span<const float> samples, std::string& out) = 0;
};

class ILLMProvider : public IModelProvider {
public:
	virtual void generateAsync(
		std::string_view prompt, std::optional<std::string_view> params,
		size_t max_len,
		LLMCallback callback) const = 0;
};

class ModelProviderFactory {
public:
	static std::unique_ptr<IModelProvider> createBuildInModel(
		BuildInModelType type, size_t n_thread = 0);

	static std::unique_ptr<IASRProvider> createASRModel();

	static std::unique_ptr<ILLMProvider> createLLModel();

	static std::unique_ptr<IModelProvider> createCustomModel(
		InferenceType infernce_type, size_t n_thread = 0);
};

class BackendInitializer {
public:
	static void InitializeBackend();
};
