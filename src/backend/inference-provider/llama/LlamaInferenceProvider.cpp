#include "LlamaInferenceProvider.h"
#include "../common/common.hpp"
#include <assert.h>

constexpr const int ngl = 99;
constexpr const int n_ctx = 4096;

LlamaInferenceProvider::LlamaInferenceProvider()
{
}

LlamaInferenceProvider::~LlamaInferenceProvider()
{
	for (auto& msg : messages_) {
		free(const_cast<char*>(msg.content));
	}
}

Status LlamaInferenceProvider::initialize(
	const char* model_path, 
	const Model_Params& params)
{
	if (!model_path)
	{
		fprintf(stderr, "%s: error: model_path is null.\n", __func__);
		return { LOCALAI_INVALID_ARG, "The model_path is null" };
	}

	// initialize the model
	llama_model_params model_params = llama_model_default_params();
	model_params.n_gpu_layers = params.n_gpu_layers;

	llama_model* model = llama_model_load_from_file(model_path, model_params);
	if (!model) {
		fprintf(stderr, "%s: error: unable to load model\n", __func__);
		return { LOCALAI_MODEL_LOAD_FAILED, "Unable to load model"};
	}

	const llama_vocab* vocab = llama_model_get_vocab(model);

	// initialize the context
	llama_context_params ctx_params = llama_context_default_params();
	ctx_params.n_ctx = n_ctx; // text context, 0 = from model
	ctx_params.n_batch = n_ctx;

	llama_context* ctx = llama_init_from_model(model, ctx_params);
	if (!ctx) {
		llama_model_free(model);
		fprintf(stderr, "%s: error: failed to create the llama_context\n", __func__);
		return { LOCALAI_NOT_INITIALIZED, "Failed to create the llama_context"};
	}

	// initialize the sampler
	llama_sampler* smpl = llama_sampler_chain_init(llama_sampler_chain_default_params());
	if (!smpl) {
		llama_model_free(model);
		llama_free(ctx);
		fprintf(stderr, "%s: error: failed to initialize llama_sampler.\n", __func__);
		return { LOCALAI_NOT_INITIALIZED, "Failed to initialize llama_sampler" };
	}

	llama_sampler_chain_add(smpl, llama_sampler_init_min_p(0.05f, 1));
	llama_sampler_chain_add(smpl, llama_sampler_init_temp(0.8f));
	llama_sampler_chain_add(smpl, llama_sampler_init_dist(LLAMA_DEFAULT_SEED));

	formatted_.resize(llama_n_ctx(ctx));

	model_.reset(model);
	ctx_.reset(ctx);
	smpl_.reset(smpl);

	return {};
}

Status LlamaInferenceProvider::updateParams(const Model_Params& params)
{
	return {};
}

