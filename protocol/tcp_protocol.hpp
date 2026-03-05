/*
** EPITECH PROJECT, 2026
** Untitled (Workspace)
** File description:
** tcp_protocol.hpp
*/

#ifndef PROTOCOL_HPP_
#define PROTOCOL_HPP_

#include <cstdint>
#include <vector>

namespace babel {

    enum tcp_OpCode : uint8_t {
        SIGNIN = 0x01,              // username + password
        SIGNIN_RESPONSE = 0x02,     // 0 = success, 0 = failure
        LOGIN = 0x03,               // username + password
        LOGIN_RESPONSE = 0x04,      // 0 = success, 1 = failure
        GET_USERS = 0x05,           // empty
        USERS_LIST = 0x06,          // {user1, user2, ...}
        CALL = 0x07,                // userX
        ACCEPT_CALL = 0x08,         // empty
        DECLINE_CALL = 0x09,        // empty
        CALL_RESPONSE = 0x0A,       // 0 = success, 1 = user not found, 2 = user busy
        END_CALL = 0x0B,            // empty
    };

    struct Tcp_Header {
        tcp_OpCode op_code;
        uint16_t bodySize;
    };

    struct Tcp_Body {
        std::vector<uint8_t> data;
    };

    struct Tcp_Packet {
        Tcp_Header header;
        Tcp_Body body;
    };

}

#endif