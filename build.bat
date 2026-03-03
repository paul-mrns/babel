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

:: 3. Prep Build Directory
if not exist build mkdir build

:: 4. Conan Install
echo [LOG] Fetching Dependencies...
conan install . --output-folder=build --build=missing -s build_type=Release

:: 5. CMake Configuration
echo [LOG] Configuring CMake...
cmake -S . -B build -G "Visual Studio 18 2026" ^
    -DCMAKE_TOOLCHAIN_FILE="build/build/generators/conan_toolchain.cmake"

:: 6. Compilation
echo [LOG] Compiling Binaries...
cmake --build build --config Release

echo [LOG] Build process complete.
pause