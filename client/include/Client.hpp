/*
** EPITECH PROJECT, 2026
** babel
** File description:
** Client.hpp
*/

#ifndef CLIENT_HPP_
#define CLIENT_HPP_

#include "AudioStream/AudioPortStream.hpp"
#include "Codec/OpusCodec.hpp"
#include "TCP/TCPFactory.hpp"
#include "UDP/UDPFactory.hpp"
#include <string>
#include <memory>
#include <vector>
#include <functional>

namespace babel {

    enum class ClientState {
        DISCONNECTED,
        NOT_LOGGED_IN,
        IDLE,
        INCOMING_CALL,
        IN_CALL
    };

    class Client {
        public:
            Client(TCPSystem tcpSys, UDPSystem udpSys);
            ~Client() = default;

            void run();

            ClientState getState() const { return _state; }
            bool _isRunning() const { return _running; }

        private:
            void initCommandDispatch();
            void handleCommand(const std::string& command);
            void handlePacket(Tcp_Header hdr, std::vector<uint8_t> body);
        
            //commands
            void connectCmd(std::vector<std::string> args);
            void loginCmd(std::vector<std::string> args);
            void registerCmd(std::vector<std::string> args);
            void helpCmd(std::vector<std::string> args);
            void listCmd(std::vector<std::string> args);
            void callCmd(std::vector<std::string> args);
            void endCallCmd(std::vector<std::string> args);
            void exitCmd(std::vector<std::string> args);
            void answerCallCmd(std::string answer);

            void createCallSocket();
            void startCall(std::vector<uint8_t> body);

            void networkLoop();

            std::thread _networkThread;
            std::vector<std::pair<std::string, std::function<void(const std::vector<std::string>&)>>> _dispatchTable;

            TCPSystem _tcpSystem;
            std::unique_ptr<ITCPCommunication> _tcp;

            UDPSystem _udpSystem;
            std::unique_ptr<IUDPCommunication> _udp;

            ClientState _state;
            std::string _myUsername;
            bool _running;
    };
}

#endif