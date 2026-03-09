/*
** EPITECH PROJECT, 2026
** Untitled (Workspace)
** File description:
** AUDP.hpp
*/

#ifndef AUDP_HPP
#define AUDP_HPP

#include "IUDP.hpp"

namespace babel {
    class AUDPCommunication : public IUDPCommunication {
        public:
            virtual ~AUDPCommunication() = default;

            void setOnDataReceived(std::function<void(const std::vector<unsigned char>&)> cb) override { _onDataReceived = cb; }
            bool isRunning() const override { return _isRunning; }

        protected:
            std::function<void(const std::vector<unsigned char>&)> _onDataReceived;
            bool _isRunning = false;
            uint16_t _localPort = 0;
    };
}

#endif