/*
** EPITECH PROJECT, 2026
** Untitled (Workspace)
** File description:
** ACodec.hpp
*/

#ifndef ACODEC_HPP_
#define ACODEC_HPP_

#include "ICodec.hpp"

namespace babel {
    class ACodec : public ICodec {

        protected:
            int _sampleRate;
            int _channels;
            int _bitrate;

        public:
            ACodec(int sampleRate = DEFAULT_SAMPLE_RATE, int channels = DEFAULT_CHANNELS, int bitrate = 24000)
                : _sampleRate(sampleRate), _channels(channels), _bitrate(bitrate) {}
    
            int getSampleRate() const { return _sampleRate; }
        };
}

#endif