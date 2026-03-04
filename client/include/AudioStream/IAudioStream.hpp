/*
** EPITECH PROJECT, 2026
** Untitled (Workspace)
** File description:
** IAudioStream.hpp
*/

#pragma once
#include <vector>
#include <functional>

#define DEFAULT_SAMPLE_RATE 48000
#define DEFAULT_CHANNELS 1              // 1 = mono, 2 = stereo

namespace babel {
    struct AudioBuffer {
        std::vector<float> samples;
        int sampleRate = DEFAULT_SAMPLE_RATE;
        int channels = DEFAULT_CHANNELS;
    };

    class IAudioStream {
        public:
            virtual ~IAudioStream() = default;

            using OnRead = std::function<void(const AudioBuffer&)>;

            virtual void start() = 0;
            virtual void stop() = 0;
            virtual void setOnReadCallback(OnRead callback) = 0;
            virtual void write(const AudioBuffer& data) = 0;
    };
}