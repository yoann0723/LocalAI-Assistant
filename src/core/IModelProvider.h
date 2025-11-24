#pragma once
#include <future>
#include <string>
#include <vector>

struct ImageData {
    std::string format; // e.g., "png", "jpeg"
    std::vector<unsigned char> data; // raw image data
};

struct ImageInfo
{
    std::string description;
};

struct AudioData
{
	std::vector<float> samples;
	int sampleRate;
	int channels;
};

struct InferenceResult {
    bool ok = true;
    std::string text; // for LLM generate
    std::vector<float> embedding; // for embed
};

class ILLMProvider {
public:
    virtual ~ILLMProvider() = default;
    // Asynchronous calls returning futures
    virtual std::future<InferenceResult> generateLLM(const std::string& prompt) = 0;
};

class IEmbeddingProvider {
    public:
    virtual ~IEmbeddingProvider() = default;
    // Asynchronous call returning future
    virtual std::future<InferenceResult> embedText(const std::string& text) = 0;
};

class IVisionProvider {
    public:
    virtual ~IVisionProvider() = default;
    // Asynchronous call returning future
    virtual std::future<ImageInfo> visionInfer(const ImageData& img) = 0;
};

class ISTTProvider {
    public:
    virtual ~ISTTProvider() = default;
    // Asynchronous call returning future
    virtual std::future<std::string> speechToText(const AudioData& audioData) = 0;
};