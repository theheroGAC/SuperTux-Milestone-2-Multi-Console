#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD_DIR="${ROOT}/build/ps3"
PKG_DIR="${ROOT}/dist/ps3/pkgbuild"
GAME_DIR="${PKG_DIR}/PS3_GAME"
OUT_DIR="${ROOT}/dist/ps3"
TITLE_ID="STUX00001"
CONTENT_ID="UP0001-${TITLE_ID}_00-0000000000000000"

if [ ! -f "${BUILD_DIR}/supertux2-ps3" ]; then
  echo "ELF di gioco mancante (build/ps3/supertux2-ps3): esegui prima ports/ps3/build.sh" >&2
  exit 1
fi

if [ "${WITH_DATA:-0}" = "1" ]; then
  PKG_NAME="supertux2_PS3.pkg"
else
  PKG_NAME="supertux2_PS3_nodata.pkg"
fi

rm -rf "${PKG_DIR}"
mkdir -p "${GAME_DIR}/USRDIR"
cp "${ROOT}/upstream/packaging/ps3/sfo.xml" "${PKG_DIR}/sfo.xml"
for file in ICON0.PNG PIC1.PNG; do
  if [ -f "${ROOT}/upstream/packaging/ps3/${file}" ]; then
    cp "${ROOT}/upstream/packaging/ps3/${file}" "${GAME_DIR}/${file}"
  fi
done

if [ "${WITH_DATA:-0}" = "1" ]; then
  cp -r "${ROOT}/upstream/data" "${GAME_DIR}/USRDIR/data"
  "${ROOT}/tools/brand_logo.sh" ps3 "${GAME_DIR}/USRDIR/data"
fi

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

if command -v make_self_npdrm >/dev/null 2>&1; then
  export PYTHONPATH="${PS3DEV}/bin:${PYTHONPATH:-}"
  cp "${BUILD_DIR}/supertux2-ps3" /tmp/supertux2.elf
  if command -v powerpc64-ps3-elf-strip >/dev/null 2>&1; then
    powerpc64-ps3-elf-strip /tmp/supertux2.elf
  elif [ -x "${PS3DEV}/ppu/bin/powerpc64-ps3-elf-strip" ]; then
    "${PS3DEV}/ppu/bin/powerpc64-ps3-elf-strip" /tmp/supertux2.elf
  fi
  if command -v sprxlinker >/dev/null 2>&1; then
    sprxlinker /tmp/supertux2.elf
  elif [ -x "${PS3DEV}/bin/sprxlinker" ]; then
    "${PS3DEV}/bin/sprxlinker" /tmp/supertux2.elf
  fi
  (cd "${GAME_DIR}/USRDIR" && make_self_npdrm /tmp/supertux2.elf EBOOT.BIN "${CONTENT_ID}")
  python3 "${PS3DEV}/bin/sfo.py" -f "${PKG_DIR}/sfo.xml" "${GAME_DIR}/PARAM.SFO"
  python3 "${ROOT}/tools/pkg_ps3.py" --contentid="${CONTENT_ID}" "${GAME_DIR}" "${OUT_DIR}/${PKG_NAME}"
  GN_PKG="${OUT_DIR}/${PKG_NAME%.pkg}.gnpdrm.pkg"
  cp -f "${OUT_DIR}/${PKG_NAME}" "${GN_PKG}"
  if command -v package_finalize >/dev/null 2>&1; then
    package_finalize "${GN_PKG}"
  elif [ -x "${PS3DEV}/bin/package_finalize" ]; then
    "${PS3DEV}/bin/package_finalize" "${GN_PKG}"
  fi
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
    export PYTHONPATH=\"\${PS3DEV}/bin:\${PYTHONPATH:-}\"
    cp \"${WSL_ROOT}/build/ps3/supertux2-ps3\" /tmp/supertux2.elf
    powerpc64-ps3-elf-strip /tmp/supertux2.elf
    if [ -x \"\${PS3DEV}/bin/sprxlinker\" ]; then
      \"\${PS3DEV}/bin/sprxlinker\" /tmp/supertux2.elf
    fi
    (cd \"${WSL_ROOT}/dist/ps3/pkgbuild/PS3_GAME/USRDIR\" && make_self_npdrm /tmp/supertux2.elf EBOOT.BIN \"${CONTENT_ID}\")
    python3 \"\${PS3DEV}/bin/sfo.py\" -f \"${WSL_ROOT}/dist/ps3/pkgbuild/sfo.xml\" \"${WSL_ROOT}/dist/ps3/pkgbuild/PS3_GAME/PARAM.SFO\"
    python3 \"${WSL_ROOT}/tools/pkg_ps3.py\" --contentid=\"${CONTENT_ID}\" \"${WSL_ROOT}/dist/ps3/pkgbuild/PS3_GAME\" \"${WSL_ROOT}/dist/ps3/${PKG_NAME}\"
    GN_PKG=\"${WSL_ROOT}/dist/ps3/${PKG_NAME%.pkg}.gnpdrm.pkg\"
    cp -f \"${WSL_ROOT}/dist/ps3/${PKG_NAME}\" \"\${GN_PKG}\"
    if [ -x \"\${PS3DEV}/bin/package_finalize\" ]; then
      \"\${PS3DEV}/bin/package_finalize\" \"\${GN_PKG}\"
    fi
  "
else
  echo "ps3dev non trovato: imposta PS3DEV o installa PSL1GHT in /usr/local/ps3dev" >&2
  exit 1
fi

if [ "${WITH_DATA:-0}" = "1" ]; then
  cat > "${OUT_DIR}/LEGGIMI.txt" <<'TXT'
SuperTux Classic Milestone 2 PS3 Edition 1.00
Port di SuperTux 2 (fork Wii di DeltaResero) per PlayStation 3 - theheroGAC

INSTALLAZIONE
1) Copia supertux2_PS3.pkg su una chiavetta USB (o via FTP in /dev_hdd0/packages/)
2) Installalo da Gioco > Installa pacchetto
3) Avvia il gioco dal menu XMB

