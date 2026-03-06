/*
** EPITECH PROJECT, 2026
** babel
** File description:
** ITCP.hpp
*/

#ifndef TCP_FACTORY_HPP_
#define TCP_FACTORY_HPP_

#include "TCPAsio.hpp"
#include <memory>
#include <stdexcept>

namespace babel {

    enum class TCPSystem {
        ASIO,
    };

    class TCPFactory {
        public:
            static std::unique_ptr<ITCPCommunication> create(TCPSystem system)
            {
                switch (system) {
                    case TCPSystem::ASIO:
                        return std::make_unique<TCPAsio>();
                    default:
                        throw std::invalid_argument("Unknown TCP system");
                }
            }
    };
}

#endif
