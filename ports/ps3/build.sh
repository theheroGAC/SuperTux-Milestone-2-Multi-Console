#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD_DIR="${ROOT}/build/ps3"
JOBS="${JOBS:-2}"

if [ -z "${PS3DEV:-}" ]; then
  if [ -d "/usr/local/ps3dev" ]; then
    export PS3DEV="/usr/local/ps3dev"
  elif [ -d "${ROOT}/tools/ps3dev" ]; then
    export PS3DEV="${ROOT}/tools/ps3dev"
  fi
fi

if [ -z "${PSL1GHT:-}" ]; then
  export PSL1GHT="${PS3DEV:-/usr/local/ps3dev}"
fi

if [ -n "${PS3DEV:-}" ]; then
  export PATH="${PS3DEV}/bin:${PS3DEV}/ppu/bin:${PATH}"
fi

"${ROOT}/tools/brand_logo.sh" ps3

if command -v powerpc64-ps3-elf-gcc >/dev/null 2>&1 || command -v ppu-lv2-gcc >/dev/null 2>&1; then
  cmake -B "${BUILD_DIR}" -S "${ROOT}/upstream" -G "Unix Makefiles" \
    -DCMAKE_TOOLCHAIN_FILE="${ROOT}/upstream/cmake/toolchains/PS3.cmake" \
    -DCMAKE_BUILD_TYPE=Release
  cmake --build "${BUILD_DIR}" -j "${JOBS}"
elif command -v wsl >/dev/null 2>&1; then
  WSL_ROOT="/mnt/d/SuperTux2-MultiPort"
  wsl -e bash -lc "
    set -e
    export PS3DEV=\${PS3DEV:-/usr/local/ps3dev}
    if [ ! -d \"\$PS3DEV\" ] && [ -d \"${WSL_ROOT}/tools/ps3dev\" ]; then
      export PS3DEV=\"${WSL_ROOT}/tools/ps3dev\"
    fi
    export PSL1GHT=\${PSL1GHT:-\$PS3DEV}
    export PATH=\"\${PS3DEV}/bin:\${PS3DEV}/ppu/bin:\${PATH}\"
    cmake -B \"${WSL_ROOT}/build/ps3\" -S \"${WSL_ROOT}/upstream\" -G 'Unix Makefiles' \
      -DCMAKE_TOOLCHAIN_FILE=\"${WSL_ROOT}/upstream/cmake/toolchains/PS3.cmake\" \
      -DCMAKE_BUILD_TYPE=Release
    cmake --build \"${WSL_ROOT}/build/ps3\" -j ${JOBS}
  "
else
  echo "ps3dev non trovato: imposta PS3DEV o installa PSL1GHT in /usr/local/ps3dev" >&2
  exit 1
fi
