/*
** EPITECH PROJECT, 2026
** Untitled (Workspace)
** File description:
** tcp_protocol.hpp
*/

#ifndef TCP_PROTOCOL_HPP_
#define TCP_PROTOCOL_HPP_

#include <cstdint>
#include <vector>

#define SUCCESS 0
#define FAILURE 1

namespace babel {

    enum tcp_OpCode : uint8_t {
        CONNECT = 0x01,             // empty
        CONNECT_RESPONSE = 0x02,    // 0 = success, 1 = failure
        REGISTER = 0x03,            // username + password
        REGISTER_RESPONSE = 0x04,   // 0 = success, 1 = failure, 2 = user already registered
        LOGIN = 0x05,               // username + password
        LOGIN_RESPONSE = 0x06,      // 0 = success, 1 = failure, 2 = already logged in
        GET_USERS = 0x07,           // empty
        USERS_LIST = 0x08,          // {user1, user2, ...}
        CALL = 0x09,                // userX
        INCOMING_CALL = 0x0a,       // empty
        ANSWER_CALL = 0x0b,         // 0 = accept, 1 = decline
        CALL_RESULT = 0x0c,         // 0 = call has been accepted, 1 = user not found, 2 = user is in a call already, 3 = user declined, 4 = user calling himself
        START_CALL = 0x0d,          // port & ip
        END_CALL = 0x0e             // empty
    };

    struct Tcp_Header {
        tcp_OpCode op_code;
        uint16_t bodySize;
    };

    struct Tcp_Body {
        std::vector<uint8_t> data;
    };

}

#endif