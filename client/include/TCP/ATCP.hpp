/*
** EPITECH PROJECT, 2026
** babel
** File description:
** ATCP.hpp
*/

#ifndef ATCP_HPP
#define ATCP_HPP

#include "ITCP.hpp"

namespace babel {
    class ATCPCommunication : public ITCPCommunication {
        public:
            void onMessage(std::function<void(Tcp_Header, std::vector<uint8_t>)> callback)
            {
                _onMessage = callback;
            }

            bool isRunning() const override { return _isRunning; }

            std::string getIP() const override { return _localIP; }
            uint16_t getPort() const override { return _localPort; }

        protected:
            bool _isRunning = false;
            Tcp_Header _header;
            std::vector<uint8_t> _body;
            std::function<void(Tcp_Header, std::vector<uint8_t>)> _onMessage;
            std::string _localIP;
            uint16_t _localPort;
    };
}

#endif