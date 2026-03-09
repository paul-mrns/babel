/*
** EPITECH PROJECT, 2026
** Untitled (Workspace)
** File description:
** AsioUDP.hpp
*/

#ifndef ASIO_UDP_HPP
#define ASIO_UDP_HPP

#include "AUDP.hpp"
#include <asio.hpp>
#include <thread>
#include <iostream>
#include <array>

namespace babel {
    class AsioUDP : public AUDPCommunication {
        public:
            AsioUDP() : _io(), _socket(_io) {}
            ~AsioUDP() { stop(); }

            uint16_t bind() override;
            void connect(const std::string& remoteIp, uint16_t remotePort) override;
            void sendAudio(const std::vector<unsigned char>& data) override;
            void stop() override;

        private:
            void asyncReceive();

            asio::io_context _io;
            asio::ip::udp::socket _socket;
            asio::ip::udp::endpoint _remoteEndpoint;
            asio::ip::udp::endpoint _senderEndpoint;
            std::thread _worker;
            std::array<unsigned char, 1500> _buffer;
    };
}

#endif