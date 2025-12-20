#include "OrtInferenceProvider.h"
#if defined(_WIN32)
#include <windows.h>
#endif

#ifdef _WIN32
wchar_t* util_mbstowcs(const char* mbs) 
{
	int wlen = MultiByteToWideChar(CP_UTF8, 0, mbs, -1, NULL, 0);
	if (!wlen) {
		errno = EINVAL;
		return NULL;
	}

	wchar_t* wbuf = static_cast<wchar_t *>(malloc(wlen * sizeof(wchar_t)));
	wlen = MultiByteToWideChar(CP_UTF8, 0, mbs, -1, wbuf, wlen);
	if (!wlen) {
		free(wbuf);
		errno = EINVAL;
		return NULL;
	}

	return wbuf;
}
#endif

static Ort::Env& getOrtEnv() 
{
    static Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "LocalAI");
	return env;
}

OrtInferenceSession::~OrtInferenceSession()
{
}

Status OrtInferenceSession::initialize(const char* model_path, const Model_Params& params)
{
	Ort::SessionOptions options;
	wchar_t* wpath = util_mbstowcs(model_path);
	if (wpath) {
		session_ = std::make_shared<Ort::Session>(getOrtEnv(), wpath, options);
		free(wpath);
	}
	return {};
}

Status OrtInferenceSession::updateParams(const Model_Params& params)
{
	return {};
}

Status OrtInferenceSession::run(const InferenceRequest& request, Variant* result)
{
	Ort::RunOptions options;
	std::vector<Ort::Value> input_tensors;
	std::vector<const char*> input_names_char;
	std::vector<const char*> output_names_char;

	session_->Run(options, input_names_char.data(), input_tensors.data(),
		input_names_char.size(), output_names_char.data(), output_names_char.size());

	//TODO: fill the result struct

	return {};
}

Status OrtInferenceSession::runAsync(const InferenceRequest& request, InferenceCallback callback, void* user_data)
{
	Ort::RunOptions options;
	std::vector<Ort::Value> input_tensors;
	std::vector<Ort::Value> output_tensors;
	std::vector<const char*> input_names_char;
	std::vector<const char*> output_names_char;

	struct CallbackData {
		InferenceCallback callback;
        void* user_data;
	};

	CallbackData callback_data{ callback, user_data };

	session_->RunAsync(options, input_names_char.data(), input_tensors.data(),
		input_names_char.size(), output_names_char.data(), output_tensors.data(), output_names_char.size(),
		[](void* user_data, OrtValue** outputs, size_t num_outputs, OrtStatusPtr status) {

		}, nullptr);

	return {};
}
