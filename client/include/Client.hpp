/*
** EPITECH PROJECT, 2026
** babel
** File description:
** Client.hpp
*/

#ifndef CLIENT_HPP_
#define CLIENT_HPP_

#include "AudioStream/PortAudioStream.hpp"
#include "Codec/OpusCodec.hpp"
#include "TCP/TCPFactory.hpp"
#include <string>
#include <memory>
#include <vector>
#include <functional>

namespace babel {
    class Client {
        public:
            Client(void);
            ~Client() = default;
    };
}

#endif