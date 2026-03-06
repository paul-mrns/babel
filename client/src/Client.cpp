/*
** EPITECH PROJECT, 2026
** babel
** File description:
** Client.cpp
*/

#include "../include/Client.hpp"
#include <iostream>

namespace babel {

Client::Client(TCPSystem netType)
    : _netType(netType)
{
    _running = true;
    _state = ClientState::DISCONNECTED;
    std::cout << "------HEY, WELCOME TO BABEL CLIENT------" << std::endl;
    std::cout << "Enter 'HELP' for a list of commands." << std::endl;
    initCommandDispatch();
}

void Client::initCommandDispatch()
{
    _dispatchTable.push_back({"CONNECT", [this](const auto& args) { connectCmd(args); }});
    _dispatchTable.push_back({"LOGIN", [this](const auto& args) { loginCmd(args); }});
    _dispatchTable.push_back({"REGISTER", [this](const auto& args) { registerCmd(args); }});
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

    for (const auto& pair : _dispatchTable) {
        if (cmdKeyword == pair.first) {
            pair.second(args);
            return;
        }
    }
    std::cerr << "Unknown command: " << cmdKeyword << ". Type 'HELP' for help." << std::endl;
}

void Client::update()
{
    if (_tcp && _tcp->isConnected())
        _tcp->update();
    std::string input;

    std::cout << "Babel> ";
    std::getline(std::cin, input);
    if (!input.empty())
        handleCommand(input);
    if (_tcp && _tcp->isConnected())
        _tcp->update();
}

void Client::connectCmd(std::vector<std::string> args)
{
    if (args.size() != 2) {
        std::cerr << "Usage: CONNECT <ip> <port>\n";
        return;
    }

    try {
        _tcp = TCPFactory::create(_netType);
        _tcp->connectToServer(args[0], std::stoi(args[1]));
        _tcp->onMessage([this](Tcp_Header header, std::vector<uint8_t> body) {
            handlePacket(header, body);
        });
        _tcp->sendPacket(tcp_OpCode::CONNECT, {});
    } catch (const std::exception& ex) {
        std::cerr << "Connection failed: " << ex.what() << "\n";
    }
}

void Client::handlePacket(Tcp_Header header, std::vector<uint8_t> body)
{
    switch (header.op_code) {

        case tcp_OpCode::CONNECT_RESPONSE:
            if (!body.empty() && body[0] == 0x00) {
                _state = ClientState::NOT_LOGGED_IN;
            } else {
                std::cerr << "Server rejected connection.\n";
                _tcp->disconnect();
            }
            break;

        case tcp_OpCode::LOGIN_RESPONSE:
            if (!body.empty() && body[0] == 0x00) {
                _state = ClientState::IDLE;
                std::cout << "Logged in as " << _myUsername << ".\n";
            } else {
                std::cerr << "Login failed.\n";
            }
            break;

        case tcp_OpCode::REGISTER_RESPONSE:
            if (!body.empty() && body[0] == 0x00) {
                std::cout << "Registration successful. You can now LOGIN.\n";
            } else {
                std::cerr << "Registration failed.\n";
            }
            break;

        case tcp_OpCode::USERS_LIST: {
            std::cout << "Online users:\n";
            std::string user;
            for (uint8_t byte : body) {
                if (byte == 0x00) {
                    if (!user.empty()) {
                        std::cout << "  - " << user << "\n";
                        user.clear();
                    }
                } else {
                    user += static_cast<char>(byte);
                }
            }
            if (!user.empty())
                std::cout << "  - " << user << "\n";
            break;
        }

        case tcp_OpCode::CALL_RESPONSE:
            if (!body.empty()) {
                if (body[0] == 0x00) {
                    _state = ClientState::IN_CALL;
                    std::cout << "Call accepted.\n";
                } else if (body[0] == 0x01) {
                    std::cerr << "Call failed: user not found.\n";
                } else if (body[0] == 0x02) {
                    std::cerr << "Call failed: user is busy.\n";
                }
            }
            break;

        case tcp_OpCode::CALL:
            std::cout << "Incoming call from: "
                      << std::string(body.begin(), body.end()) << "\n";
            std::cout << "Type ACCEPT or DECLINE.\n";
            break;

        case tcp_OpCode::END_CALL:
            _state = ClientState::IDLE;
            std::cout << "Call ended.\n";
            break;

        default:
            std::cerr << "Unknown opcode: 0x"
                      << std::hex << static_cast<int>(header.op_code) << std::dec << "\n";
            break;
    }
}

void Client::loginCmd(std::vector<std::string> args)
{
    if (args.size() != 2) {
        std::cerr << "Usage: LOGIN <username> <password>" << std::endl;
        return;
    }
    if (!_tcp || !_tcp->isConnected()) {
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

void Client::registerCmd(std::vector<std::string> args)
{
    if (args.size() != 2) {
        std::cerr << "Usage: REGISTER <username> <password>" << std::endl;
        return;
    }
    if (!_tcp || !_tcp->isConnected()) {
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

void Client::helpCmd(std::vector<std::string> args)
{
    static_cast<void>(args);
    std::cout << "Available commands:" << std::endl;
    std::cout << "  CONNECT <ip> <port> - Connect to the server" << std::endl;
    std::cout << "  LOGIN <username> <password> - Log in to the server" << std::endl;
    std::cout << "  REGISTER <username> <password> - Register a new account" << std::endl;
    std::cout << "  USERS - List online users" << std::endl;
    std::cout << "  CALL <username> - Call a user" << std::endl;
    std::cout << "  EXIT - Exit the client" << std::endl;
}

void Client::listCmd(std::vector<std::string> args)
{
    static_cast<void>(args);
    if (!_tcp || !_tcp->isConnected()) {
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
    if (!_tcp || !_tcp->isConnected()) {
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

void Client::exitCmd(std::vector<std::string> args)
{
    static_cast<void>(args);
    std::cout << "Exiting client..." << std::endl;
    if (_tcp && _tcp->isConnected()) {
        _tcp->disconnect();
    }
    _running = false;
}

}