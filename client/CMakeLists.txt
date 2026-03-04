add_executable(babel_client 
    src/main.cpp
    src/AudioStream/PortAudioStream.cpp
    src/Codec/OpusCodec.cpp
    include/AudioStream/IAudioStream.hpp
    include/AudioStream/AAudioStream.hpp
    include/AudioStream/PortAudioStream.hpp
    include/Codec/ICodec.hpp
    include/Codec/ACodec.hpp
    include/Codec/OpusCodec.hpp
)

if(TARGET portaudio_static)
    set(PA_TARGET portaudio_static)
elseif(TARGET PortAudio::PortAudio)
    set(PA_TARGET PortAudio::PortAudio)
else()
    set(PA_TARGET portaudio)
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(babel_client PRIVATE 
        Opus::opus 
        asio::asio 
        -Wl,--start-group 
        ${PA_TARGET}
        asound              # ALSA
        jack                # JACK
        pthread
        m                   # Math 
        -Wl,--end-group
    )
    target_link_options(babel_client PRIVATE "-Wl,--no-as-needed")
else()
    target_link_libraries(babel_client PRIVATE 
        Opus::opus 
        asio::asio 
        ${PA_TARGET}
    )
endif()

target_include_directories(babel_client PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)