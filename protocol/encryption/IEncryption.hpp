/*
** EPITECH PROJECT, 2026
** Untitled (Workspace)
** File description:
** IEncryption.hpp
*/

#ifndef IENCRYPTION_HPP_
#define IENCRYPTION_HPP_

#include <vector>
#include <cstdint>

namespace babel {
    class IEncryption {
    public:
        virtual ~IEncryption() = default;

        virtual std::vector<uint8_t> encrypt(const std::vector<uint8_t>& plaintext) = 0;
        virtual std::vector<uint8_t> decrypt(const std::vector<uint8_t>& data) = 0;
    };
}

#endif