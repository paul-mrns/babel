@echo off
setlocal

:: 1. Setup Virtual Environment
if not exist .venv (
    echo [LOG] Creating Python Virtual Environment...
    python -m venv .venv
)
call .venv\Scripts\activate.bat

:: 2. Ensure Conan is installed
echo [LOG] Installing/Updating Conan...
python -m pip install conan --quiet

:: 3. Built-in Conan Detection/Creation
echo [LOG] Detecting System Profile...
if exist %USERPROFILE%\.conan2\profiles\default (
    del /f %USERPROFILE%\.conan2\profiles\default
)
conan profile detect --force

:: 4. Prep Build Directory
if not exist build mkdir build

:: 5. Conan Install (Fixes "Dependencies Failing")
echo [LOG] Fetching Dependencies...
conan install . --output-folder=build --build=missing -pr:b=default -pr:h=default -s build_type=Release

:: 6. CMake Configuration
echo [LOG] Configuring CMake for VS 2026...
cmake -S . -B build -G "Visual Studio 18 2026" ^
    -DCMAKE_TOOLCHAIN_FILE="build/build/generators/conan_toolchain.cmake"

:: 7. Compilation
echo [LOG] Compiling Binaries...
cmake --build build --config Release

echo [LOG] Build process complete.
pause