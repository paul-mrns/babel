add_executable(babel_server
    src/main.cpp
)

target_include_directories(babel_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(babel_server
    babel_protocol
)

if (TARGET CONAN_PKG::asio)
    target_link_libraries(babel_server CONAN_PKG::asio)
endif()

# Windows: link against ws2_32 for Winsock
if (WIN32)
    target_link_libraries(babel_server ws2_32)
endif()
