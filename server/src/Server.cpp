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
    std::string clientIp = _tcp->getClientIp(clientId);
    auto newSession = std::make_shared<UserSession>(clientId, clientIp);
    _sessions[clientId] = newSession;
    std::cout << "[Server] Client " << clientId << " connected from " << clientIp << "\n";
}

void Server::onClientDisconnected(uint32_t clientId)
{
    std::cout << "[Server] Client " << clientId << " disconnected\n";
    handleEndCall(clientId);
    auto itQuery = std::find_if(_callQueries.begin(), _callQueries.end(), [clientId](const auto& pair) {
        return pair.first == clientId || pair.second == clientId;
    });

    if (itQuery != _callQueries.end()) {
        uint32_t callerId = itQuery->first;
        uint32_t calleeId = itQuery->second;
        uint32_t otherId = (callerId == clientId) ? calleeId : callerId;
        _tcp->sendTo(otherId, tcp_OpCode::CALL_RESULT, {0x01}); // Not found/Disconnected
        
        _callQueries.erase(itQuery);
    }
    _sessions.erase(clientId);
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
        case tcp_OpCode::ANSWER_CALL:
            handleAnswerCall(clientId, body);
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

    auto it = _sessions.find(clientId);
    if (it == _sessions.end())
        return;
    auto& currentSession = it->second;
    bool alreadyLoggedIn = std::any_of(_sessions.begin(), _sessions.end(), [&](auto& pair) {
        return pair.second->isLoggedIn() && pair.second->getUsername() == username;
    });
    if (alreadyLoggedIn) {
        _tcp->sendTo(clientId, tcp_OpCode::LOGIN_RESPONSE, {0x02});
        return;
    }
    currentSession->setCredentials(username, password);
    _tcp->sendTo(clientId, tcp_OpCode::LOGIN_RESPONSE, {0x00});
    std::cout << "[Server] Session " << clientId << " identified as '" << username << "'\n";
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
    auto itBusy = std::find_if(_sessions.begin(), _sessions.end(), [&](const auto& pair) {
        return pair.second->isLoggedIn() && pair.second->getUsername() == username;
    });

    if (itBusy != _sessions.end()) {
        _tcp->sendTo(clientId, tcp_OpCode::REGISTER_RESPONSE, {0x01});
        return;
    }
    auto it = _sessions.find(clientId);
    if (it != _sessions.end()) {
        it->second->setCredentials(username, password);
        _tcp->sendTo(clientId, tcp_OpCode::REGISTER_RESPONSE, {0x00});
        std::cout << "[Server] '" << username << "' registered and logged in session " << clientId << "\n";
    }
}

void Server::handleGetUsers(uint32_t clientId)
{
    std::vector<uint8_t> body;
    for (const auto& [id, session] : _sessions) {
        if (session->isLoggedIn() && session->getId() != clientId) {
            const std::string& name = session->getUsername();
            
            body.insert(body.end(), name.begin(), name.end());
            body.push_back(0x00);
        }
    }
    _tcp->sendTo(clientId, tcp_OpCode::USERS_LIST, body);
    std::cout << "[Server] Sent online user list to session " << clientId << "\n";
}

void Server::handleCall(uint32_t clientId, const std::vector<uint8_t>& body)
{
    std::string targetName(body.begin(), body.end());
    auto callerSession = _sessions[clientId];
    auto calleeSession = findSessionByUsername(targetName);
    if (callerSession && calleeSession && callerSession->getId() == calleeSession->getId()) {
        _tcp->sendTo(clientId, tcp_OpCode::CALL_RESULT, {0x04}); // Calling yourself
        return;
    }
    if (!calleeSession || !calleeSession->isLoggedIn()) {
        _tcp->sendTo(clientId, tcp_OpCode::CALL_RESULT, {0x01}); // Not found / Offline
        return;
    }
    bool isBusy = calleeSession->isInCall();
    if (isBusy) {
        _tcp->sendTo(clientId, tcp_OpCode::CALL_RESULT, {0x02}); // User busy
        return;
    }
    std::vector<uint8_t> callBody;
    if (callerSession) {
        std::string callerName = callerSession->getUsername();
        callBody.assign(callerName.begin(), callerName.end());
    }
    _callQueries[clientId] = calleeSession->getId();
    _tcp->sendTo(calleeSession->getId(), tcp_OpCode::INCOMING_CALL, callBody);
    std::cout << "[Server] " << callerSession->getUsername() << " is calling " << targetName << "\n";
}

