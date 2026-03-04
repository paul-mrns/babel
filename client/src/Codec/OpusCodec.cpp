/*
** EPITECH PROJECT, 2026
** Untitled (Workspace)
** File description:
** OpusCodec.cpp
*/

#include "../../include/Codec/OpusCodec.hpp"
#include <stdexcept>

namespace babel {

OpusCodec::OpusCodec(int sampleRate, int channels, int bitrate)
    : ACodec(sampleRate, channels, bitrate)
{
    int err;

    _encoder = opus_encoder_create(_sampleRate, _channels, OPUS_APPLICATION_VOIP, &err);
    if (err != OPUS_OK)
        throw std::runtime_error("Opus: Failed to create encoder");

    _decoder = opus_decoder_create(_sampleRate, _channels, &err);
    if (err != OPUS_OK)
        throw std::runtime_error("Opus: Failed to create decoder");

    opus_encoder_ctl(_encoder, OPUS_SET_BITRATE(_bitrate));
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
    std::vector<uint8_t> output(OPUS_MAX_PACKET_SIZE);

    opus_int32 nbBytes = opus_encode_float(
        _encoder, 
        input.samples.data(), 
        _frameSize, 
        output.data(), 
        static_cast<opus_int32>(output.size())
    );

    if (nbBytes < 0)
        throw std::runtime_error("Opus: Encoding error");
    output.resize(nbBytes);
    return output;
}

AudioBuffer OpusCodec::decode(const std::vector<uint8_t>& input)
{
    AudioBuffer output;
    output.sampleRate = _sampleRate;
    output.channels = _channels;
    output.samples.resize(_frameSize * _channels);

    int frameCount = opus_decode_float(
        _decoder, 
        input.data(), 
        static_cast<opus_int32>(input.size()), 
        output.samples.data(), 
        _frameSize, 
        0
    );

    if (frameCount < 0)
        throw std::runtime_error("Opus: Decoding error");
    return output;
}

} // namespace babel