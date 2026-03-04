/*
** EPITECH PROJECT, 2026
** babel
** File description:
** main.cpp
*/

#include "../include/AudioStream/PortAudioStream.hpp"
#include "../include/Codec/OpusCodec.hpp"
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
        babel::OpusCodec codec;
        std::vector<std::vector<uint8_t>> encodedBuffers;
        std::vector<babel::AudioBuffer> decodedBuffers;

        babel::PortAudioStream stream;

        std::cout << "Recording for 5 seconds..." << std::endl;

        stream.setOnReadCallback([&](const babel::AudioBuffer& buf) {
            encodedBuffers.push_back(codec.encode(buf));
        });

        stream.start();
        std::this_thread::sleep_for(std::chrono::seconds(5));
        stream.stop();

        std::cout << "Decoding buffers..." << std::endl;
        for (const auto& enc : encodedBuffers) {
            decodedBuffers.push_back(codec.decode(enc));
        }
        std::cout << "Playing back audio..." << std::endl;

        stream.start();
        for (const auto& buf : decodedBuffers) {
            stream.write(buf);
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Simulate
        }
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Wait for playback to finish
        stream.stop();
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 84;
    }
    return 0;
}