Il pacchetto contiene tutto, dati di gioco compresi.

PERCORSI
- dati del gioco: /dev_hdd0/game/STUX00001/USRDIR/data
- salvataggi e configurazione: /dev_hdd0/game/STUX00001/USRDIR/userdata

NOTE
- Il pacchetto leggero senza dati (per prove rapide) si ottiene con
  bash ports/ps3/package.sh e si chiama supertux2_PS3_nodata.pkg; in quel caso i dati
  vanno copiati a mano da SuperTux2-data.zip in USRDIR o in /dev_usb000/SuperTux2/data.
TXT

  echo "Pacchetto PS3 con i dati inclusi: ${OUT_DIR}/${PKG_NAME}"
else
  echo "Pacchetto PS3 senza dati: ${OUT_DIR}/${PKG_NAME}"

  if [ "${DATA_ZIP:-1}" = "1" ]; then
    if command -v zip >/dev/null 2>&1; then
      "${ROOT}/tools/brand_logo.sh" ps3
      (cd "${ROOT}/upstream" && zip -r -1 -q "${OUT_DIR}/SuperTux2-data.zip" data)
      echo "Dati da copiare sulla console: ${OUT_DIR}/SuperTux2-data.zip"
    else
      echo "zip non disponibile: copia a mano ${ROOT}/upstream/data in /dev_hdd0/game/${TITLE_ID}/USRDIR/data" >&2
    fi
  fi

  cat > "${OUT_DIR}/LEGGIMI.txt" <<'TXT'
SuperTux Classic Milestone 2 PS3 Edition 1.00
Port di SuperTux 2 (fork Wii di DeltaResero) per PlayStation 3 - theheroGAC

INSTALLAZIONE
1) Copia supertux2_PS3_nodata.pkg su una chiavetta USB (o via FTP in /dev_hdd0/packages/)
   e installalo da Gioco > Installa pacchetto.
2) Questo pacchetto non contiene la cartella data (circa 116 MB): va copiata una volta
   sola sulla console.
     - con l'FTP: copia il contenuto di SuperTux2-data.zip in
       /dev_hdd0/game/STUX00001/USRDIR/  ottenendo USRDIR/data/...
     - con la chiavetta USB: copia la cartella data in /dev_usb000/SuperTux2/data/
   Lo zip ha data/ come cartella di primo livello, quindi si estrae direttamente
   in USRDIR o in /dev_usb000/SuperTux2.

PERCORSI
- dati del gioco: /dev_hdd0/game/STUX00001/USRDIR/data oppure /dev_usb000/SuperTux2/data
- salvataggi e configurazione: /dev_hdd0/game/STUX00001/USRDIR/userdata

NOTE
- Il pacchetto completo con i dati dentro si ottiene con
  WITH_DATA=1 bash ports/ps3/package.sh (supertux2_PS3.pkg).
TXT

  echo "Istruzioni: ${OUT_DIR}/LEGGIMI.txt"
fi

echo "PKG: ${OUT_DIR}/${PKG_NAME}"
