cmake_minimum_required(VERSION 3.17)
project(babel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Werror)
endif()

find_package(asio REQUIRED)
find_package(portaudio REQUIRED) 
find_package(opus REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(SqliteOrm REQUIRED)

add_subdirectory(protocol)
add_subdirectory(server)
add_subdirectory(client)