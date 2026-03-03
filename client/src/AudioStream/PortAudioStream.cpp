/*
** EPITECH PROJECT, 2026
** Untitled (Workspace)
** File description:
** PortAudioStream.cpp
*/

#include "../../include/AudioStream/PortAudioStream.hpp"
#include <stdexcept>
#include <a.out.h>
#include <iostream>

namespace babel {

PortAudioStream::PortAudioStream()
    : _stream(nullptr)
{
    if (Pa_Initialize() != paNoError)
        throw std::runtime_error("PortAudio: Failed to initialize");
    PaStreamParameters inputParams = setInputDevice();
    PaStreamParameters outputParams = setOutputDevice();
    PaError err = Pa_OpenStream(&_stream, &inputParams, &outputParams, _sampleRate, _framesPerBuffer, paClipOff, paCallback, this);
    if (err != paNoError)
        throw std::runtime_error("PortAudio: Failed to open stream");
}

PortAudioStream::~PortAudioStream()
{
    if (_stream) {
        Pa_StopStream(_stream);
        Pa_CloseStream(_stream);
    }
    Pa_Terminate();
}

void PortAudioStream::start()
{
    Pa_StartStream(_stream);
}

void PortAudioStream::stop()
{
    Pa_StopStream(_stream);
}

PaStreamParameters PortAudioStream::setInputDevice()
{
    PaStreamParameters params;
    params.device = Pa_GetDefaultInputDevice();
    std::cout << "Selected input device: " << Pa_GetDeviceInfo(params.device)->name << std::endl;
    if (params.device == paNoDevice)
        throw std::runtime_error("PortAudio: No input device found");
    params.channelCount = DEFAULT_CHANNELS;
    params.sampleFormat = paFloat32;
    params.suggestedLatency = Pa_GetDeviceInfo(params.device)->defaultLowInputLatency;
    params.hostApiSpecificStreamInfo = nullptr;
    return params;
}

PaStreamParameters PortAudioStream::setOutputDevice()
{
    PaStreamParameters params;
    params.device = Pa_GetDefaultOutputDevice();
    std::cout << "Selected output device: " << Pa_GetDeviceInfo(params.device)->name << std::endl;
    if (params.device == paNoDevice)
        throw std::runtime_error("PortAudio: No Output device found");
    params.channelCount = DEFAULT_CHANNELS;
    params.sampleFormat = paFloat32;
    params.suggestedLatency = Pa_GetDeviceInfo(params.device)->defaultLowOutputLatency;
    params.hostApiSpecificStreamInfo = nullptr;
    return params;
}

void PortAudioStream::setOnReadCallback(OnRead callback)
{
    _onRead = callback;
}

void PortAudioStream::write(const AudioBuffer& data)
{
    std::lock_guard<std::mutex> lock(_queueMutex);

    if (_readIndex >= _playbackBuffer.size()) {
        _playbackBuffer.clear();
        _readIndex = 0;
    }
    float gain = 0.5f;
    for (float sample : data.samples) {
        _playbackBuffer.push_back(sample * gain);
    }
}

int PortAudioStream::paCallback(const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void *userData)
{
    auto *self = static_cast<PortAudioStream*>(userData);
    const float *in  = static_cast<const float*>(input);
    float *out = static_cast<float*>(output);
    const unsigned long samples = frameCount * DEFAULT_CHANNELS;

    if (!in || !out) {
        std::cerr << "PortAudio: Invalid buffer\n";
        return paContinue;
    }

    // IN
    if (self->_onRead) {
        AudioBuffer buf;
        buf.sampleRate = DEFAULT_SAMPLE_RATE;
        buf.channels = DEFAULT_CHANNELS;
        buf.samples.assign(in, in + samples);
        self->_onRead(buf);
    }

    // OUT
    std::unique_lock<std::mutex> lock(self->_queueMutex, std::try_to_lock);
    if (!lock.owns_lock()) {
        std::fill(out, out + samples, 0.0f);
        return paContinue;
    }
    for (unsigned long i = 0; i < samples; ++i) {
        out[i] = (self->_readIndex < self->_playbackBuffer.size())
            ? self->_playbackBuffer[self->_readIndex++]
            : 0.0f;
    }
    if (self->_readIndex >= self->_playbackBuffer.size()) {
        self->_playbackBuffer.clear();
        self->_readIndex = 0;
    }
    return paContinue;
}

} //namespace babel