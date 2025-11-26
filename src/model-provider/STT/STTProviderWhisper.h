#pragma once
#include "IModelProvider.h"
#include <future>

class STTProviderWhisper : public ISTTProvider {
	public:
	virtual ~STTProviderWhisper() = default;
	// Asynchronous call returning future
	virtual std::future<std::string> speechToText(const AudioData& audioData) override {
		return std::async(std::launch::async, []() {
			// Placeholder implementation
			return std::string("Transcribed text from audio data.");
		});
	}
};