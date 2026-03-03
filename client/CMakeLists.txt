add_executable(babel_client 
    src/main.cpp
    src/AudioStream/PortAudioStream.cpp
    include/AudioStream/IAudioStream.hpp
    include/AudioStream/PortAudioStream.hpp
)

if(TARGET portaudio_static)
    set(PA_TARGET portaudio_static)
elseif(TARGET PortAudio::PortAudio)
    set(PA_TARGET PortAudio::PortAudio)
else()
    set(PA_TARGET portaudio)
endif()

target_link_libraries(babel_client PRIVATE 
    Opus::opus 
    asio::asio 
    ${PA_TARGET}
)

if(UNIX)
    target_link_libraries(babel_client PRIVATE 
        asound   # ALSA 
        jack     # JACK
        pthread  # Standard threading
    )
endif()

target_include_directories(babel_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})