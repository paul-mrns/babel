@echo off
setlocal

:: Add required Conan remotes
conan remote add conancenter https://center.conan.io || echo Remote already exists
conan remote add bincrafters https://bincrafters.jfrog.io/artifactory/api/conan/public-conan || echo Remote already exists

mkdir build 2>nul
cd build
conan install .. --build=missing
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
echo Build done: babel_server.exe and babel_client.exe