/*
** EPITECH PROJECT, 2026
** babel
** File description:
** Server.cpp
*/

#include "../include/Server.hpp"
#include <iostream>
#include <algorithm>

namespace babel {

Server::Server(TCPSystem netType, int port)
{
    _tcp = TCPFactory::create(netType);
    _tcp->onClientConnected([this](uint32_t id) {
        onClientConnected(id);
    });
    _tcp->onClientDisconnected([this](uint32_t id) {
        onClientDisconnected(id);
    });
    _tcp->onMessage([this](uint32_t id, Tcp_Header hdr, std::vector<uint8_t> body) {
        onMessage(id, hdr, body);
    });
    _tcp->listen(port);
}

void Server::update()
{
    _tcp->update();
}

void Server::onClientConnected(uint32_t clientId)
{
    std::cout << "[Server] Client " << clientId << " connected\n";
}

void Server::onClientDisconnected(uint32_t clientId)
{
    std::cout << "[Server] Client " << clientId << " disconnected\n";
    User *user = findByClientId(clientId);
    if (user)
        user->online = false;
}

void Server::onMessage(uint32_t clientId, Tcp_Header hdr, std::vector<uint8_t> body)
{
    switch (hdr.op_code) {
        case tcp_OpCode::CONNECT:   
            handleConnect(clientId);        
            break;
        case tcp_OpCode::LOGIN:     
            handleLogin(clientId, body);    
            break;
        case tcp_OpCode::REGISTER:  
            handleRegister(clientId, body); 
            break;
        case tcp_OpCode::GET_USERS: 
            handleGetUsers(clientId);       
            break;
        case tcp_OpCode::CALL:      
            handleCall(clientId, body);     
            break;
        case tcp_OpCode::END_CALL:  
            handleEndCall(clientId);        
            break;
        default:
            std::cerr << "[Server] Unknown opcode: 0x"
                      << std::hex << static_cast<int>(hdr.op_code)
                      << std::dec << "\n";
            break;
    }
}

void Server::handleConnect(uint32_t clientId)
{
    _tcp->sendTo(clientId, tcp_OpCode::CONNECT_RESPONSE, {0x00});
    std::cout << "[Server] Client " << clientId << " handshake OK\n";
}

void Server::handleLogin(uint32_t clientId, const std::vector<uint8_t>& body)
{
    auto sep = std::find(body.begin(), body.end(), 0x00);
    if (sep == body.end()) {
        _tcp->sendTo(clientId, tcp_OpCode::LOGIN_RESPONSE, {0x01});
        return;
    }
    std::string username(body.begin(), sep);
    std::string password(sep + 1, body.end());

    User *user = findByUsername(username);
    if (!user || user->password != password) {
        _tcp->sendTo(clientId, tcp_OpCode::LOGIN_RESPONSE, {0x01});
        return;
    }
    user->clientId = clientId;
    user->online   = true;
    _tcp->sendTo(clientId, tcp_OpCode::LOGIN_RESPONSE, {0x00});
    std::cout << "[Server] '" << username << "' logged in\n";
}

void Server::handleRegister(uint32_t clientId, const std::vector<uint8_t>& body)
{
    auto sep = std::find(body.begin(), body.end(), 0x00);
    if (sep == body.end()) {
        _tcp->sendTo(clientId, tcp_OpCode::REGISTER_RESPONSE, {0x01});
        return;
    }
    std::string username(body.begin(), sep);
    std::string password(sep + 1, body.end());

    if (findByUsername(username)) {
        _tcp->sendTo(clientId, tcp_OpCode::REGISTER_RESPONSE, {0x01});
        return;
    }
    _users.push_back({clientId, username, password, false});
    _tcp->sendTo(clientId, tcp_OpCode::REGISTER_RESPONSE, {0x00});
    std::cout << "[Server] '" << username << "' registered\n";
}

void Server::handleGetUsers(uint32_t clientId)
{
    std::vector<uint8_t> body;
    for (const auto& user : _users) {
        if (!user.online)
            continue;
        body.insert(body.end(), user.username.begin(), user.username.end());
        body.push_back(0x00);
    }
    _tcp->sendTo(clientId, tcp_OpCode::USERS_LIST, body);
}

void Server::handleCall(uint32_t clientId, const std::vector<uint8_t>& body)
{
    std::string target(body.begin(), body.end());
    User *caller = findByClientId(clientId);
    User *callee = findByUsername(target);

    if (!callee || !callee->online) {
        _tcp->sendTo(clientId, tcp_OpCode::CALL_RESPONSE, {0x01}); // not found
        return;
    }
    std::vector<uint8_t> callBody;
    if (caller)
        callBody.assign(caller->username.begin(), caller->username.end());

    _tcp->sendTo(callee->clientId, tcp_OpCode::CALL, callBody);
    _tcp->sendTo(clientId, tcp_OpCode::CALL_RESPONSE, {0x00});
    std::cout << "[Server] Call from '" << (caller ? caller->username : "?")
              << "' to '" << target << "'\n";
}

void Server::handleEndCall(uint32_t clientId)
{
    std::cout << "[Server] Client " << clientId << " ended call\n";
}

User* Server::findByClientId(uint32_t clientId)
{
    for (auto& user : _users)
        if (user.clientId == clientId)
            return &user;
    return nullptr;
}

User* Server::findByUsername(const std::string& username)
{
    for (auto& user : _users)
        if (user.username == username)
            return &user;
    return nullptr;
}

}