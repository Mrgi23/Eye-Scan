#!/usr/bin/env bash

set -euo pipefail

FILES=$(find ./inc ./src \( -name "*.hpp" -o -name "*.cpp" \))
for FILE in $FILES; do
  if ! diff -u "$FILE" <(clang-format "$FILE") > /dev/null; then
    echo "Formatting $FILE"
    clang-format -i "$FILE"
  fi
done
