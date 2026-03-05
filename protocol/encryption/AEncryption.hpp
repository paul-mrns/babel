/*
** EPITECH PROJECT, 2026
** Untitled (Workspace)
** File description:
** AEncryption.hpp
*/

#ifndef AENCRYPTION_HPP_
#define AENCRYPTION_HPP_

#include "IEncryption.hpp"
#include <openssl/sha.h>
#include <string>
#include <stdexcept>
#include <iostream>
#include <fstream>

namespace babel {
    class AEncryption : public IEncryption {
        public:
            explicit AEncryption()
            {
                std::string secret = getKey("ENCRYPTION_KEY");
                if (secret.empty()) {
                    throw std::runtime_error("Critical: ENCRYPTION_KEY not found in .env");
                }
                std::cout << "Key = " << secret << std::endl;
                SHA256(reinterpret_cast<const unsigned char*>(secret.c_str()), secret.size(), _key);
            }

        protected:
            unsigned char _key[SHA256_DIGEST_LENGTH];

        private:
            std::string getKey(const std::string& envVar)
            {
                const char* val = std::getenv(envVar.c_str());
                if (!val) {
                    std::cerr << "Warning: " << envVar << " not set in environment" << std::endl;
                }

                std::ifstream file("../.env");
                if (!file.is_open()) {
                    std::cerr << "Warning: Could not open .env file" << std::endl;
                    return "";
                }

                std::string line;
                while (std::getline(file, line)) {
                    if (line.find(envVar + "=") == 0) {
                        return line.substr(envVar.length() + 1);
                    }
                }
                return "";
            }
    };
}

#endif