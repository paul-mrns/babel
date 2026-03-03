/*
** EPITECH PROJECT, 2026
** babel
** File description:
** main.cpp
*/

#include <iostream>

#include "../include/AudioStream/PortAudioStream.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>
#include <iomanip>

int main() {
    try {
        babel::PortAudioStream audio;

        audio.setOnReadCallback([&audio](const babel::AudioBuffer& buf) {
             float sum = 0.0f;
            for (float s : buf.samples)
                sum += s * s;
            float rms = std::sqrt(sum / buf.samples.size());

            // Visualization 
            int bars = static_cast<int>(rms * 40);
            std::cout << "\r[";
            for (int i = 0; i < 40; ++i)
                std::cout << (i < bars ? '#' : ' ');
            std::cout << "] " << std::fixed << std::setprecision(4) << rms << "   " << std::flush;

            audio.write(buf);
        });
        audio.start();

        std::cout << "Loopback running, press Enter to stop...\n";
        std::cin.get();
        audio.stop();

    } catch (const std::exception& e) {
        std::cerr << "CRITICAL ERROR: " << e.what() << std::endl;
        return 84;
    }
    return 0;
}
