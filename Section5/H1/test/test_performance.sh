#!/bin/bash
set -euo pipefail

# ============================================================
# Performance & Validation Test - RLE
# Estructura esperada:
#   /build -> ejecutable(s) generados por el estudiante
#   /src
#   /test -> este script
#
# No se califica la compilación.
# Se ejecuta el PRIMER ejecutable encontrado en ../build.
# ============================================================

# --- Colors ---
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
NC='\033[0m'

BUILD_DIR="../build"
LOG_FILE="rle_test.log"

INPUT_FILE="original_data.bin"
COMPRESSED_FILE="compressed_data.rle"
DECOMPRESSED_FILE="decompressed_data.bin"

# --- Puntuación ---
FINAL_SCORE=0
MAX_SCORE=100

P1=20   # original
P2=20   # comprimido
P3=20   # descomprimido
P4=30   # contenido igual
P5=10   # compresión efectiva

# --- helper size (Linux/mac) ---
get_size() {
  local f="$1"
  if stat -c%s "$f" 2>/dev/null; then
    return 0
  else
    stat -f%z "$f"
  fi
}

# --- limpiar salidas previas ---
rm -f "$INPUT_FILE" "$COMPRESSED_FILE" "$DECOMPRESSED_FILE" "$LOG_FILE"

echo "--- Searching executable in $BUILD_DIR ---"

if [ ! -d "$BUILD_DIR" ]; then
  echo -e "${RED}Build dir $BUILD_DIR not found.${NC}"
  echo -e "Final Grade: ${YELLOW}0${NC} / ${MAX_SCORE}"
  exit 1
fi

# buscamos ejecutables (archivos con permiso de ejecución, no directorios)
EXEC=""
while IFS= read -r path; do
  EXEC="$path"
  break
done < <(find "$BUILD_DIR" -maxdepth 1 -type f -perm -111)

if [ -z "$EXEC" ]; then
  echo -e "${RED}No executable found in $BUILD_DIR.${NC}"
  echo -e "Final Grade: ${YELLOW}0${NC} / ${MAX_SCORE}"
  exit 1
fi

echo -e "${GREEN}Using executable:${NC} $EXEC"
echo

echo "--- Running program (from test/) ---"
if ! "$EXEC" > "$LOG_FILE" 2>&1; then
  echo -e "${YELLOW}Program exited with non-zero status. Will still grade outputs.${NC}"
fi

echo
echo "--- Checking outputs ---"

# 1) original_data.bin
if [ -f "$INPUT_FILE" ]; then
  SIZE_ORIG=$(get_size "$INPUT_FILE")
  if [ "$SIZE_ORIG" -gt 0 ]; then
    echo -e "${GREEN}[OK]${NC} $INPUT_FILE generated (${SIZE_ORIG} bytes)"
    FINAL_SCORE=$((FINAL_SCORE + P1))
  else
    echo -e "${RED}[FAIL]${NC} $INPUT_FILE exists but is empty"
  fi
else
  echo -e "${RED}[FAIL]${NC} $INPUT_FILE was NOT generated"
fi

# 2) compressed_data.rle
if [ -f "$COMPRESSED_FILE" ]; then
  SIZE_COMP=$(get_size "$COMPRESSED_FILE")
  if [ "$SIZE_COMP" -gt 0 ]; then
    echo -e "${GREEN}[OK]${NC} $COMPRESSED_FILE generated (${SIZE_COMP} bytes)"
    FINAL_SCORE=$((FINAL_SCORE + P2))
  else
    echo -e "${RED}[FAIL]${NC} $COMPRESSED_FILE exists but is empty"
  fi
else
  echo -e "${RED}[FAIL]${NC} $COMPRESSED_FILE was NOT generated"
fi

# 3) decompressed_data.bin
if [ -f "$DECOMPRESSED_FILE" ]; then
  SIZE_DECOMP=$(get_size "$DECOMPRESSED_FILE")
  if [ "$SIZE_DECOMP" -gt 0 ]; then
    echo -e "${GREEN}[OK]${NC} $DECOMPRESSED_FILE generated (${SIZE_DECOMP} bytes)"
    FINAL_SCORE=$((FINAL_SCORE + P3))
  else
    echo -e "${RED}[FAIL]${NC} $DECOMPRESSED_FILE exists but is empty"
  fi
else
  echo -e "${RED}[FAIL]${NC} $DECOMPRESSED_FILE was NOT generated"
fi

# 4) contenido igual
if [ -f "$INPUT_FILE" ] && [ -f "$DECOMPRESSED_FILE" ]; then
  if cmp -s "$INPUT_FILE" "$DECOMPRESSED_FILE"; then
    echo -e "${GREEN}[OK]${NC} Decompressed content matches original"
    FINAL_SCORE=$((FINAL_SCORE + P4))
  else
    echo -e "${RED}[FAIL]${NC} Decompressed content DOES NOT match original"
  fi
else
  echo -e "${YELLOW}[SKIP]${NC} Content check skipped (missing files)"
fi

# 5) compresión efectiva
if [ -f "$INPUT_FILE" ] && [ -f "$COMPRESSED_FILE" ]; then
  if [ "$SIZE_COMP" -lt "$SIZE_ORIG" ]; then
    echo -e "${GREEN}[OK]${NC} Compression effective: $SIZE_COMP < $SIZE_ORIG"
    FINAL_SCORE=$((FINAL_SCORE + P5))
  else
    echo -e "${YELLOW}[WARN]${NC} Compression not effective.${NC}"
  fi
else
  echo -e "${YELLOW}[SKIP]${NC} Compression ratio check skipped (missing files)"
fi

echo
echo "-----------------------------"
echo -e "Final Grade: ${YELLOW}${FINAL_SCORE}${NC} / ${MAX_SCORE}"
echo "Log saved to: $LOG_FILE"
echo "-----------------------------"
