/*
** EPITECH PROJECT, 2026
** babel
** File description:
** TCPAsio.hpp
*/

#ifndef TCP_ASIO_HPP_
#define TCP_ASIO_HPP_

#pragma once

#include <asio.hpp>
#include "ATCP.hpp"
#include <iostream>

namespace babel {
    class TCPAsio : public ATCPCommunication {
        
        public:
            TCPAsio() : _io(), _socket(_io) {}
            ~TCPAsio() { disconnect(); }

            bool connectToServer(const std::string& ip, int port) override;
            void sendPacket(tcp_OpCode op, const std::vector<uint8_t>& body) override;
            void update() override;
            void disconnect() override;

        private:
            void asyncReadHeader();
            void asyncReadBody();
            void handleMsg();

            asio::io_context        _io;
            asio::ip::tcp::socket   _socket;
            Tcp_Header              _incomingHeader{};
            std::vector<uint8_t>    _incomingBody;
};

}

#endif 