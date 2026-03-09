add_executable(babel_server
    src/main.cpp
    src/Server.cpp
    src/TCP/TCPAsio.cpp
    include/TCP/ATCP.hpp
    include/TCP/ITCP.hpp
    include/TCP/TCPAsio.hpp
    include/TCP/TCPFactory.hpp
)

target_include_directories(babel_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(babel_server PRIVATE asio::asio)

if(WIN32)
    target_compile_definitions(babel_server PRIVATE _WIN32_WINNT=0x0A00)
    target_link_libraries(babel_server PRIVATE ws2_32 mswsock)
endif()