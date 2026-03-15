# **UDP PROTOCOL**

## 1. AudioBuffer Packet structure

- samples (float vector): 480 samples for a 10ms audio
- sample rate (Integer): set by default at 48000hz for resolution
- channel (Integer 1 or 2): Mono/Stereo channel

## 2. Packet life-cycle

```mermaid
sequenceDiagram
    participant HW as Hardware (Mic/Speaker)
    participant AS as AudioStream (PortAudio/Miniaudio)
    participant C as Codec (Opus)
    participant N as UDP Socket (Asio)

    Note over HW, N: [SENDER SIDE]
    HW->>AS: Analog Sound -> Samples (PCM)
    AS->>AS: fills AudioBuffer (480 samples)
    AS->>C: Passes AudioBuffer to Codec
    C->>C: Encode (Float -> uint8_t)
    C->>N: Send Codec buffer through UDP

    Note over HW, N: [RECEIVER SIDE]
    N->>C: Receives buffer and passes it
    C->>C: Decode (uint8_t -> Float)
    C->>AS: write(AudioBuffer)
    AS->>HW: Samples -> Analog Sound
```

