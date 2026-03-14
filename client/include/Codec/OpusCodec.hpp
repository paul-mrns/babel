/*
** EPITECH PROJECT, 2026
** Untitled (Workspace)
** File description:
** OpusCodec.hpp
*/

#ifndef OPUSCODEC_HPP_
#define OPUSCODEC_HPP_

#include "ACodec.hpp"

extern "C" {
    #include <opus/opus.h>
}
#include <string>
#include <stdexcept>

namespace babel {

    #define OPUS_MAX_PACKET_SIZE 4000

    class OpusCodec : public ACodec {

        public:
            OpusCodec(int sampleRate = DEFAULT_SAMPLE_RATE, int channels = DEFAULT_CHANNELS, int bitrate = 24000);
            ~OpusCodec() override;
        
            std::vector<uint8_t> encode(const AudioBuffer& input) override;
            AudioBuffer decode(const std::vector<uint8_t>& input) override;
        
        private:
            OpusEncoder* _encoder;
            OpusDecoder* _decoder;

            const int _frameSize = 480; // 10ms @ 48kHz

    };
}

#endif