/*
** EPITECH PROJECT, 2026
** babel
** File description:
** Server.hpp
*/

#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <memory>
#include <string>
#include <vector>
#include "../../communication/TCPProtocol.hpp"
#include "TCP/TCPFactory.hpp"

namespace babel {

    struct User {
        uint32_t clientId;
        std::string username;
        std::string password;
        bool online = false;
    };

    class Server {
        public:
            explicit Server(TCPSystem netType, int port = 4242);
            ~Server() = default;
            void update();

        private:
    
            void onMessage(uint32_t clientId, Tcp_Header hdr, std::vector<uint8_t> body);
            void onClientConnected(uint32_t clientId);
            void onClientDisconnected(uint32_t clientId);

            void handleConnect(uint32_t clientId);
            void handleLogin(uint32_t clientId, const std::vector<uint8_t>& body);
            void handleRegister(uint32_t clientId, const std::vector<uint8_t>& body);
            void handleGetUsers(uint32_t clientId);
            void handleCall(uint32_t clientId, const std::vector<uint8_t>& body);
            void handleEndCall(uint32_t clientId);

    
            User* findByClientId(uint32_t clientId);
            User* findByUsername(const std::string& username);

            std::unique_ptr<ITCPCommunication> _tcp;
            std::vector<User> _users;
    };
} // namespace babelabel

#endif