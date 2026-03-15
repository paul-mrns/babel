/*
** EPITECH PROJECT, 2026
** Untitled (Workspace)
** File description:
** OpusCodec.cpp
*/

#include "../../include/Codec/OpusCodec.hpp"

namespace babel {

OpusCodec::OpusCodec(int sampleRate, int channels, int bitrate)
    : ACodec(sampleRate, channels, bitrate)
{
    int err;

    _encoder = opus_encoder_create(_sampleRate, _channels, OPUS_APPLICATION_VOIP, &err);
    if (err != OPUS_OK)
        throw std::runtime_error("[Opus Codec] Failed to create encoder");

    _decoder = opus_decoder_create(_sampleRate, _channels, &err);
    if (err != OPUS_OK)
        throw std::runtime_error("[Opus Codec] Failed to create decoder");

    opus_encoder_ctl(_encoder, OPUS_SET_BITRATE(_bitrate));
    opus_encoder_ctl(_encoder, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE)); // Optimized voice
    opus_encoder_ctl(_encoder, OPUS_SET_COMPLEXITY(10));            // Optimized CPU
    opus_encoder_ctl(_encoder, OPUS_SET_INBAND_FEC(1));
    std::cout << "[Opus Codec] has been initialised" << std::endl;
}

OpusCodec::~OpusCodec()
{
    if (_encoder)
        opus_encoder_destroy(_encoder);
    if (_decoder)
        opus_decoder_destroy(_decoder);
}

std::vector<uint8_t> OpusCodec::encode(const AudioBuffer& input)
{
    int frameSizePerChannel = static_cast<int>(input.samples.size() / _channels);

    std::vector<uint8_t> output(OPUS_MAX_PACKET_SIZE);

    opus_int32 nbBytes = opus_encode_float(
        _encoder, 
        input.samples.data(), 
        frameSizePerChannel,
        output.data(), 
        static_cast<opus_int32>(output.size())
    );

    if (nbBytes < 0)
        throw std::runtime_error("[Opus Codec] Encoding error: " + std::to_string(nbBytes));
    
    output.resize(nbBytes);
    return output;
}

AudioBuffer OpusCodec::decode(const std::vector<uint8_t>& input)
{
    if (input.empty()) return {};

    AudioBuffer output;
    output.sampleRate = _sampleRate;
    output.channels = _channels;
    const int maxFrameSize = 960; 
    output.samples.resize(maxFrameSize * _channels);

    int frameCount = opus_decode_float(
        _decoder, 
        input.data(), 
        static_cast<opus_int32>(input.size()), 
        output.samples.data(), 
        maxFrameSize,
        0
    );
    if (frameCount < 0) {
        std::cerr << "[Opus] Decode Error Code: " << frameCount << std::endl;
        throw std::runtime_error("[Opus Codec] Decoding error");
    }
    output.samples.resize(frameCount * _channels);
    return output;
}

}