#!/bin/bash
set -e
mkdir -p build

# 1. Python Virtual env 
if [ ! -d ".venv" ]; then
    echo "[build.sh] Creating isolated Python environment (.venv)..."
    python3 -m venv .venv
fi

# 2. Activate venv
echo "[build.sh] Activating virtual environment..."
source .venv/bin/activate

# 3. Ensure Conan is installed 
echo "[build.sh] Updating Conan and fixing remotes..."
pip install --upgrade pip conan --quiet

# Remove dead remotes and point to the modern Conan Center
conan remote remove bincrafters 2>/dev/null || true
conan remote update conancenter --url="https://center2.conan.io" 2>/dev/null || \
conan remote add conancenter "https://center2.conan.io" 2>/dev/null || true

# 4. Detect Conan Profile (if not already present)
if [ ! -f ~/.conan2/profiles/default ]; then
    echo "[build.sh] No default Conan profile found. Detecting now..."
    conan profile detect --force
fi

# 5. Run Conan Install
echo "[build.sh] Installing dependencies (Asio, PortAudio, Opus)..."
conan install . --output-folder=build --build=portaudio/19.7 -s build_type=Release -c tools.files.download:verify=False -o "portaudio*:with_jack=False" -o "portaudio*:with_alsa=True"

# 6. Dynamically locate the Toolchain file
echo "[build.sh] Searching for conan_toolchain.cmake..."
TOOLCHAIN_FILE=$(find build -name "conan_toolchain.cmake" | head -n 1)

if [ -z "$TOOLCHAIN_FILE" ]; then
    echo "ERROR: Could not find conan_toolchain.cmake."
    exit 1
fi
echo "[build.sh] Found Toolchain at: $TOOLCHAIN_FILE"

# 7. Configure CMake
echo "[build.sh] Configuring CMake Project..."
cmake -S . -B build \
    -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" \
    -DCMAKE_BUILD_TYPE=Release

# 8. Compilation
echo "[build.sh] Starting Compilation..."
cmake --build build --config Release