void Server::handleAnswerCall(uint32_t calleeId, const std::vector<uint8_t>& body)
{
    uint32_t callerId = 0;
    for (auto const& [caller, callee] : _callQueries) {
        if (callee == calleeId) {
            callerId = caller;
            break;
        }
    }
    if (callerId == 0)
        return;
    auto callerSession = _sessions[callerId];
    auto calleeSession = _sessions[calleeId];
    if (!callerSession || !calleeSession) {
        _callQueries.erase(callerId);
        return;
    }
    if (!body.empty() && body[0] == 0x00) {
        std::string callerIp = callerSession->getIp();
        std::string calleeIp = calleeSession->getIp();
        uint16_t callerUdpPort = 50002; 
        uint16_t calleeUdpPort = (body.size() >= 3) ? (body[1] << 8 | body[2]) : 50002;

        callerSession->setInCall(true);
        calleeSession->setInCall(true);
        _activeCalls[callerId] = calleeId;
        std::vector<uint8_t> resToCaller = {0x00};
        resToCaller.push_back((calleeUdpPort >> 8) & 0xFF);
        resToCaller.push_back(calleeUdpPort & 0xFF);
        resToCaller.insert(resToCaller.end(), calleeIp.begin(), calleeIp.end());
        _tcp->sendTo(callerId, tcp_OpCode::CALL_RESULT, resToCaller);

        _activeCalls[calleeId] = callerId;
        std::vector<uint8_t> resToCallee = {0x00};
        resToCallee.push_back((callerUdpPort >> 8) & 0xFF);
        resToCallee.push_back(callerUdpPort & 0xFF);
        resToCallee.insert(resToCallee.end(), callerIp.begin(), callerIp.end());
        _tcp->sendTo(calleeId, tcp_OpCode::CALL_RESULT, resToCallee);
        
        std::cout << "[Server] Call Active: " << callerSession->getUsername() 
                  << " (" << callerIp << ") <-> " << calleeSession->getUsername() 
                  << " (" << calleeIp << ")\n";
    } 
    else {
        std::cout << "[Server] " << calleeSession->getUsername() << " has declined the call from " << callerSession->getUsername() << "." << std::endl;
        _tcp->sendTo(callerId, tcp_OpCode::CALL_RESULT, {0x03});
        _tcp->sendTo(calleeId, tcp_OpCode::CALL_RESULT, {0x03});
    }
    _callQueries.erase(callerId);
}

void Server::handleEndCall(uint32_t clientId)
{
    auto it = _activeCalls.find(clientId);
    if (it == _activeCalls.end()) {
        _callQueries.erase(clientId);
        return;
    }
    uint32_t peerId = it->second;
    auto session = _sessions[clientId];
    auto peerSession = _sessions[peerId];
    if (session) session->setInCall(false);
    if (peerSession) {
        peerSession->setInCall(false);
        _tcp->sendTo(peerId, tcp_OpCode::END_CALL, {});
    }
    _activeCalls.erase(clientId);
    _activeCalls.erase(peerId);
    std::cout << "[Server] Call between " << clientId << " and " << peerId << " terminated.\n";
}

std::shared_ptr<UserSession> Server::findSessionByUsername(const std::string& username)
{
    for (auto const& [id, session] : _sessions) {
        if (session->isLoggedIn() && session->getUsername() == username) {
            return session;
        }
    }
    return nullptr;
}

}