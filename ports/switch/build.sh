#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD_DIR="${ROOT}/build/switch"
DEPS_PREFIX="${ROOT}/deps/switch"
JOBS="${JOBS:-2}"

export DEVKITPRO="${DEVKITPRO:-/opt/devkitpro}"
export DEVKITA64="${DEVKITPRO}/devkitA64"
export PATH="${DEVKITA64}/bin:${DEVKITPRO}/tools/bin:${PATH}"

"${ROOT}/tools/brand_logo.sh" switch
touch "${ROOT}/upstream/src/main.cpp"
export TMPDIR="${TMPDIR:-${ROOT}/tmp}"
export PKG_CONFIG_LIBDIR="${DEPS_PREFIX}/lib/pkgconfig:${DEVKITPRO}/portlibs/switch/lib/pkgconfig"
mkdir -p "${TMPDIR}"

if [ ! -f "${DEPS_PREFIX}/lib/pkgconfig/SDL2_mixer.pc" ]; then
  "${ROOT}/ports/switch/build_sdl2_mixer.sh"
fi

cmake -B "${BUILD_DIR}" -S "${ROOT}/upstream" -G "Unix Makefiles" \
  -DCMAKE_TOOLCHAIN_FILE="${ROOT}/upstream/cmake/toolchains/Switch.cmake" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="${DEPS_PREFIX}" \
  -DCMAKE_CXX_FLAGS="-I${DEPS_PREFIX}/include/SDL2 -L${DEPS_PREFIX}/lib" \
  -DCMAKE_C_FLAGS="-I${DEPS_PREFIX}/include/SDL2 -L${DEPS_PREFIX}/lib" \
  -DSUPERTUX_ROMFS_DATA=ON \
  -DCMAKE_MAKE_PROGRAM=make

cmake --build "${BUILD_DIR}" -j "${JOBS}"
