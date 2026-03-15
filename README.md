# Babel

A modular VoIP communication platform inspired by Skype, built in modern C++. Babel provides real‑time audio streaming, encoding/decoding, UDP/TCP networking, and a command‑driven shell environment for interacting with a central server. The project is designed to be lightweight, extensible, and suitable for learning or building real‑time communication systems.

## Project goals

- Real‑time voice communication between clients
- Cross‑platform audio capture and playback
- Efficient audio compression using pluggable codecs
- Low‑latency UDP transport for audio
- Reliable TCP transport for signaling and session management
- A shell‑based interface for interacting with the server
- Clean, modular architecture for easy extension

## Project structure

- Audio Engine (AudioPort, Miniaudio) : capture and play sound, duplex streaming, internal buffer, callback-driven, Codec ready
- Codec (Opus) : PCM (baseline), Optional integration with Opus or other codecs
- Network (Asio) : UDP (peer to peer) for audio streaming, TCP (Client-Server) for shell interface
- Shell Interface : user commands, connecting to server, login/registering, calling, connected user list

## Dependencies

- C++17 or later
- CMake (build system)
- Python3 (python virtual environnement)
- Conan (dependencies handling)
- Visual Studio (Required for Windows Users, preferably latest 18.0 2026)

## Getting started

### Compilation/Building

On Linux 
```
./build.sh
```

On Windows
```
.\build.bat
```

### Start the project

Linux: 
```
./build/client/babel_client
```
OR

```
./build/server/babel_server
```

---

Windows:
```
.\build\build\client\Release\babel_client.exe
```
OR
```
.\build\build\server\Release\babel_server.exe
```

### Using the shell
type ```HELP``` for list of available commands and ```EXIT``` to exit babel

## Code Structure

```tree
├── build.bat
├── build.sh
├── conanfile.txt
├── CMakeLists.txt
├── CMakeUserPresets.json
├── README.md
├── communication
│   └── TCPProtocol.hpp
├── documentation
│   ├── CONTRIBUTING.md.md
│   ├── tcp_protocol.md
│   └── udp_protocol.md
├── client
│   ├── CMakeLists.txt
│   ├── include
│   │   ├── AudioStream
│   │   │   ├── AAudioStream.hpp
│   │   │   ├── AudioPortStream.hpp
│   │   │   ├── AudioStreamFactory.hpp
│   │   │   ├── IAudioStream.hpp
│   │   │   └── MiniAudioStream.hpp
│   │   ├── Client.hpp
│   │   ├── Codec
│   │   │   ├── ACodec.hpp
│   │   │   ├── CodecFactory.hpp
│   │   │   ├── ICodec.hpp
│   │   │   └── OpusCodec.hpp
│   │   ├── TCP
│   │   │   ├── ATCP.hpp
│   │   │   ├── ITCP.hpp
│   │   │   ├── TCPAsio.hpp
│   │   │   └── TCPFactory.hpp
│   │   └── UDP
│   │       ├── AsioUDP.hpp
│   │       ├── AUDP.hpp
│   │       ├── IUDP.hpp
│   │       └── UDPFactory.hpp
│   └── src
│       ├── AudioStream
│       │   ├── AudioPortStream.cpp
│       │   └── MiniAudioStream.cpp
│       ├── ClientCommands.cpp
│       ├── Client.cpp
│       ├── Codec
│       │   └── OpusCodec.cpp
│       ├── main.cpp
│       ├── TCP
│       │   └── TCPAsio.cpp
│       └── UDP
│           └── AsioUDP.cpp
└── server
    ├── CMakeLists.txt
    ├── include
    │   ├── Server.hpp
    │   ├── TCP
    │   │   ├── ATCP.hpp
    │   │   ├── ITCP.hpp
    │   │   ├── TCPAsio.hpp
    │   │   └── TCPFactory.hpp
    │   └── UserSession.hpp
    └── src
        ├── main.cpp
        ├── Server.cpp
        └── TCP
            └── TCPAsio.cpp
```

## Contributing

Contributions are welcome. Please open issues or submit pull requests.  

## Thank you for checking out Babel Project :p