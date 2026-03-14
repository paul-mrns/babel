/*
** EPITECH PROJECT, 2026
** Untitled (Workspace)
** File description:
** tcp_protocol.hpp
*/

#ifndef UDP_PROTOCOL_HPP_
#define UDP_PROTOCOL_HPP_

#include <vector>
#include <cstdint>

namespace babel {
    namespace babel {
        
    struct Udp_Header {
        uint32_t timestamp;
        uint16_t bodySize; 
    };

    struct Udp_Packet {
        Udp_Header header;
        uint8_t payload[1024]; 
    };
}
}

#endif