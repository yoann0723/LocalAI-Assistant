#pragma once
#include <memory>
#include <vector>

#include "AIModelHub.h"
#include "../include/localai_c_api.h"

class ASREngine {
public:
	ASREngine(IASRProvider* asr_provider, 
		LocalAI_AudioProviderInfo* provider);
	~ASREngine();

	void resume();
	void pause();
	void stop();

private:
	void asrLoop();

	IASRProvider* asr_provider_ = nullptr;
	std::atomic<ASRStatus> status_{ ASR_RUNNING };
	LocalAI_AudioProviderInfo audio_provider_{0};
	std::thread loop_;
	std::mutex mutex_;
	std::condition_variable cv_;
	std::vector<float> data_;
};
