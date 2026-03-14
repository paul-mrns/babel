/*
** EPITECH PROJECT, 2026
** babel
** File description:
** ITCP.hpp
*/

#ifndef ITCP_HPP_
#define ITCP_HPP_

#include <cstdint>
#include <functional>
#include <vector>
#include <string>
#include "../../../communication/TCPProtocol.hpp"

namespace babel {

class ITCPCommunication {
    public:
        virtual ~ITCPCommunication() = default;

        virtual void listen(int port) = 0;
        virtual void update() = 0;
        virtual void stop() = 0;

        virtual void sendTo(uint32_t clientId, tcp_OpCode op, const std::vector<uint8_t>& body) = 0;
        virtual void broadcast(tcp_OpCode op, const std::vector<uint8_t>& body) = 0;
        virtual void disconnectClient(uint32_t clientId) = 0;

        virtual void onClientConnected(std::function<void(uint32_t)> callback) = 0;
        virtual void onClientDisconnected(std::function<void(uint32_t)> callback) = 0;
        virtual void onMessage(std::function<void(uint32_t, Tcp_Header, std::vector<uint8_t>)> callback) = 0;

        virtual std::string getClientIp(uint32_t clientId) const = 0;
};

}

#endif