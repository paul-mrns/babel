/*
** EPITECH PROJECT, 2026
** Untitled (Workspace)
** File description:
** AIUDP.hpp
*/

#ifndef IUDP_HPP_
#define IUDP_HPP_

#include "../../../communication/UDPProtocol.hpp"
#include <vector>
#include <string>
#include <functional>
#include <cstdint>

namespace babel {
    class IUDPCommunication {
        public:
            virtual ~IUDPCommunication() = default;
        
            virtual uint16_t bind() = 0; 
            virtual void connect(const std::string& remoteIp, uint16_t remotePort) = 0;
            virtual void sendAudio(const std::vector<unsigned char>& data) = 0;
            virtual void setOnDataReceived(std::function<void(const std::vector<unsigned char>&)> cb) = 0;

            virtual bool isRunning() const = 0;
            virtual void stop() = 0;
    };
}

#endif