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

Client::Client(TCPSystem tcpSys, UDPSystem udpSys)
    : _tcpSystem(tcpSys), _udpSystem(udpSys)
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

void Client::networkLoop()
{
    while (_running) {
        if (_tcp && _tcp->isRunning())
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
                _tcp->stop();
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
                    createCallSocket();
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

        case tcp_OpCode::START_CALL:
            startCall(body);
            break;

        default:
            std::cerr << "Unknown opcode: 0x" << std::hex << static_cast<int>(header.op_code) << std::dec << std::endl;
            break;
    }
}

void Client::createCallSocket()
{
    _state = ClientState::IN_CALL;
    std::cout << "Call accepted.\n";
    _udp = UDPFactory::create(_udpSystem);
    uint16_t chosenPort = _udp->bind();
    std::string localIp = _tcp->getIP();
    if (chosenPort == 0) {
        std::cerr << "[Client] Critical Error: Could not bind local UDP port.\n";
        return;
    }
    std::vector<uint8_t> packetBody;
    packetBody.push_back((chosenPort >> 8) & 0xFF);
    packetBody.push_back(chosenPort & 0xFF);
    packetBody.insert(packetBody.end(), localIp.begin(), localIp.end());
    _tcp->sendPacket(tcp_OpCode::START_CALL, packetBody);
}

void Client::startCall(std::vector<uint8_t> body)
{
    if (body.size() < 3) {
        std::cerr << "[Client] Error: Received invalid UDP packet." << std::endl;
        return;
    }
    uint16_t peerPort = (body[0] << 8) | body[1];
    std::string peerIp(body.begin() + 2, body.end());

    if (peerPort == 0 || peerIp.empty()) {
        std::cerr << "[Client] Error: Invalid Address -> " << peerIp << ":" << peerPort << std::endl;
        return;
    }
    std::cout << "Connecting to " << peerIp << ":" << peerPort << std::endl;
    if (_udp) {
        _udp->connect(peerIp, peerPort);
    } else {
        std::cerr << "[Client] Error: UDP system not initialized." << std::endl;
        return;
    }
    callProcess();
}

void Client::callProcess()
{
    _codec = CodecFactory::create(CodecSystem::OPUS);
    _audioStream = AudioStreamFactory::create(AudioStreamSystem::AUDIOPORT);

    if (!_codec || !_audioStream) {
        std::cerr << "[Client] Call initialization failed: hardware/codec error." << std::endl;
        return;
    }
    _udp->setOnDataReceived([this](const std::vector<uint8_t>& encryptedData) {
        if (_codec && _audioStream) {
            AudioBuffer audioPacket = _codec->decode(encryptedData);
            _audioStream->write(audioPacket);
        }
    });
    _audioStream->setOnReadCallback([this](const AudioBuffer& rawPcm) {
        if (_codec && _udp) {
            std::vector<uint8_t> opusPacket = _codec->encode(rawPcm);
            _udp->sendAudio(opusPacket);
        }
    });
    _audioStream->start();
}
}