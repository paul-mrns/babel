/*
** EPITECH PROJECT, 2026
** Untitled (Workspace)
** File description:
** AudioPort.hpp
*/


#ifndef AUDIOPORTSTREAM_HPP_
#define AUDIOPORTSTREAM_HPP_

#include "AAudioStream.hpp"
#include <mutex>
#include <deque>
extern "C" {
    #include <portaudio.h>
}

namespace babel {
    class AudioPortStream : public AAudioStream {
        
        public:
            AudioPortStream();
            ~AudioPortStream() override;
            void start() override;
            void stop() override;
            void write(const AudioBuffer& data) override;

        private:
            static int paCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
            const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData);
            
            PaStream* _stream = nullptr;
            
            std::vector<float> _inputBuffer;
            std::vector<float> _playbackBuffer;
            size_t _readIndex = 0;
            std::mutex _queueMutex;
            
            const int _framesPerBuffer = 480; // 10ms @ 48kHz

            PaStreamParameters setInputDevice();
            PaStreamParameters setOutputDevice();
    };
}

#endif