cmake_minimum_required(VERSION 3.17)
project(babel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Conan setup (guarded: present when Conan has generated it)
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)
endif()

if (MSVC)                                       # Windows
    add_compile_options(/W4 /WX)
else()                                          # Linux/macOS
    add_compile_options(-Wall -Wextra -Werror)
endif()

add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(protocol)