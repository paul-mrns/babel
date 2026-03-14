/*
** EPITECH PROJECT, 2026
** babel
** File description:
** Client.hpp
*/

#ifndef CLIENT_HPP_
#define CLIENT_HPP_

#include "TCP/TCPFactory.hpp"
#include "UDP/UDPFactory.hpp"
#include "AudioStream/AudioStreamFactory.hpp"
#include "Codec/CodecFactory.hpp"

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
            Client(TCPSystem tcpSys, UDPSystem udpSys, AudioStreamSystem audioSys, CodecSystem codecSys);
            ~Client() = default;

            void run();

            ClientState getState() const { return _state; }
            bool _isRunning() const { return _running; }

        private:
            void initCommandDispatch();
            void handleCommand(const std::string& command);
            void handlePacket(Tcp_Header hdr, std::vector<uint8_t> body);
        
            void connectCmd(std::vector<std::string> args);
            void loginCmd(std::vector<std::string> args);
            void registerCmd(std::vector<std::string> args);
            void helpCmd(std::vector<std::string> args);
            void stateCmd(std::vector<std::string> args);
            void listCmd(std::vector<std::string> args);
            void callCmd(std::vector<std::string> args);
            void endCallCmd(std::vector<std::string> args);
            void exitCmd(std::vector<std::string> args);
            void answerCallCmd(std::string answer);

            void createCallSocket();
            void startCall(std::vector<uint8_t> body);

            void networkLoop();

            void callProcess();

            std::thread _networkThread;
            std::vector<std::pair<std::string, std::function<void(const std::vector<std::string>&)>>> _dispatchTable;

            TCPSystem _tcpSystem;
            UDPSystem _udpSystem;
            AudioStreamSystem _audioSystem;
            CodecSystem _codecSystem;
            
            std::unique_ptr<ITCPCommunication> _tcp;
            std::unique_ptr<IUDPCommunication> _udp;
            std::unique_ptr<IAudioStream> _audioStream;
            std::unique_ptr<ICodec> _encoder;
            std::unique_ptr<ICodec> _decoder;

            ClientState _state;
            std::string _myUsername;
            bool _running;
    };
}

#endif