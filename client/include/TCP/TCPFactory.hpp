/*
** EPITECH PROJECT, 2026
** babel
** File description:
** TCPFactory.hpp
*/

#pragma once

#include "TCPAsio.hpp"
#include <memory>
#include <stdexcept>

namespace babel {
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