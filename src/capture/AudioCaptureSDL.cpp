#include "common.h"
#include "IAudioCapture.h"
#include <SDL.h>
#include <atomic>
#include <mutex>
#include <vector>

class AudioCaptureSDL :public Capture::audio::IAudioCapture {
public:
	AudioCaptureSDL();
	~AudioCaptureSDL() override;
	virtual bool initialize(int sampleRate, int bufferLenMS, AudioCaptureCallbackFn callback) override;
	virtual bool resume() override;
	virtual bool pause() override;
	virtual void stop() override;
	virtual bool isCapturing() const;
	virtual void clearBuffer() override;
	virtual size_t getAudioData(int ms, std::shared_ptr<float> &out) override;

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

bool AudioCaptureSDL::initialize(int sampleRate, int bufferLenMS, AudioCaptureCallbackFn callback)
{
	sampleRate_ = sampleRate;
	audioCallback_ = callback;
	if(bufferLenMS > 0)
		bufferLenMS_ = bufferLenMS;

	SDL_AudioSpec want;
	SDL_AudioSpec have;

	SDL_zero(want);
	SDL_zero(have);

	want.freq = sampleRate;
	want.channels = 1;
	want.format = AUDIO_F32;
	want.samples = 1024;
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

size_t AudioCaptureSDL::getAudioData(int ms, std::shared_ptr<float>& out)
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

	out = std::shared_ptr<float>(new float[nSample], std::default_delete<float[]>());
	if (s0 + nSample > audioBuffer_.size()) {
		// read in two parts
		size_t n1 = audioBuffer_.size() - s0;
		size_t n2 = nSample - n1;
		memcpy(out.get(), &audioBuffer_[s0], n1 * sizeof(float));
		memcpy(&out.get()[n1], &audioBuffer_[0], n2 * sizeof(float));
	}
	else {
		memcpy(out.get(), & audioBuffer_[s0], nSample * sizeof(float));
	}

	return nSample;
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
