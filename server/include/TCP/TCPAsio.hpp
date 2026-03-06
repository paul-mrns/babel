/*
** EPITECH PROJECT, 2026
** babel
** File description:
** ITCP.hpp
*/

#ifndef TCP_ASIO_HPP_
#define TCP_ASIO_HPP_

#include "ATCP.hpp"
#include <asio.hpp>
#include <unordered_map>
#include <iostream>
#include <cstring>

namespace babel {
    class TCPAsio : public ATCPCommunication {
        public:
            TCPAsio()
                : _io(), _acceptor(_io), _nextId(1), _running(false)
            {}
            ~TCPAsio() { stop(); }

            void listen(int port) override;
            void update() override;
            void stop() override;
            void sendTo(uint32_t clientId, tcp_OpCode op, const std::vector<uint8_t>& body) override;
            void broadcast(tcp_OpCode op, const std::vector<uint8_t>& body) override;
            void disconnectClient(uint32_t clientId) override;

        private:
            void asyncAccept();
            void asyncReadHeader(uint32_t id, std::shared_ptr<asio::ip::tcp::socket> socket);
            void asyncReadBody(uint32_t id, std::shared_ptr<asio::ip::tcp::socket> socket, Tcp_Header header);
            void handleMsg(uint32_t id, Tcp_Header header, std::vector<uint8_t> body);
            void handleDisconnect(uint32_t id);
            void sendRaw(std::shared_ptr<asio::ip::tcp::socket> socket, tcp_OpCode op, const std::vector<uint8_t>& body);

            asio::io_context _io;
            asio::ip::tcp::acceptor _acceptor;
            std::unordered_map<uint32_t, std::shared_ptr<asio::ip::tcp::socket>> _clients;
            uint32_t _nextId;
            bool _running;
};

} // namespace babel

#endif