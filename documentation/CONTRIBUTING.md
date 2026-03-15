# Developer Documentation

## Overview

Babel is a modular VoIP platform built in modern C++. Its architecture separates audio processing, codec management, networking, and command‑line interaction into cleanly defined modules. This documentation explains how the system is organized, how components interact, and how to extend or debug the project.

## Project Structure and Responsibilities

Babel is divided into several modules, each with a clear responsibility.
- AudioStream (IAudioStream and AAudioStream), handles audio capture, playback, buffering, and codec integration.
- Codec (ICodec and ACodec), Encodes and decodes audio frames. New codecs must implement the codec interface.
- Network (IUDPCommunication/AUDPCommunication and ITCPCommunication/ATCPCommunication), TCP signaling and UDP audio transport.
- Shell (Client.hpp/Client.cpp), Provides the command-line interface for interacting with the server.
- Server (Server.hpp/Server.cpp), Manages client sessions, authentication, and call signaling.

Understanding this structure helps contributors place new code in the correct module and maintain separation of concerns.

## Coding Guidelines

-Use modern C++ (C++17 or later).
- Avoid blocking operations in real-time audio callbacks.
- Keep networking non-blocking and event-driven.
- Use std::unique_ptr and std::shared_ptr appropriately.
- Keep modules independent and interface-driven.
- Follow existing naming conventions and file organization.
- Write clear, self-contained commits.
- use branches and pull requests for developpment

## Commit Messages

Commit messages should be descriptive and follow a consistent style.  
``` feat:``` for new features  
``` fix:``` for bug fixes and small improvements  
``` del:``` for files deleting  
``` hotfix:``` small and important fixes  


## New dependencies

To add new libraries, make sure to:
- add the dependency to the conanfile.txt at the root of the repo
- modify CMakeFiles depending on the part that needs it (root/, client/ or server/)
- make sure that the project runs on Windows and Linux (checks with CI jobs)
- verify headers
- add the library implementation to the corresponding section
    - Client: AudioStream/, Codec/ , TCP/ ,  UDP/
    - Server: TCP/
- add the dependencies on the designed Interface and Abstract classes

## Contact

Don't hesitate to ask questions at:
- paul.mourens@epitech.eu