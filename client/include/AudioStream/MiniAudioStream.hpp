/*
** EPITECH PROJECT, 2026
** babel
** File description:
** MiniAudioStream.hpp
*/

#ifndef MINIAUDIO_STREAM_HPP_
#define MINIAUDIO_STREAM_HPP_

#include "miniaudio.h"
#include "AAudioStream.hpp"
#include <deque>
#include <stdexcept>
#include <mutex>
#include <iostream>

namespace babel {
    class MiniAudioStream : public AAudioStream {
        public:
            MiniAudioStream();
            ~MiniAudioStream() override;

            void start() override;
            void stop() override;
            void write(const AudioBuffer& data) override;

        private:
            static void dataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

            ma_device _device;
            bool _initialized = false;

            std::deque<float> _playbackQueue;
            std::mutex _queueMutex;

            std::vector<float> _inputBuffer;
            const size_t _maxQueueSize = 9600; // stereo 48kHz
    };
}
#endif