/*
** EPITECH PROJECT, 2026
** Untitled (Workspace)
** File description:
** AudioPortStream.cpp
*/

#include "../../include/AudioStream/AudioPortStream.hpp"
#include <stdexcept>
#include <algorithm>

namespace babel {

AudioPortStream::AudioPortStream()
    : AAudioStream(DEFAULT_SAMPLE_RATE, DEFAULT_CHANNELS), _stream(nullptr)
{
    if (Pa_Initialize() != paNoError)
        throw std::runtime_error("PortAudio: Failed to initialize");
    PaStreamParameters inputParams = setInputDevice();
    PaStreamParameters outputParams = setOutputDevice();
    PaError err = Pa_OpenStream(&_stream, &inputParams, &outputParams, _sampleRate, _framesPerBuffer, paClipOff, paCallback, this);
    if (err != paNoError)
        throw std::runtime_error("PortAudio: Failed to open stream");
}

AudioPortStream::~AudioPortStream()
{
    if (_stream) {
        Pa_StopStream(_stream);
        Pa_CloseStream(_stream);
    }
    Pa_Terminate();
}

void AudioPortStream::start()
{
    Pa_StartStream(_stream);
}

void AudioPortStream::stop()
{
    Pa_StopStream(_stream);
}

PaStreamParameters AudioPortStream::setInputDevice()
{
    PaStreamParameters params;
    params.device = Pa_GetDefaultInputDevice();
    if (params.device == paNoDevice)
        throw std::runtime_error("PortAudio: No input device found");
    params.channelCount = DEFAULT_CHANNELS;
    params.sampleFormat = paFloat32;
    params.suggestedLatency = Pa_GetDeviceInfo(params.device)->defaultLowInputLatency;
    params.hostApiSpecificStreamInfo = nullptr;
    return params;
}

PaStreamParameters AudioPortStream::setOutputDevice()
{
    PaStreamParameters params;
    params.device = Pa_GetDefaultOutputDevice();
    if (params.device == paNoDevice)
        throw std::runtime_error("PortAudio: No Output device found");
    params.channelCount = DEFAULT_CHANNELS;
    params.sampleFormat = paFloat32;
    params.suggestedLatency = Pa_GetDeviceInfo(params.device)->defaultLowOutputLatency;
    params.hostApiSpecificStreamInfo = nullptr;
    return params;
}

void AudioPortStream::write(const AudioBuffer& data)
{
    std::lock_guard<std::mutex> lock(_queueMutex);

    if (_readIndex >= _playbackBuffer.size()) {
        _playbackBuffer.clear();
        _readIndex = 0;
    }
    for (float sample : data.samples)
        _playbackBuffer.push_back(std::clamp(sample, -1.0f, 1.0f));
}

int AudioPortStream::paCallback(const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void *userData)
{
    auto *self = static_cast<AudioPortStream*>(userData);
    const float *in = static_cast<const float*>(input);
    float *out = static_cast<float*>(output);

    // IN
    if (in) {
        self->_inputBuffer.insert(self->_inputBuffer.end(), in, in + (frameCount * DEFAULT_CHANNELS));

        while (self->_inputBuffer.size() >= (480 * DEFAULT_CHANNELS)) {
            AudioBuffer buf;
            buf.sampleRate = DEFAULT_SAMPLE_RATE;
            buf.channels = DEFAULT_CHANNELS;
            
            auto start = self->_inputBuffer.begin();
            auto end = start + (480 * DEFAULT_CHANNELS);
            buf.samples.assign(start, end);
            self->_inputBuffer.erase(start, end);
            
            if (self->_onReadCallback) self->_onReadCallback(buf);
        }
    }

    // OUT
    if (out) {
        std::lock_guard<std::mutex> lock(self->_queueMutex);
        for (unsigned long i = 0; i < frameCount * DEFAULT_CHANNELS; ++i) {
            if (self->_readIndex < self->_playbackBuffer.size()) {
                out[i] = self->_playbackBuffer[self->_readIndex++];
            } else {
                out[i] = 0.0f;
            }
        }
    }
    return paContinue;
}
} //namespace babel