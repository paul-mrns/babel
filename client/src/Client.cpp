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

Client::Client(TCPSystem netType)
    : _netType(netType)
{
    _running = true;
    _state = ClientState::DISCONNECTED;
    std::cout << "------HEY, WELCOME TO BABEL CLIENT------" << std::endl;
    std::cout << "Enter 'HELP' for a list of commands." << std::endl;
    initCommandDispatch();
}

void Client::run()
{
    _networkThread = std::thread(&Client::networkLoop, this);

    std::string input;
    while (_running) {
        std::getline(std::cin, input);
        if (!input.empty())
            handleCommand(input);
        if (!_running)
            break;
    }

    _running = false;
    if (_networkThread.joinable()) {
        _networkThread.join();
    }
}

void babel::Client::networkLoop()
{
    while (_running) {
        if (_tcp && _tcp->isConnected())
            _tcp->update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void Client::handlePacket(Tcp_Header header, std::vector<uint8_t> body)
{
    switch (header.op_code) {

        case tcp_OpCode::CONNECT_RESPONSE:
            if (!body.empty() && body[0] == SUCCESS) {
                _state = ClientState::NOT_LOGGED_IN;
            } else {
                std::cerr << "Server rejected connection.\n";
                _tcp->disconnect();
            }
            break;

        case tcp_OpCode::LOGIN_RESPONSE:
            if (!body.empty() && body[0] == SUCCESS) {
                _state = ClientState::IDLE;
                std::cout << "Logged in as " << _myUsername << ".\n";
            } else {
                std::cerr << "Login failed.\n";
            }
            break;

        case tcp_OpCode::REGISTER_RESPONSE:
            if (!body.empty() && body[0] == SUCCESS) {
                _state = ClientState::IDLE;
                std::cout << "Registration successful. You are now logged in.\n";
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
                        std::cout << "  - " << user << std::endl;
                        user.clear();
                    }
                } else {
                    user += static_cast<char>(byte);
                }
            }
            if (!user.empty())
                std::cout << "  - " << user << std::endl;
            break;
        }

        case tcp_OpCode::CALL_RESULT:
            if (!body.empty()) {
                if (body[0] == 0x00) {
                    connectToCall(body);
                } else if (body[0] == 0x01) {
                    std::cout << "Call failed: user not found.\n";
                } else if (body[0] == 0x02) {
                    std::cout << "Call failed: user is busy.\n";
                } else if (body[0] == 0X03) {
                    std::cout << "Call has been declined.\n";
                } else {
                    std::cout << "Cannot call yourself.\n";
                }
            }
            break;

        case tcp_OpCode::INCOMING_CALL:
            std::cout << "Incoming call from: " << std::string(body.begin(), body.end()) << std::endl;
            std::cout << "Enter \"ACCEPT\" or \"DECLINE\"" << std::endl;
            _state = ClientState::INCOMING_CALL;
            break;

        case tcp_OpCode::END_CALL:
            _state = ClientState::IDLE;
            std::cout << "Call ended.\n";
            break;

        default:
            std::cerr << "Unknown opcode: 0x"
                      << std::hex << static_cast<int>(header.op_code) << std::dec << std::endl;
            break;
    }
}

void Client::connectToCall(const std::vector<uint8_t>& body)
{
    _state = ClientState::IN_CALL;
    std::cout << "Call accepted.\n";

    if (body.size() < 4) {
        std::cerr << "[Client] CALL_RESULT packet is invalid.\n";
        return;
    }
    uint16_t remotePort = (body[1] << 8) | body[2];
    std::string remoteIp(body.begin() + 3, body.end());
    std::cout << "------------------------------------------\n";
    std::cout << ">>> TARGET ACQUIRED <<<\n";
    std::cout << "Connecting UDP stream to: " << remoteIp << ":" << remotePort << "\n";
    std::cout << "------------------------------------------\n";

    //open UDP port
}

}