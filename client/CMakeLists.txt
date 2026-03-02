add_executable(babel_client
    src/main.cpp
)

target_include_directories(babel_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(babel_client
    babel_protocol
)

if (TARGET CONAN_PKG::asio)
    target_link_libraries(babel_client CONAN_PKG::asio)
endif()

if (TARGET CONAN_PKG::portaudio)
    target_link_libraries(babel_client CONAN_PKG::portaudio)
endif()

if (TARGET CONAN_PKG::opus)
    target_link_libraries(babel_client CONAN_PKG::opus)
endif()

if (WIN32)
    target_link_libraries(babel_client ws2_32)
endif()
