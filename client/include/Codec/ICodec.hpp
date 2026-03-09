/*
** EPITECH PROJECT, 2026
** Untitled (Workspace)
** File description:
** ICodec.hpp
*/

#ifndef ICODEC_HPP_
#define ICODEC_HPP_

#include <vector>
#include <cstdint>
#include "AudioStream/IAudioStream.hpp"

namespace babel {
    class ICodec{
        public:
            virtual ~ICodec() = default;
        
            virtual std::vector<uint8_t> encode(const AudioBuffer& input) = 0;
            virtual AudioBuffer decode(const std::vector<uint8_t>& input) = 0;
    };
}

#endif