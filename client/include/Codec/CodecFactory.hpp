/*
** EPITECH PROJECT, 2026
** babel
** File description:
** CodecFactory.hpp
*/

#ifndef CODEC_FACTORY_HPP_
#define CODEC_FACTORY_HPP_

#include <memory>
#include "OpusCodec.hpp"

namespace babel {

    enum class CodecSystem {
        OPUS
    };

    class CodecFactory {
        public:
            static std::unique_ptr<ICodec> create(CodecSystem type) {
                if (type == CodecSystem::OPUS) {
                    return std::make_unique<OpusCodec>();
                }
                return nullptr;
            }
    };
}

#endif