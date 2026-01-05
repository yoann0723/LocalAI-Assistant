#pragma once
#include "IModelProvider.h"

#include <functional>
#include <optional>

class ILLMInferProvider;
class ThreadPool;

class LLMProviderImpl final : public ILLMProvider {
public:
	explicit LLMProviderImpl(size_t n_thread = 1);

	virtual ~LLMProviderImpl();

	// Inherited via IModelProvider
	const char* name() const override;

	ModelCapability capability() const override;

	Status initialize(
		const char* model_path, 
		const Model_Params& params) override;

	Status updateParams(
		const Model_Params& params) override;

	void unInitialize() override;

	/*void generateAsync(
		std::string_view prompt, std::optional<std::string_view> params,
		size_t max_len, 
		LocalAI_TextCallback callback, void *user_data) const;*/

	void generateAsync(
		std::string_view prompt, std::optional<std::string_view> params,
		size_t max_len,
		LLMCallback callback) const override;

private:
	std::unique_ptr<ILLMInferProvider> engine_;
	mutable std::unique_ptr<ThreadPool> pool_;
};