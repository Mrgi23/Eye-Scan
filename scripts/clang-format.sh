#!/usr/bin/env bash

set -euo pipefail

FILES=()

if [ "$#" -gt 0 ]; then
  for ARG in "$@"; do
    if [ -d "$ARG" ]; then
      while IFS= read -r FILE; do
        FILES+=("$FILE")
      done < <(
        find "$ARG" \( -name "*.hpp" -o -name "*.cpp" \)
      )
    elif [ -f "$ARG" ]; then
      FILES+=("$ARG")
    else
      echo "Warning: '$ARG' does not exist"
    fi
  done
else
  while IFS= read -r FILE; do
    FILES+=("$FILE")
  done < <(
    find ./inc ./src \( -name "*.hpp" -o -name "*.cpp" \)
  )
fi

for FILE in "${FILES[@]}"; do
  if ! cmp -s "$FILE" <(clang-format "$FILE"); then
    echo "Formatting $FILE"
    clang-format -i "$FILE"
  fi
done
