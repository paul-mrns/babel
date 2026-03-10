/*
** EPITECH PROJECT, 2026
** babel
** File description:
** Client.cpp
*/

#include "../include/Client.hpp"
#include <iostream>
#include <thread>

namespace babel {

void Client::initCommandDispatch()
{
    _dispatchTable.push_back({"CONNECT", [this](const auto& args) { connectCmd(args); }});
    _dispatchTable.push_back({"LOGIN", [this](const auto& args) { loginCmd(args); }});
    _dispatchTable.push_back({"REGISTER", [this](const auto& args) { registerCmd(args); }});
    _dispatchTable.push_back({"STATE", [this](const auto& args) { stateCmd(args); }});
    _dispatchTable.push_back({"HELP", [this](const auto& args) { helpCmd(args); }});
    _dispatchTable.push_back({"USERS", [this](const auto& args) { listCmd(args); }});
    _dispatchTable.push_back({"CALL", [this](const auto& args) { callCmd(args); }});
    _dispatchTable.push_back({"EXIT", [this](const auto& args) { exitCmd(args); }});
}

void Client::handleCommand(const std::string& command)
{
    std::vector<std::string> tokens;
    std::stringstream ss(command);
    std::string temp;
    while (ss >> temp) {
        tokens.push_back(temp);
    }

    if (tokens.empty())
        return;

    std::string cmdKeyword = tokens[0];
    std::vector<std::string> args;
    if (tokens.size() > 1) {
        args.assign(tokens.begin() + 1, tokens.end());
    }

    if (_state == ClientState::INCOMING_CALL) {
        answerCallCmd(cmdKeyword);
        return;
    }

    if (_state == ClientState::IN_CALL) {
        if (cmdKeyword == "END_CALL")
            endCallCmd(args);
        else if (cmdKeyword == "EXIT")
            exitCmd(args);
        else
            std::cout << "Only commands supported while in call are \"END_CALL\" and \"EXIT\"" << std::endl;
        return;
    }

    for (const auto& pair : _dispatchTable) {
        if (cmdKeyword == pair.first) {
            pair.second(args);
            return;
        }
    }
    std::cerr << "Unknown command: " << cmdKeyword << ". Type 'HELP' for help." << std::endl;
}

void Client::connectCmd(std::vector<std::string> args)
{
    if (args.size() != 2) {
        std::cerr << "Usage: CONNECT <ip> <port>\n";
        return;
    }

    try {
        _tcp = TCPFactory::create(_tcpSystem);
        _tcp->connectToServer(args[0], std::stoi(args[1]));
        _tcp->onMessage([this](Tcp_Header header, std::vector<uint8_t> body) {
            handlePacket(header, body);
        });
        _tcp->sendPacket(tcp_OpCode::CONNECT, {});
    } catch (const std::exception& ex) {
        std::cerr << "Connection failed: " << ex.what() << "\n";
    }
}

void Client::registerCmd(std::vector<std::string> args)
{
    if (args.size() != 2) {
        std::cerr << "Usage: REGISTER <username> <password>" << std::endl;
        return;
    }
    if (!_tcp || !_tcp->isRunning()) {
        std::cerr << "Not connected to server. Use CONNECT command first." << std::endl;
        return;
    }
    if (_state != ClientState::NOT_LOGGED_IN) {
        std::cerr << "Already logged in or in wrong state." << std::endl;
        return;
    }
    std::string username = args[0];
    _myUsername = username;
    std::string password = args[1];
    std::vector<uint8_t> body(username.begin(), username.end());
    body.push_back(0);
    body.insert(body.end(), password.begin(), password.end());
    _tcp->sendPacket(tcp_OpCode::REGISTER, body);
}

void Client::loginCmd(std::vector<std::string> args)
{
    if (args.size() != 2) {
        std::cerr << "Usage: LOGIN <username> <password>" << std::endl;
        return;
    }
    if (!_tcp || !_tcp->isRunning()) {
        std::cerr << "Not connected to server. Use CONNECT command first." << std::endl;
        return;
    }
    if (_state != ClientState::NOT_LOGGED_IN) {
        std::cerr << "Already logged in or in wrong state." << std::endl;
        return;
    }
    std::string username = args[0];
    _myUsername = username;
    std::string password = args[1];
    std::vector<uint8_t> body(username.begin(), username.end());
    body.push_back(0);
    body.insert(body.end(), password.begin(), password.end());
    _tcp->sendPacket(tcp_OpCode::LOGIN, body);
}

void Client::helpCmd(std::vector<std::string> args)
{
    static_cast<void>(args);
    std::cout << "Available commands:" << std::endl;
    std::cout << "  CONNECT <ip> <port> - Connect to the server" << std::endl;
    std::cout << "  LOGIN <username> <password> - Log in to the server" << std::endl;
    std::cout << "  REGISTER <username> <password> - Register a new account" << std::endl;
    std::cout << "  HELP - display available commands" << std::endl;
    std::cout << "  STATE - display the user state (debug)" << std::endl;
    std::cout << "  USERS - List online users" << std::endl;
    std::cout << "  CALL <username> - Call a user" << std::endl;
    std::cout << "  END_CALL - End your active call" << std::endl;
    std::cout << "  EXIT - Exit the client" << std::endl;
}

void Client::stateCmd(std::vector<std::string> args)
{
    static_cast<void>(args);
    switch (_state) {
        case ClientState::DISCONNECTED:
            std::cout << "Client is in state \"DISCONNECTED\": use \"CONNECT\" to connect to server " << std::endl;
            break;
        case ClientState::NOT_LOGGED_IN:
            std::cout << "Client is in state \"NOT_LOGGED_IN\": use \"LOGIN\" or \"REGISTER\"" << std::endl;    
            break;
        case ClientState::IDLE:
            std::cout << "Client is in state \"IDLE\"" << std::endl;    
            break;
        case ClientState::INCOMING_CALL:
            std::cout << "Client is in state \"INCOMMING CALL\": respond with \"ACCEPT\" or \"DECLINE\"" << std::endl;    
            break;
        case ClientState::IN_CALL:
            std::cout << "Client is in state \"IN_CALL\": you can exit by entering \"END_CALL\" or \"EXIT\" to exit Babel" << std::endl;    
            break;
        default:
            std::cout << "Unknow state\n";
            break;
    }
}

void Client::listCmd(std::vector<std::string> args)
{
    static_cast<void>(args);
    if (!_tcp || !_tcp->isRunning()) {
        std::cerr << "Not connected to server. Use CONNECT command first." << std::endl;
        return;
    }
    if (_state == ClientState::NOT_LOGGED_IN) {
        std::cerr << "Not logged in. Use LOGIN or REGISTER command first." << std::endl;
        return;
    }
    _tcp->sendPacket(tcp_OpCode::GET_USERS, {});
}

void Client::callCmd(std::vector<std::string> args)
{
    if (args.size() != 1) {
        std::cerr << "Usage: CALL <username>" << std::endl;
        return;
    }
    if (!_tcp || !_tcp->isRunning()) {
        std::cerr << "Not connected to server. Use CONNECT command first." << std::endl;
        return;
    }
    if (_state != ClientState::IDLE) {
        std::cerr << "Cannot make a call in the current state." << std::endl;
        return;
    }
    std::string targetUser = args[0];
    std::vector<uint8_t> body(targetUser.begin(), targetUser.end());
    _tcp->sendPacket(tcp_OpCode::CALL, body);
}

void Client::endCallCmd(std::vector<std::string> args)
{
    static_cast<void>(args);
    _tcp->sendPacket(tcp_OpCode::END_CALL, {});
    if (_udp && _udp->isRunning())
        _udp->stop();
    std::cout << "[Client] Call ended by user.\n";
}


void Client::exitCmd(std::vector<std::string> args)
{
    if (_tcp && _tcp->isRunning()) {
        _tcp->stop();
    }
    if (_udp && _udp->isRunning())
        endCallCmd(args);
    _running = false;
    std::cout << "Exiting client..." << std::endl;
}

void Client::answerCallCmd(std::string answer)
{
    if (answer == "ACCEPT") {
        _tcp->sendPacket(tcp_OpCode::ANSWER_CALL, {0});
    } else if (answer == "DECLINE") {
        _tcp->sendPacket(tcp_OpCode::ANSWER_CALL, {1});
    } else {    
        std::cerr << "Unknown command: " << answer << ". Type \"ACCEPT\" or \"DECLINE\"" << std::endl;
    }

}

}