/*
** EPITECH PROJECT, 2026
** babel
** File description:
** UserSession.hpp
*/

#ifndef USER_SESSION_HPP_
#define USER_SESSION_HPP_

#include <string>
#include <cstdint>

namespace babel {
    class UserSession {

        public:
            UserSession(uint32_t id, std::string ip) 
                : _clientId(id), _ip(std::move(ip)), _isInCall(false), _isLoggedIn(false) {}

            uint32_t getId() const { return _clientId; }
            std::string getIp() const { return _ip; }
            bool isInCall() const { return _isInCall; }
            bool isLoggedIn() const { return _isLoggedIn; }
            const std::string& getUsername() const { return _username; }

            void setInCall(bool state) { _isInCall = state; }
            void setCredentials(const std::string username, const std::string password) 
            {
                _username = username;
                _password = password;
                _isLoggedIn = true;
            }

            void logout() { _isLoggedIn = false; _username = ""; }

            bool isValidPassword(const std::string& password) { return _password == password; };

        private:
            uint32_t _clientId;
            std::string _ip;
            std::string _username;
            std::string _password;
            bool _isInCall;
            bool _isLoggedIn;
    };
}

#endif