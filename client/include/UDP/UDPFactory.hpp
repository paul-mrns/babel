/*
** EPITECH PROJECT, 2026
** babel
** File description:
** UDPFactory.hpp
*/

#ifndef UDPFACTORY_HPP_
#define UDPFACTORY_HPP_

#include "AsioUDP.hpp"
#include <memory>
#include <stdexcept>

namespace babel {
    class UDPFactory {
        public:
            static std::unique_ptr<IUDPCommunication> create(UDPSystem system)
            {
                switch (system) {
                    case UDPSystem::ASIO:
                        return std::make_unique<AsioUDP>();
                    default:
                        throw std::invalid_argument("Unknown UDP system");
                }
            }
    };
}

#endif