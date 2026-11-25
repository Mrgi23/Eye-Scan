#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(pwd)"

cleanup() {
  cd "$ROOT_DIR"
}

CLEAN=false
COVERAGE=false
for arg in "$@"; do
  case "$arg" in
    clean)
      CLEAN=true
      ;;
    coverage)
      COVERAGE=true
      ;;
  *)
    echo "Unknown argument: $arg"
    exit 1
    ;;
  esac
done

cd tests

if [[ "$CLEAN" == "true" ]]; then
  rm -rf build
fi

conan install . --build=missing

cmake -G Ninja --preset conan-release
cmake --build --preset conan-release

cd build/Release
if [[ "$COVERAGE" == "true" ]]; then
  ninja coverage
else
  ./tests
fi
