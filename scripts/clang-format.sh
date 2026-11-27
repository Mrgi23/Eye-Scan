#!/usr/bin/env bash

set -euo pipefail

# Set the mode for clang-format (format or check)
MODE=format
if [[ "${1:-}" == "--check" ]]; then
  MODE=check
  shift
fi

# Find all .hpp and .cpp files to process
FILES=()
if [[ "$#" -gt 0 ]]; then
  for ARG in "$@"; do
    if [[ -d "$ARG" ]]; then
      while IFS= read -r FILE; do
        FILES+=("$FILE")
      done < <(
        find "$ARG" \( -name "*.hpp" -o -name "*.cpp" \)
      )
    elif [[ -f "$ARG" ]]; then
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

# Check/format files based on the mode
FAILED=0
for FILE in "${FILES[@]}"; do
  if ! cmp -s "$FILE" <(clang-format "$FILE"); then
    if [[ "$MODE" == "check" ]]; then
      echo "File $FILE is not properly formatted"
      FAILED=1
    else
      echo "Formatting $FILE"
      clang-format -i "$FILE"
    fi
  fi
done

exit "$FAILED"
