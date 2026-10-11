#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
DEPS_PREFIX="${ROOT}/deps/switch"
SRC_DIR="${ROOT}/deps/src/SDL_mixer"
JOBS="${JOBS:-2}"
VERSION="release-2.8.1"

export DEVKITPRO="${DEVKITPRO:-/opt/devkitpro}"
export DEVKITA64="${DEVKITPRO}/devkitA64"
export PATH="${DEVKITA64}/bin:${DEVKITPRO}/tools/bin:${PATH}"
export TMPDIR="${TMPDIR:-${ROOT}/tmp}"
mkdir -p "${TMPDIR}" "${ROOT}/deps/src"

if [ ! -d "${SRC_DIR}" ]; then
  git clone --depth 1 --branch "${VERSION}" https://github.com/libsdl-org/SDL_mixer.git "${SRC_DIR}"
fi

cmake -B "${SRC_DIR}/build" -S "${SRC_DIR}" -G "Unix Makefiles" \
  -DCMAKE_TOOLCHAIN_FILE="${DEVKITPRO}/cmake/Switch.cmake" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="${DEPS_PREFIX}" \
  -DSDL2MIXER_VENDORED=OFF \
  -DSDL2MIXER_DEPS_SHARED=OFF \
  -DSDL2MIXER_CMD=OFF \
  -DSDL2MIXER_MIDI=OFF \
  -DSDL2MIXER_VORBIS=TREMOR \
  -DSDL2MIXER_MP3=OFF \
  -DSDL2MIXER_MOD=OFF \
  -DSDL2MIXER_FLAC=OFF \
  -DSDL2MIXER_OPUS=OFF \
  -DSDL2MIXER_WAVPACK=OFF

cmake --build "${SRC_DIR}/build" -j "${JOBS}"
cmake --install "${SRC_DIR}/build"
