/*
** EPITECH PROJECT, 2026
** babel
** File description:
** main.cpp
*/

#include "../include/Client.hpp"
#include <iostream>

void runAudioTest(const std::string& audioLibrary)
{
    std::cout << "--- Audio Quality Test: 'Magneto' Mode (Record -> Replay) ---" << std::endl;
     std::cout << "--- Audio Quality Test (Factory Initialization) ---" << std::endl;
    babel::AudioStreamSystem system;
    if (audioLibrary == "miniaudio")
        system = babel::AudioStreamSystem::MINIAUDIO;
    else if (audioLibrary == "audioport")
        system = babel::AudioStreamSystem::AUDIOPORT;
    else
        return;
    try {
        auto testStream = babel::AudioStreamFactory::create(system);
        babel::OpusCodec codec;
        
        std::vector<std::vector<unsigned char>> recordedPackets;
        std::mutex recordMutex;
        testStream->setOnReadCallback([&](const babel::AudioBuffer& data) {
            std::lock_guard<std::mutex> lock(recordMutex);
            std::vector<unsigned char> encoded = codec.encode(data);
            recordedPackets.push_back(encoded);
        });
        std::cout << "[1/2] Recording for 5 seconds... Speak now!" << std::endl;
        testStream->start();
        for (int i = 5; i > 0; --i) {
            std::cout << "Recording... " << i << "s" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        testStream->stop();
        std::cout << "\n[Wait] Processing " << recordedPackets.size() << " packets..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); 

        if (recordedPackets.empty()) {
            throw std::runtime_error("No audio packets were recorded. Check your microphone.");
        }

        std::cout << "[2/2] Replaying through Opus Decoder..." << std::endl;
        testStream->start();

        for (size_t i = 0; i < recordedPackets.size(); ++i) {
            const auto& packet = recordedPackets[i];
            if (packet.empty()) continue;
            try {
                babel::AudioBuffer decodedData = codec.decode(packet);
                testStream->write(decodedData);
            } catch (const std::exception& e) {
                std::cerr << "Crash at packet " << i << ": " << e.what() << std::endl;
                break;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        testStream->stop();
        std::cout << "--- Test Completed ---" << std::endl;
    } catch (const std::exception& ex) {
        throw std::runtime_error(std::string("Magneto Test Failure: ") + ex.what());
    }
}

int main(int argc, char* argv[])
{
    // 1. CI / Workflow Test
    if (argc == 2 && std::string(argv[1]) == "--test") {
        std::cout << "CI workflow testing..." << std::endl;
        return 0;
    }

    // Audio Quality Test
    if (argc == 3 && std::string(argv[1]) == "--test-audio") {
        try {
            runAudioTest(std::string(argv[2]));
            return 0;
        } catch (const std::exception& ex) {
            std::cerr << "Error: " << ex.what() << std::endl;
            return 84;
        }
    }

    // Normal
    try {
        babel::Client client(babel::TCPSystem::ASIO, babel::UDPSystem::ASIO, babel::AudioStreamSystem::MINIAUDIO, babel::CodecSystem::OPUS);
        while (client._isRunning()) {
            client.run();
        }
    } catch (const std::exception& ex) {
        std::cerr << "Fatal Error: " << ex.what() << std::endl;
        return 84;
    }
    return 0;
}