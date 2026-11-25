#!/usr/bin/env bash

set -euo pipefail

# Collect command-line arguments
OUTPUT_DIR="coverage"
FORMAT=""
COVERAGE=false
while [[ "$#" -gt 0 ]]; do
  case "$1" in
    --output-dir)
      if [[ "$#" -lt 2 ]]; then
        echo "Missing value for --output-dir"
        exit 1
      fi

      OUTPUT_DIR="$2"
      shift 2
      ;;
    --format)
      if [[ "$#" -lt 2 ]]; then
        echo "Missing value for --format"
        exit 1
      fi

      FORMAT="$2"
      if [[ "$FORMAT" != "xml" && "$FORMAT" != "json" ]]; then
        echo "Unsupported format: $FORMAT"
        exit 1
      fi

      shift 2
      ;;
    --coverage)
      COVERAGE=true
      shift
      ;;
    *)
      echo "Unknown argument: $1"
      exit 1
      ;;
  esac
done

# Initialize output directory
mkdir -p "$OUTPUT_DIR"
OUTPUT_DIR="$(realpath "$OUTPUT_DIR")"

# Run tests from the build directory
ROOT_DIR="$(pwd)"
pushd tests/build/Release >/dev/null
if [[ -n "$FORMAT" ]]; then
  ./tests --gtest_output="${FORMAT}:${OUTPUT_DIR}/results.${FORMAT}"
else
  ./tests
fi

# Capture coverage data and generate coverage reports
if [[ "$COVERAGE" == true ]]; then
  lcov --capture --directory . -o raw.info \
    --ignore-errors inconsistent,inconsistent --ignore-errors mismatch,mismatch \
    --rc geninfo_unexecuted_blocks=1 > /dev/null
  lcov -e raw.info "${ROOT_DIR}/inc/*" "${ROOT_DIR}/src/*" -o "${OUTPUT_DIR}/coverage.info" \
    --ignore-errors unused,unused > /dev/null
  sed -i "/D0Ev/d" "${OUTPUT_DIR}/coverage.info"
  genhtml "${OUTPUT_DIR}/coverage.info" -o "${OUTPUT_DIR}"
fi

# Return to the original directory
popd >/dev/null
