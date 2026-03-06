/*
** EPITECH PROJECT, 2026
** babel
** File description:
** Client.cpp
*/

#include "../../include/TCP/TCPAsio.hpp"

namespace babel {

bool TCPAsio::connectToServer(const std::string& ip, int port)
{
    try {
        asio::ip::tcp::resolver resolver(_io);
        auto endpoints = resolver.resolve(ip, std::to_string(port));
        asio::connect(_socket, endpoints);
        _isConnected = true;
        asyncReadHeader();
        std::cout << "[Asio] Connected to " << ip << ":" << port << "\n";
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[Asio] Connection failed: " << e.what() << "\n";
        _isConnected = false;
        return false;
    }
}

void TCPAsio::sendPacket(tcp_OpCode op, const std::vector<uint8_t>& body)
{
    if (!_isConnected)
        return;
    auto packet = std::make_shared<std::vector<uint8_t>>();
    packet->resize(sizeof(Tcp_Header) + body.size());
    Tcp_Header hdr;
    hdr.op_code  = op;
    hdr.bodySize = static_cast<uint16_t>(body.size());
    std::memcpy(packet->data(), &hdr, sizeof(Tcp_Header));
    if (!body.empty())
        std::memcpy(packet->data() + sizeof(Tcp_Header), body.data(), body.size());
    asio::async_write(_socket, asio::buffer(*packet), [this, packet](std::error_code ec, std::size_t) {
        if (ec) {
            std::cerr << "[Asio] Write error: " << ec.message() << "\n";
            disconnect();
        }
    });
}

void TCPAsio::update()
{
    if (_isConnected)
        _io.poll();
}

void TCPAsio::disconnect()
{
    if (!_isConnected)
        return;
    _isConnected = false;
    std::error_code ec;
    _socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
    _socket.close(ec);
    std::cout << "[Asio] Disconnected\n";
}

void TCPAsio::asyncReadHeader()
{
    asio::async_read(_socket, asio::buffer(&_incomingHeader, sizeof(Tcp_Header)), [this](std::error_code ec, std::size_t) {
        if (ec) {
            std::cerr << "[Asio] Read header error: " << ec.message() << "\n";
            disconnect();
            return;
        }
        _incomingBody.resize(_incomingHeader.bodySize);
        if (_incomingHeader.bodySize > 0)
            asyncReadBody();
        else
            handleMsg();
    });
}

void TCPAsio::asyncReadBody()
{
    asio::async_read(_socket, asio::buffer(_incomingBody), [this](std::error_code ec, std::size_t) {
        if (ec) {
            std::cerr << "[Asio] Read body error: " << ec.message() << "\n";
            disconnect();
            return;
        }
        handleMsg();
    });
}

void TCPAsio::handleMsg()
{
    if (_onMessage)
        _onMessage(_incomingHeader, _incomingBody);
    asyncReadHeader();
}

}