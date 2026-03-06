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

            bool isConnected() const override { return _isConnected; }

        protected:
            bool _isConnected = false;
            Tcp_Header _header;
            std::vector<uint8_t> _body;
            std::function<void(Tcp_Header, std::vector<uint8_t>)> _onMessage;
    };
}

#endif