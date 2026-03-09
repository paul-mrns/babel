@echo off
setlocal

if not exist .venv (
    echo [LOG] Creating Python Virtual Environment...
    python -m venv .venv
)
call .venv\Scripts\activate.bat

echo [LOG] Installing/Updating Conan...
python -m pip install conan --quiet

echo [LOG] Detecting System Profile...
if exist %USERPROFILE%\.conan2\profiles\default (
    del /f %USERPROFILE%\.conan2\profiles\default
)
conan profile detect --force

if not exist build mkdir build

echo [LOG] Fetching Dependencies...
conan install . --output-folder=build --build=missing -pr:b=default -pr:h=default -s build_type=Release

echo [LOG] Configuring CMake...
cmake --preset conan-default

echo [LOG] Compiling Binaries...
cmake --build --preset conan-default --config Release

echo [LOG] Build process complete.
pause