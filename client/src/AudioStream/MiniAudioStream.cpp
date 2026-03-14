/*
** EPITECH PROJECT, 2026
** Untitled (Workspace)
** File description:
** AudioPortStream.cpp
*/

#define MINIAUDIO_IMPLEMENTATION
#include "../../include/AudioStream/MiniAudioStream.hpp"
#include <algorithm>

namespace babel {

MiniAudioStream::MiniAudioStream()
    : AAudioStream(DEFAULT_SAMPLE_RATE, DEFAULT_CHANNELS)
{
    ma_device_config config = ma_device_config_init(ma_device_type_duplex);
    config.capture.format = ma_format_f32;
    config.capture.channels = _channels;
    config.playback.format = ma_format_f32;
    config.playback.channels = _channels;
    config.sampleRate = _sampleRate;
    config.dataCallback = dataCallback;
    config.pUserData = this;
    config.periodSizeInFrames = 480;
    config.noClip = MA_TRUE;

    if (ma_device_init(NULL, &config, &_device) != MA_SUCCESS) {
        throw std::runtime_error("Miniaudio: Device init failed");
    }
    _initialized = true;
}

MiniAudioStream::~MiniAudioStream()
{
    if (ma_device_is_started(&_device)) {
        ma_device_stop(&_device);
    }
    if (_initialized) {
        ma_device_uninit(&_device);
        _initialized = false;
    }
    std::lock_guard<std::mutex> lock(_queueMutex);
    _playbackQueue.clear();
}

void MiniAudioStream::start()
{
    if (!_initialized)
        return;
    if (ma_device_start(&_device) == MA_SUCCESS) {
        _running = true;
    } else {
        throw std::runtime_error("Miniaudio: Failed to start device");
    }
}

void MiniAudioStream::stop()
{
    if (!_initialized)
        return;

    if (ma_device_stop(&_device) == MA_SUCCESS) {
        _running = false;
        std::lock_guard<std::mutex> lock(_queueMutex);
        _playbackQueue.clear();
    }
}

void MiniAudioStream::write(const AudioBuffer& data)
{
    std::lock_guard<std::mutex> lock(_queueMutex);

    if (_playbackQueue.size() + data.samples.size() > _maxQueueSize) {
        size_t toRemove = _playbackQueue.size() / 2;
        _playbackQueue.erase(_playbackQueue.begin(), _playbackQueue.begin() + toRemove);
    }

    for (float sample : data.samples)
        _playbackQueue.push_back(std::clamp(sample, -1.0f, 1.0f));
}

void MiniAudioStream::dataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    auto* self = static_cast<MiniAudioStream*>(pDevice->pUserData);

    // IN
    if (pInput != nullptr) {
        const float* inputF32 = static_cast<const float*>(pInput);
        self->_inputBuffer.insert(self->_inputBuffer.end(), inputF32, inputF32 + (frameCount * self->_channels));
        while (self->_inputBuffer.size() >= static_cast<size_t>(480 * self->_channels)) {
            AudioBuffer buf;
            buf.sampleRate = self->_sampleRate;
            buf.channels = self->_channels;

            auto start = self->_inputBuffer.begin();
            auto end = start + (480 * self->_channels);
            buf.samples.assign(start, end);
            self->_inputBuffer.erase(start, end);
            float gain = 2.0f; 
            for (float &sample : buf.samples) {
                sample *= gain;
                if (sample > 1.0f) sample = 1.0f;
                if (sample < -1.0f) sample = -1.0f;
            }
            self->notifyRead(buf);
        }
    }

    // OUT
    if (pOutput != nullptr) {
        float* outputF32 = static_cast<float*>(pOutput);
        std::lock_guard<std::mutex> lock(self->_queueMutex);
        for (ma_uint32 i = 0; i < frameCount * self->_channels; ++i) {
            if (!self->_playbackQueue.empty()) {
                outputF32[i] = self->_playbackQueue.front();
                self->_playbackQueue.pop_front();
            } else {
                outputF32[i] = 0.0f;
            }
        }
    }
}

}