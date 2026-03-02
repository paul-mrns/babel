#!/bin/bash
set -e

# Add required Conan remotes
conan remote add conancenter https://center.conan.io --force || true
conan remote add bincrafters https://bincrafters.jfrog.io/artifactory/api/conan/public-conan --force || true

mkdir -p build
cd build
conan install .. --build=missing
cmake .. -G "Unix Makefiles"
cmake --build .
echo "Build successful: babel_server and babel_client"