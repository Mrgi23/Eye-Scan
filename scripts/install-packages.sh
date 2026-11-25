#!/usr/bin/env bash

set -euo pipefail

sudo apt-get update
sudo apt-get install -y \
  build-essential cmake clang-format curl gcc g++ git gpg \
  lcov libavif-dev libcamera-dev libdrm-dev libegl1-mesa-dev libgles2-mesa-dev \
  libjpeg-dev libopenblas-dev libopenjp2-7-dev libopenexr-dev libpng-dev libwebp-dev \
  ninja-build pipx pkg-config qt6-base-dev qt6-tools-dev qt6-wayland

sudo pipx ensurepath
sudo pipx install conan

conan profile detect --force
conan remote add conancenter https://center2.conan.io --force
conan remote add artifactory https://conan.mrgi23.com/artifactory/api/conan/Conan-Index --force
