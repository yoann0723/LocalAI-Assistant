#include "IAudioCapture.h"
#include "common.h"
#include <assert.h>
#include <SDL.h>
#include <atomic>
#include <mutex>
#include <vector>

constexpr const int default_channels = 1;
constexpr const int default_samples = 1024;

class AudioCaptureSDL :public Capture::audio::IAudioCapture {
public:
	AudioCaptureSDL();
	~AudioCaptureSDL() override;
	virtual bool initialize(int sampleRate, int bufferLenMS, int& outBufSize, AudioCaptureCallbackFn callback) override;
	virtual bool resume() override;
	virtual bool pause() override;
	virtual void stop() override;
	virtual bool isCapturing() const;
	virtual void clearBuffer() override;
	virtual size_t getAudioData(float* buffer, int buffer_size, int ms) override;
	virtual Capture::audio::AudioInfo getAudioInfo() override;

	void audioCaptureCallback(Uint8* stream, int len);

private:
	AudioCaptureCallbackFn audioCallback_ = nullptr;
	SDL_AudioDeviceID deviceId_ = 0;
	std::mutex mutex_;
	std::vector<float> audioBuffer_;
	std::atomic_bool capturing_ = false;
	int sampleRate_ = 16000;
	size_t audioLength_ = 0; // avialable audio length in samples
	size_t audioPos_ = 0; // the next write position in samples
	int bufferLenMS_ = MAX_AUDIO_CAPTURE_BUFFER_MS;
};

AudioCaptureSDL::AudioCaptureSDL() 
{
	SDL_Init(SDL_INIT_AUDIO);
}

AudioCaptureSDL::~AudioCaptureSDL() {
	stop();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

bool AudioCaptureSDL::initialize(int sampleRate, int bufferLenMS, int& outBufSize, AudioCaptureCallbackFn callback)
{
	SDL_AudioSpec want;
	SDL_AudioSpec have;

	SDL_zero(want);
	SDL_zero(have);

	want.freq = sampleRate;
	want.channels = default_channels;
	want.format = AUDIO_F32;
	want.samples = default_samples;
	want.callback = [](void* userdata, uint8_t* stream, int len) {
		AudioCaptureSDL* audio = (AudioCaptureSDL*)userdata;
		audio->audioCaptureCallback(stream, len);
		};
	want.userdata = this;

	deviceId_ = SDL_OpenAudioDevice(nullptr, SDL_TRUE, &want, &have, 0);
	if (!deviceId_) {
		printf("Failed to open audio device: %s\n", SDL_GetError());
		deviceId_ = 0;
		return false;
	}

	audioCallback_ = callback;
	bufferLenMS_ = bufferLenMS > 0 ? bufferLenMS : MAX_AUDIO_CAPTURE_BUFFER_MS;

	sampleRate_ = have.freq;
	audioBuffer_.resize((sampleRate_ * bufferLenMS_) / 1000);

	outBufSize = audioBuffer_.size();

	return true;
}

bool AudioCaptureSDL::resume() {
	
	if (!deviceId_) {
		printf("Audio device not initialized.\n");
		return false;
	}

	if (capturing_) {
		printf("Audio capture already started.\n");
		return true;
	}

	SDL_PauseAudioDevice(deviceId_, 0);
	capturing_ = true;

	return true;
}

bool AudioCaptureSDL::pause()
{
	if (!deviceId_) {
		fprintf(stderr, "%s: no audio device to pause!\n", __func__);
		return false;
	}

	if (!capturing_) {
		fprintf(stderr, "%s: already paused!\n", __func__);
		return false;
	}

	SDL_PauseAudioDevice(deviceId_, 1);

	capturing_ = false;

	return true;
}

void AudioCaptureSDL::stop()
{
	if (deviceId_) {
		SDL_CloseAudioDevice(deviceId_);
		deviceId_ = 0;
	}
	capturing_ = false;
}

bool AudioCaptureSDL::isCapturing() const 
{
	return capturing_;
}

void AudioCaptureSDL::clearBuffer()
{
	if (!deviceId_) {
		fprintf(stderr, "%s: no audio device to clear buffer!\n", __func__);
		return;
	}

	if (!capturing_) {
		fprintf(stderr, "%s: not capturing, cannot clear buffer!\n", __func__);
		return;
	}

	{
		std::lock_guard<std::mutex> lock(mutex_);
		audioLength_ = 0;
		audioPos_ = 0;
	}
}

size_t AudioCaptureSDL::getAudioData(float* buffer, int buffer_size, int ms)
{
	std::lock_guard<std::mutex> lock(mutex_);
	if (ms <= 0)
		ms = bufferLenMS_;

	size_t nSample = (sampleRate_ * ms) / 1000;
	if(nSample > audioLength_)
		nSample = audioLength_; // cannot get more than available

	int s0 = audioPos_ - nSample;
	if (s0 < 0) {
		s0 += audioBuffer_.size();
	}

	if (nSample > buffer_size) {
		assert(false);
		fprintf(stderr, "buffer capacity is not sufficient.\n");
		return 0;
	}

	//out = std::shared_ptr<float>(new float[nSample], std::default_delete<float[]>());
	if (s0 + nSample > audioBuffer_.size()) {
		// read in two parts
		size_t n1 = audioBuffer_.size() - s0;
		size_t n2 = nSample - n1;
		memcpy(buffer, &audioBuffer_[s0], n1 * sizeof(float));
		memcpy(&(buffer[n1]), &audioBuffer_[0], n2 * sizeof(float));
	}
	else {
		memcpy(buffer, &audioBuffer_[s0], nSample * sizeof(float));
	}

	return nSample;
}

Capture::audio::AudioInfo AudioCaptureSDL::getAudioInfo()
{
	Capture::audio::AudioInfo info{};
	info.sample_rate = sampleRate_;
	info.channels = default_channels;
	return info;
}

void AudioCaptureSDL::audioCaptureCallback(Uint8* stream, int len)
{
	if (!capturing_)
		return;

	size_t nSamples = len / sizeof(float);
	if (nSamples > audioBuffer_.size()) {
		nSamples = audioBuffer_.size();
		stream += (len - nSamples * sizeof(float)); // keep only the last part
	}
	
	{
		std::lock_guard<std::mutex> lock(mutex_);

		if (nSamples + audioPos_ > audioBuffer_.size()) {
			// write in two parts
			size_t n1 = audioBuffer_.size() - audioPos_;
			size_t n2 = nSamples - n1;
			memcpy(&audioBuffer_[audioPos_], stream, n1 * sizeof(float));
			memcpy(&audioBuffer_[0], stream + n1 * sizeof(float), n2 * sizeof(float));
		}
		else {
			memcpy(&audioBuffer_[audioPos_], stream, nSamples * sizeof(float));
		}

		// update position and length
		audioPos_ = (audioPos_ + nSamples) % audioBuffer_.size();
		audioLength_ = std::min(audioLength_ + nSamples, audioBuffer_.size());
	}
}

namespace Capture::audio {
	std::unique_ptr<IAudioCapture> createAudioCapture() {
		return std::make_unique<AudioCaptureSDL>();
	}
}
