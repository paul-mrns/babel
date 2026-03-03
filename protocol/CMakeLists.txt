add_library(babel_protocol INTERFACE)

target_include_directories(babel_protocol INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if (TARGET CONAN_PKG::asio)
    target_link_libraries(babel_protocol INTERFACE CONAN_PKG::asio)
endif()