#!/usr/bin/env bash

set -euo pipefail

# Check if sudo is available
if command -v sudo >/dev/null 2>&1; then
  SUDO=sudo
else
  SUDO=
fi

# Update and install required packages
$SUDO apt-get update
$SUDO apt-get install -y \
  build-essential cmake clang-format curl gcc g++ git gpg \
  lcov libavif-dev libcamera-dev libdrm-dev libegl1-mesa-dev libgles2-mesa-dev \
  libjpeg-dev libopenblas-dev libopenjp2-7-dev libopenexr-dev libpng-dev libwebp-dev \
  ninja-build pipx pkg-config qt6-base-dev qt6-tools-dev qt6-wayland

# Install Conan using pipx
pipx ensurepath
if ! command -v conan >/dev/null 2>&1; then
  pipx install conan
else
  pipx install conan --force
fi
export PATH="$HOME/.local/bin:$PATH"

# Configure Conan profiles and remotes
conan profile detect --force
conan remote add conancenter https://center2.conan.io --force
conan remote add artifactory https://conan.mrgi23.com/artifactory/api/conan/Conan-Index --force
