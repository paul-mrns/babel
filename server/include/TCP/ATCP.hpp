
#ifndef ATCP_HPP_
#define ATCP_HPP_

#include "ITCP.hpp"

namespace babel {
    class ATCPCommunication : public ITCPCommunication {
    
        public:
            void onClientConnected(std::function<void(uint32_t)> callback) override
            {
                _onClientConnected = callback;
            }
        
            void onClientDisconnected(std::function<void(uint32_t)> callback) override
            {
                _onClientDisconnected = callback;
            }
        
            void onMessage(std::function<void(uint32_t, Tcp_Header, std::vector<uint8_t>)> callback) override
            {
                _onMessage = callback;
            }

        protected:
            std::function<void(uint32_t)> _onClientConnected;
            std::function<void(uint32_t)> _onClientDisconnected;
            std::function<void(uint32_t, Tcp_Header, std::vector<uint8_t>)> _onMessage;
    };
}

#endif