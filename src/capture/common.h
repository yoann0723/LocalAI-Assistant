#pragma once

#define MAX_AUDIO_CAPTURE_BUFFER_MS 30 * 1000

using AudioCaptureCallbackFn = void (*)(const float* data, size_t size, void *context);