Status LlamaInferenceProvider::generate(std::string_view user, LLMOutput** output)
{
	if (!output)
		return { LOCALAI_INVALID_ARG, "The response pointer is null" };

	force_stopped_ = false;

	const char* tmpl = llama_model_chat_template(model_.get(), /* name */ nullptr);

	std::string_view prompt;
	if (use_history_messages_) {
		// add the user input to the message list and format it
		messages_.push_back({ "user", strdup(user.data()) });
		int new_len = llama_chat_apply_template(tmpl, messages_.data(), messages_.size(), true, formatted_.data(), formatted_.size());
		if (new_len > (int)formatted_.size()) {
			formatted_.resize(new_len);
			new_len = llama_chat_apply_template(tmpl, messages_.data(), messages_.size(), true, formatted_.data(), formatted_.size());
		}
		if (new_len < 0) {
			fprintf(stderr, "failed to apply the chat template\n");
			return { LOCALAI_MODEL_GENERATE_ERROR, "failed to apply the chat template"};
		}

		// remove previous messages to obtain the prompt to generate the response
		prompt = std::string_view(formatted_.begin() + prev_len, formatted_.begin() + new_len);
	}
	else {
		prompt = user;
	}

	auto response = std::make_unique<LLMOutput>();

	const llama_vocab* vocab = llama_model_get_vocab(model_.get());
	const bool is_first = llama_memory_seq_pos_max(llama_get_memory(ctx_.get()), 0) == -1;

	// tokenize the prompt
	const int n_prompt_tokens = -llama_tokenize(vocab, prompt.data(), prompt.size(), NULL, 0, is_first, true);
	std::vector<llama_token> prompt_tokens(n_prompt_tokens);
	if (llama_tokenize(vocab, prompt.data(), prompt.size(), prompt_tokens.data(), prompt_tokens.size(), is_first, true) < 0) {
		fprintf(stderr, "failed to tokenize the prompt\n");
		return {
			LOCALAI_MODEL_TOKENIZE_ERROR,
			"llama_tokenize: failed to tokenize the prompt"
		};
	}

	// prepare a batch for the prompt
	llama_batch batch = llama_batch_get_one(prompt_tokens.data(), prompt_tokens.size());
	llama_token new_token_id;

	// using string internal cache, 15 bytes + '\n'
	response->text.resize(response->text.capacity());

	while (true) {
		// check if we have enough space in the context to evaluate this batch
		int n_ctx = llama_n_ctx(ctx_.get());
		int n_ctx_used = llama_memory_seq_pos_max(llama_get_memory(ctx_.get()), 0) + 1;
		if (n_ctx_used + batch.n_tokens > n_ctx) {
			assert(false && "context size exceeded, the app is about to exit.");
			exit(0);
		}

		int ret = llama_decode(ctx_.get(), batch);
		if (ret != 0) {
			fprintf(stderr, "failed to decode, ret = %d\n", ret);
			return { 
				LOCALAI_MODEL_DECODE_ERROR, 
				"llama_decode: Failed to decode, error: " + std::to_string(ret) 
			};
		}

		// sample the next token
		new_token_id = llama_sampler_sample(smpl_.get(), ctx_.get(), -1);

		// is it an end of generation?
		if (llama_vocab_is_eog(vocab, new_token_id) || force_stopped_) {
			break;
		}

		// convert the token to a string, print it and add it to the response
		int n = llama_token_to_piece(vocab, new_token_id, 
			response->text.data() + response->piece_count, 
			response->text.size() - response->piece_count, 0, true);
		if (n < 0) {
			response->text.resize(response->piece_count + (-n));
			int check = llama_token_to_piece(vocab, new_token_id,
				response->text.data() + response->piece_count,
				response->text.size() - response->piece_count, 0, true);
			GGML_ASSERT(check == -n);
			if (check != -n) {
				fprintf(stderr, "token_to_piece: inconsistent result\n");
				return {
					LOCALAI_MODEL_GENERATE_ERROR,
					"llama_token_to_piece: inconsistent result"
				};
			}
			n = check;
		}
		else {
			response->text.resize(response->piece_count + n);
		}

		response->piece_count += n;

		// prepare the next batch with the sampled token
		batch = llama_batch_get_one(&new_token_id, 1);
#ifdef _DEBUG
		fprintf(stderr, "%s", response->text.c_str());
		fflush(stderr);
#endif
	}

	response->text.resize(response->piece_count + 1);
	response->text[response->piece_count] = '\0';
	response->text.shrink_to_fit();

	if (use_history_messages_) {
		// add the response to the messages
		messages_.push_back({ "assistant", strdup(response->text.c_str()) });
		prev_len = llama_chat_apply_template(tmpl, messages_.data(), messages_.size(), false, nullptr, 0);
		if (prev_len < 0) {
			fprintf(stderr, "failed to apply the chat template\n");
			return { LOCALAI_MODEL_GENERATE_ERROR, "failed to apply the chat template" };
		}
	}

	*output = response.release();
	return {};
}

void LlamaInferenceProvider::stopGenerate()
{
	force_stopped_ = true;
}
