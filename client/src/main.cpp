/*
** EPITECH PROJECT, 2026
** babel
** File description:
** main.cpp
*/

#include "../include/Client.hpp"
#include <thread>
#include <chrono>
#include <cmath>
#include <iostream>
#include <iomanip>

int main(int argc, char* argv[])
{
    if (argc == 2 && std::string(argv[1]) == "--test") {
        std::cout << "CI worflow testing..." << std::endl;
        return 0;
    }
    try {
        babel::Client client(babel::TCPSystem::ASIO);
        while (client._isRunning()) {
            client.update();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 84;
    }
    return 0;
}
