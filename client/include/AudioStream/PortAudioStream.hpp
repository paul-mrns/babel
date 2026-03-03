/*
** EPITECH PROJECT, 2026
** Untitled (Workspace)
** File description:
** AudioPort.hpp
*/

#pragma once
#include "IAudioStream.hpp"
#include <portaudio.h>
#include <mutex>
#include <deque>

namespace babel {
    class PortAudioStream : public IAudioStream {
    public:
        PortAudioStream();
        ~PortAudioStream() override;
        void start() override;
        void stop() override;
        void setOnReadCallback(OnRead callback) override;
        void write(const AudioBuffer& data) override;

        
        private:
        static int paCallback(const void *inputBuffer, void *outputBuffer,
            unsigned long framesPerBuffer,
            const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags statusFlags,
            void *userData);
            
            PaStream* _stream;
            OnRead _onRead;
            
            std::vector<float> _playbackBuffer;
            size_t _readIndex = 0;
            std::mutex _queueMutex;
            
            const int _sampleRate = DEFAULT_SAMPLE_RATE;
            const int _framesPerBuffer = 480; // 10ms @ 48kHz

            PaStreamParameters setInputDevice();
            PaStreamParameters setOutputDevice();
    };
}