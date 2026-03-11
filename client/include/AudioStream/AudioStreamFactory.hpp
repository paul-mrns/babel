/*
** EPITECH PROJECT, 2026
** babel
** File description:
** AudioStreamFactory.hpp
*/

#ifndef AUDIO_STREAM_FACTORY_HPP_
#define AUDIO_STREAM_FACTORY_HPP_

#include <memory>
#include "AudioPortStream.hpp"
#include "MiniAudioStream.hpp"

namespace babel {

    enum class AudioStreamSystem {
        AUDIOPORT,
        MINIAUDIO
    };

    class AudioStreamFactory {
        public:
            static std::unique_ptr<IAudioStream> create(AudioStreamSystem type) {
                if (type == AudioStreamSystem::AUDIOPORT) {
                    return std::make_unique<AudioPortStream>();
                }
                if (type == AudioStreamSystem::MINIAUDIO) {
                    return std::make_unique<MiniAudioStream>();
                }
                return nullptr;
            }
    };
}

#endif