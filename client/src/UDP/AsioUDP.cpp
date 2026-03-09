/*
** EPITECH PROJECT, 2026
** Untitled (Workspace)
** File description:
** AsioUDP.cpp
*/

#include "../../include/UDP/AsioUDP.hpp"
#include <iostream>  

namespace babel {

uint16_t AsioUDP::bind()
{
    try {
        if (_socket.is_open())
            _socket.close();
        _socket.open(asio::ip::udp::v4());
        _socket.bind(asio::ip::udp::endpoint(asio::ip::udp::v4(), 0));
        _localPort = _socket.local_endpoint().port();
        return _localPort;
    } catch (const std::exception& e) {
        std::cerr << "[Asio UDP] Bind() Error: " << e.what() << std::endl;
        return 0;
    }
}

void AsioUDP::connect(const std::string& remoteIp, uint16_t remotePort)
{
    if (_isRunning || !_socket.is_open())
        return;
    try {
        _remoteEndpoint = asio::ip::udp::endpoint(asio::ip::address::from_string(remoteIp), remotePort);
        _isRunning = true;
        asyncReceive();
        _worker = std::thread([this]() {
            try {
                auto work_guard = asio::make_work_guard(_io);
                _io.run();
            } catch (const std::exception& e) {
                std::cerr << "[Asio UDP] Thread error: " << e.what() << std::endl;
            }
        });
        std::cout << "[Asio UDP] Streaming to " << remoteIp << ":" << remotePort << " from local port " << _localPort << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[Asio UDP] Error: " << e.what() << std::endl;
    }
}

void AsioUDP::stop()
{
    if (!_isRunning)
        return;
    _isRunning = false;
    _io.stop();
    if (_worker.joinable())
        _worker.join();
    std::error_code ec;
    _socket.shutdown(asio::ip::udp::socket::shutdown_both, ec);
    if (_socket.is_open())
        _socket.close(ec);
    _io.reset();
    std::cout << "[Asio UDP] Disconnected\n";
}

void AsioUDP::sendAudio(const std::vector<unsigned char>& data)
{
    if (!_isRunning)
        return;
    _socket.async_send_to(asio::buffer(data), _remoteEndpoint, [](const std::error_code&, std::size_t){});
}

void AsioUDP::asyncReceive()
{
    if (!_isRunning)
        return;
    _socket.async_receive_from(asio::buffer(_buffer), _senderEndpoint, [this](const std::error_code& ec, std::size_t bytes) {
        if (!ec && bytes > 0 && _onDataReceived) {
            _onDataReceived(std::vector<unsigned char>(_buffer.begin(), _buffer.begin() + bytes));
            asyncReceive();
        }
    });
}

}