/*
** EPITECH PROJECT, 2026
** Untitled (Workspace)
** File description:
** main.cpp
*/

#include "../include/Server.hpp"
#include <iostream>
#include <thread>

int main(int argc, char** argv)
{
    if (argc == 2 && std::string(argv[1]) == "--test") {
        std::cout << "CI workflow testing...\n";
        return 0;
    }

    int port = 4242;
    if (argc == 2)
        port = std::stoi(argv[1]);

    try {
        babel::Server server(babel::TCPSystem::ASIO, port);
        std::cout << "[Main] Babel Server running on port " << port
                  << ". Press Ctrl+C to stop.\n";
        while (true) {
            server.update();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    } catch (const std::exception& e) {
        std::cerr << "[Fatal] " << e.what() << "\n";
        return 84;
    }
    return 0;
}