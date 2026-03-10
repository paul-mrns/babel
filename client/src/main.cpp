/*
** EPITECH PROJECT, 2026
** babel
** File description:
** main.cpp
*/

#include "../include/Client.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc == 2 && std::string(argv[1]) == "--test") {
        std::cout << "CI worflow testing..." << std::endl;
        return 0;
    }
    try {
        babel::Client client(babel::TCPSystem::ASIO, babel::UDPSystem::ASIO);
        while (client._isRunning()) {
            client.run();
        }
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 84;
    }
    return 0;
}
