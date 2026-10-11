#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD_DIR="${ROOT}/build/switch"
OUT_DIR="${ROOT}/dist/switch/switch/supertux2"

if [ ! -f "${BUILD_DIR}/supertux2.nro" ]; then
  echo "NRO mancante: esegui prima ports/switch/build.sh" >&2
  exit 1
fi

mkdir -p "${OUT_DIR}"
cp "${BUILD_DIR}/supertux2.nro" "${OUT_DIR}/supertux2.nro"

cat > "${OUT_DIR}/LEGGIMI.txt" <<'TXT'
Installazione su Switch:
1. Copia la cartella supertux2 in sdmc:/switch/
2. Avvia supertux2.nro da Homebrew Menu. Tieni premuto R su un gioco per
   avviarlo con tutta la memoria disponibile.

I dati del gioco sono incorporati nel NRO, non serve copiare altro.
Salvataggi e log finiscono in sdmc:/switch/supertux2/.
TXT

echo "Pacchetto Switch: ${OUT_DIR}"
