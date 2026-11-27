#!/usr/bin/env bash

set -euo pipefail

# Collect command-line arguments
WORK_DIR="."
CLEAN=false
REBUILD=false
while [[ "$#" -gt 0 ]]; do
  case "$1" in
    --dir)
      if [[ "$#" -lt 2 ]]; then
        echo "Missing value for --dir"
        exit 1
      fi

      WORK_DIR="$2"
      shift 2
      ;;
    --clean)
      CLEAN=true
      shift
      ;;
    --rebuild)
      REBUILD=true
      shift
      ;;
    *)
      echo "Unknown argument: $1"
      exit 1
      ;;
  esac
done

# Validate mutually exclusive options
if [[ "$CLEAN" == "true" && "$REBUILD" == "true" ]]; then
  echo "Cannot specify both --clean and --rebuild"
  exit 1
fi

# Initialize work directory
pushd "$WORK_DIR" >/dev/null
if [[ "$REBUILD" == "true" ]]; then
  rm -rf build
fi

# Install dependencies with Conan and configure the build with CMake
BUILD_ARGS=(
  --preset
  conan-release
)
if [[ "$REBUILD" == "true" ]] ||
   [[ ! -f build/Release/CMakeCache.txt ]]; then
  conan install . --build=missing
  cmake -G Ninja "${BUILD_ARGS[@]}"
fi

# Build the project with CMake
if [[ "$CLEAN" == "true" ]]; then
  BUILD_ARGS+=(--clean-first)
fi
cmake --build "${BUILD_ARGS[@]}"

# Return to the original directory
popd >/dev/null
