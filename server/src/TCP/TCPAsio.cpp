/*
** EPITECH PROJECT, 2026
** Untitled (Workspace)
** File description:
** TCPAsio.cpp
*/

#include "../../include/TCP/TCPAsio.hpp"
#include <iostream>
#include <thread>

namespace babel {

void TCPAsio::listen(int port)
{
    asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), static_cast<uint16_t>(port));
    _acceptor.open(endpoint.protocol());
    _acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    _acceptor.bind(endpoint);
    _acceptor.listen();
    _running = true;
    asyncAccept();
    std::cout << "[TCPAsio] Listening on port " << port << "\n";
}

void TCPAsio::update()
{
    if (_running)
        _io.poll();
}

void TCPAsio::stop()
{
    if (!_running)
        return;
    _running = false;
    std::error_code ec;
    _acceptor.close(ec);
    _clients.clear();
    _io.stop();
}

void TCPAsio::sendTo(uint32_t clientId, tcp_OpCode op, const std::vector<uint8_t>& body)
{
    auto it = _clients.find(clientId);
    if (it == _clients.end())
        return;
    sendRaw(it->second, op, body);
}

void TCPAsio::broadcast(tcp_OpCode op, const std::vector<uint8_t>& body)
{
    for (auto& [id, socket] : _clients)
        sendRaw(socket, op, body);
}

void TCPAsio::disconnectClient(uint32_t clientId)
{
    auto it = _clients.find(clientId);
    if (it == _clients.end())
        return;
    std::error_code ec;
    it->second->shutdown(asio::ip::tcp::socket::shutdown_both, ec);
    it->second->close(ec);
    _clients.erase(it);
    if (_onClientDisconnected)
        _onClientDisconnected(clientId);
}

void TCPAsio::asyncAccept()
{
    auto socket = std::make_shared<asio::ip::tcp::socket>(_io);
    _acceptor.async_accept(*socket,
        [this, socket](std::error_code ec) {
            if (ec) {
                if (_running)
                    std::cerr << "[TCPAsio] Accept error: " << ec.message() << "\n";
                return;
            }
            uint32_t id = _nextId++;
            _clients[id] = socket;
            if (_onClientConnected)
                _onClientConnected(id);
            asyncReadHeader(id, socket);
            asyncAccept();
        });
}

void TCPAsio::asyncReadHeader(uint32_t id, std::shared_ptr<asio::ip::tcp::socket> socket)
{
    auto header = std::make_shared<Tcp_Header>();
    asio::async_read(*socket,
        asio::buffer(header.get(), sizeof(Tcp_Header)),
        [this, id, socket, header](std::error_code ec, std::size_t) {
            if (ec) {
                handleDisconnect(id);
                return;
            }
            if (header->bodySize > 0)
                asyncReadBody(id, socket, *header);
            else
                handleMsg(id, *header, {});
        });
}

void TCPAsio::asyncReadBody(uint32_t id, std::shared_ptr<asio::ip::tcp::socket> socket, Tcp_Header header)
{
    auto body = std::make_shared<std::vector<uint8_t>>(header.bodySize);
    asio::async_read(*socket,
        asio::buffer(*body),
        [this, id, socket, header, body](std::error_code ec, std::size_t) {
            if (ec) {
                handleDisconnect(id);
                return;
            }
            handleMsg(id, header, *body);
        });
}

void TCPAsio::handleMsg(uint32_t id, Tcp_Header header, std::vector<uint8_t> body)
{
    if (_onMessage)
        _onMessage(id, header, body);
    auto it = _clients.find(id);
    if (it != _clients.end())
        asyncReadHeader(id, it->second);
}

void TCPAsio::handleDisconnect(uint32_t id)
{
    _clients.erase(id);
    if (_onClientDisconnected)
        _onClientDisconnected(id);
}

void TCPAsio::sendRaw(std::shared_ptr<asio::ip::tcp::socket> socket, tcp_OpCode op, const std::vector<uint8_t>& body)
{
    auto packet = std::make_shared<std::vector<uint8_t>>(sizeof(Tcp_Header) + body.size());
    Tcp_Header hdr;
    hdr.op_code  = op;
    hdr.bodySize = static_cast<uint16_t>(body.size());
    std::memcpy(packet->data(), &hdr, sizeof(Tcp_Header));
    if (!body.empty())
        std::memcpy(packet->data() + sizeof(Tcp_Header), body.data(), body.size());
    asio::async_write(*socket, asio::buffer(*packet),
        [packet](std::error_code ec, std::size_t) {
            if (ec)
                std::cerr << "[TCPAsio] Write error: " << ec.message() << "\n";
        });
}

}