#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD_DIR="${ROOT}/build/vita"
JOBS="${JOBS:-2}"

if [ -z "${VITASDK:-}" ]; then
  if [ -d "/usr/local/vitasdk" ]; then
    export VITASDK="/usr/local/vitasdk"
  else
    export VITASDK="C:/msys64/usr/local/vitasdk"
  fi
fi

if [ -z "${CMAKE:-}" ] && [ -x /usr/bin/cmake ]; then
  CMAKE=/usr/bin/cmake
else
  CMAKE="${CMAKE:-cmake}"
fi

export PATH="${VITASDK}/bin:${PATH}"

"${ROOT}/tools/brand_logo.sh" vita
export TMPDIR="${TMPDIR:-${ROOT}/tmp}"
mkdir -p "${TMPDIR}"

"${CMAKE}" -B "${BUILD_DIR}" -S "${ROOT}/upstream" -G "Unix Makefiles" \
  -DCMAKE_TOOLCHAIN_FILE="${ROOT}/upstream/cmake/toolchains/Vita.cmake" \
  -DCMAKE_BUILD_TYPE=Release

"${CMAKE}" --build "${BUILD_DIR}" -j "${JOBS}"
