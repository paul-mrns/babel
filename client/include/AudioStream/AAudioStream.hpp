/*
** EPITECH PROJECT, 2026
** Untitled (Workspace)
** File description:
** AAudioStream.hpp
*/

#ifndef AAUDIOSTREAM_HPP_
#define AAUDIOSTREAM_HPP_

#include "IAudioStream.hpp"
#include <mutex>

namespace babel {
    class AAudioStream : public IAudioStream {
        
        public:
            AAudioStream(int sampleRate = DEFAULT_SAMPLE_RATE, int channels = DEFAULT_CHANNELS) 
                : _sampleRate(sampleRate), _channels(channels) {}
    
            void setOnReadCallback(OnRead callback) override
            {
                std::lock_guard<std::mutex> lock(_callbackMutex);
                _onReadCallback = callback;
            }
    
            bool isRunning() const { return _running; }
   
        protected:
            bool _running = false;
            int _sampleRate;
            int _channels;
            OnRead _onReadCallback;
            std::mutex _callbackMutex;

            void notifyRead(const AudioBuffer& data)
            {
                std::lock_guard<std::mutex> lock(_callbackMutex);
                if (_onReadCallback) {
                    _onReadCallback(data);
                }
            }
    };
}

#endif