#include "ASREngine.h"
#include <algorithm>

constexpr const int vad_sample_ms_default = 2000;
constexpr const int voice_sample_ms_default = 10000;

ASREngine::ASREngine(IASRProvider* asr_provider,
	LocalAI_AudioProviderInfo* provider)
	:asr_provider_(asr_provider), audio_provider_(*provider)
{
	loop_ = std::thread(&ASREngine::asrLoop, this);
}

ASREngine::~ASREngine()
{
	stop();
	if (loop_.joinable()) {
		loop_.join();
	}
}

void ASREngine::resume() 
{
	status_ = ASR_RUNNING;
}

void ASREngine::pause()
{
	status_ = ASR_PAUSED;
}

void ASREngine::stop()
{
	status_ = ASR_STOPPED;
}

void ASREngine::asrLoop()
{
	std::string heard;

	if (!audio_provider_.fill_buffer) {
		fprintf(stderr, "No fill buffer function specified for audio provider.\n");
		return;
	}

	fprintf(stderr, "thread: `%s` started.\n", __FUNCTION__);

	if (audio_provider_.on_status_changed)
		audio_provider_.on_status_changed(ASR_RUNNING, audio_provider_.user_data);

	while (status_ != ASR_STOPPED)
	{
		if (status_ == ASR_PAUSED) {
			if (audio_provider_.on_status_changed)
				audio_provider_.on_status_changed(ASR_PAUSED, audio_provider_.user_data);
		}

		std::unique_lock<std::mutex> lock(mutex_);
		cv_.wait(lock, [this] { return (status_ == ASR_STOPPED || status_ == ASR_RUNNING); });
		
		if (status_ == ASR_STOPPED) {
			fprintf(stderr, "User stopped ASR.\n");
			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		const size_t vad_sample_size = (audio_provider_.sample_rate * vad_sample_ms_default) / 1000;
		data_.clear();
		data_.resize(vad_sample_size);

		auto available_size = audio_provider_.fill_buffer(data_.data(), data_.size(), vad_sample_ms_default, audio_provider_.user_data);
		if (0 == available_size) {
#ifdef _DEBUG
			fprintf(stderr, "Failed to fill buffer.\n");
#endif
			if (audio_provider_.on_error)
				audio_provider_.on_error(LOCALAI_RUNTIME_ERROR,
					"Failed to fill audio buffer.", audio_provider_.user_data);
			continue;
		}

		if(vad_sample_size > available_size)
			data_.resize(available_size);
		// detect audio activity
		if (!asr_provider_->vadSample(data_, audio_provider_.sample_rate, 1250)) {
			continue;
		}

		// inference
		int voice_sample_size = (audio_provider_.sample_rate * voice_sample_ms_default) / 1000;
		voice_sample_size = std::min(static_cast<int>(voice_sample_size), audio_provider_.circle_buffer_size);
		data_.clear();
		data_.resize(voice_sample_size);

		available_size = audio_provider_.fill_buffer(data_.data(), data_.size(),
			voice_sample_ms_default, audio_provider_.user_data);

		if (0 == available_size) {
			if (audio_provider_.on_error)
				audio_provider_.on_error(LOCALAI_RUNTIME_ERROR,
					"Failed to fill audio buffer.", audio_provider_.user_data);
			continue;
		}

		if (voice_sample_size > available_size)
			data_.resize(available_size);

		auto status = asr_provider_->transcribe(data_, heard);
		if (status) {

			if (heard.empty()) {
				audio_provider_.clear_audio(audio_provider_.user_data);
				continue;
			}

			if (audio_provider_.on_heard)
				audio_provider_.on_heard(heard.c_str(), heard.size(), audio_provider_.user_data);
		}else {
			fprintf(stderr, "Failed to transcribe audio. %s\n", status.str().c_str());

			if (audio_provider_.on_error)
				audio_provider_.on_error(status.code, status.error.c_str(), audio_provider_.user_data);

			pause();

			if (audio_provider_.on_status_changed)
				audio_provider_.on_status_changed(ASR_PAUSED, audio_provider_.user_data);
		}

		audio_provider_.clear_audio(audio_provider_.user_data);
	}

	if (audio_provider_.on_status_changed)
		audio_provider_.on_status_changed(ASR_STOPPED, audio_provider_.user_data);

	fprintf(stderr, "thread: `%s` stopped.\n", __FUNCTION__);
}
