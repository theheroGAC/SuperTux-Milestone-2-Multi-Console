#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")" && pwd)"
TARGET="${1:-}"

case "${TARGET}" in
  switch|vita|ps3)
    "${ROOT}/ports/${TARGET}/build.sh"
    ;;
  all)
    "${ROOT}/ports/switch/build.sh"
    "${ROOT}/ports/vita/build.sh"
    "${ROOT}/ports/ps3/build.sh"
    ;;
  *)
    echo "Uso: $0 {switch|vita|ps3|all}" >&2
    exit 1
    ;;
esac
