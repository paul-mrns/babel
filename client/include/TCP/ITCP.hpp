/*
** EPITECH PROJECT, 2026
** babel
** File description:
** ITCP.hpp
*/

#ifndef ITCP_HPP
#define ITCP_HPP

#include "../../../communication/TCPProtocol.hpp"
#include <vector>
#include <string>
#include <functional>

namespace babel {
    class ITCPCommunication {
        public:
            virtual ~ITCPCommunication() = default;
            virtual bool connectToServer(const std::string& ip, int port) = 0;
            virtual void sendPacket(tcp_OpCode op, const std::vector<uint8_t>& body) = 0;
            virtual void update() = 0;
            virtual void onMessage(std::function<void(Tcp_Header, std::vector<uint8_t>)> callback) = 0;
            virtual std::string getIP() const = 0;
            virtual uint16_t getPort() const = 0;

            virtual bool isRunning() const = 0;
            virtual void stop() = 0;
    };
}

#endif