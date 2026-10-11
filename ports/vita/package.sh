#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD_DIR="${ROOT}/build/vita"
OUT_DIR="${ROOT}/dist/vita"
TITLE_ID="${TITLE_ID:-STUX00001}"

if [ -z "${VITASDK:-}" ]; then
  if [ -d "/usr/local/vitasdk" ]; then
    export VITASDK="/usr/local/vitasdk"
  else
    export VITASDK="C:/msys64/usr/local/vitasdk"
  fi
fi
export PATH="${VITASDK}/bin:${PATH}"

ELF="${BUILD_DIR}/supertux2-vita"
if [ ! -f "${ELF}" ]; then
  ELF="${BUILD_DIR}/supertux2-vita.elf"
fi
if [ ! -f "${ELF}" ]; then
  echo "Eseguibile mancante: esegui prima ports/vita/build.sh" >&2
  exit 1
fi

rm -rf "${OUT_DIR}"
mkdir -p "${OUT_DIR}/sce_sys"
cd "${BUILD_DIR}"
vita-elf-create "${ELF}" supertux2.velf
vita-make-fself -s supertux2.velf "${OUT_DIR}/EBOOT.BIN"
vita-mksfoex -s "TITLE_ID=${TITLE_ID}" -s "APP_VER=01.00" -d "ATTRIBUTE2=12" "SuperTux Classic Milestone 2 PS Vita Edition" "${OUT_DIR}/sce_sys/param.sfo"

VPK_ARGS=(-s "${OUT_DIR}/sce_sys/param.sfo" -b "${OUT_DIR}/EBOOT.BIN")
ASSETS="${ROOT}/upstream/packaging/vita/sce_sys"
for file in icon0.png pic0.png livearea/contents/bg.png livearea/contents/startup.png livearea/contents/template.xml; do
  if [ -f "${ASSETS}/${file}" ]; then
    VPK_ARGS+=(-a "${ASSETS}/${file}=sce_sys/${file}")
  fi
done

"${ROOT}/tools/brand_logo.sh" vita

vita-pack-vpk "${VPK_ARGS[@]}" "${OUT_DIR}/supertux2.vpk"
echo "Pacchetto Vita senza dati: ${OUT_DIR}/supertux2.vpk"

VPK_FULL_ARGS=("${VPK_ARGS[@]}" -a "${ROOT}/upstream/data=data")
vita-pack-vpk "${VPK_FULL_ARGS[@]}" "${OUT_DIR}/supertux2_full.vpk"
echo "Pacchetto Vita con i dati inclusi: ${OUT_DIR}/supertux2_full.vpk"
if [ "${DATA_ZIP:-1}" = "1" ]; then
  if command -v zip >/dev/null 2>&1; then
    (cd "${ROOT}/upstream" && zip -r -1 -q "${OUT_DIR}/SuperTux2-data.zip" data)
    echo "Dati da copiare sulla memory card: ${OUT_DIR}/SuperTux2-data.zip"
  fi
fi

  cat > "${OUT_DIR}/LEGGIMI.txt" <<'EOF'
SuperTux Classic Milestone 2 PS Vita Edition 1.00
Port di SuperTux 2 (fork Wii di DeltaResero) per PlayStation Vita - theheroGAC

INSTALLAZIONE
1) Copia supertux2.vpk sulla memory card (ux0:) e installala con VitaShell.
2) I dati di gioco (cartella data, circa 116 MB) NON sono dentro la VPK: vanno
   copiati a mano, una volta sola, sulla memory card.

   Metodo A, il piu' rapido
     - copia SuperTux2-data.zip in ux0:data/
     - da VitaShell, posizionati su ux0:data/ e scegli Estrai sullo zip
     - risultato: ux0:data/data/...

   Metodo B, cartella dedicata al gioco
     - avvia il gioco una volta: crea ux0:data/SuperTux2/
     - copia SuperTux2-data.zip in ux0:data/SuperTux2/
     - da VitaShell, posizionati su ux0:data/SuperTux2/ e scegli Estrai
     - risultato: ux0:data/SuperTux2/data/...

   Il gioco cerca i dati in quest'ordine: ux0:data/SuperTux2/data/,
   ux0:data/data/, app0:data/ (quest'ultima esiste solo se la VPK e' stata
   costruita con WITH_DATA=1).

3) Se il gioco torna al LiveArea senza partire, leggi ux0:data/SuperTux2/supertux.log.

PERCORSI
- dati del gioco: ux0:data/SuperTux2/data oppure ux0:data/data
- salvataggi e configurazione: ux0:data/SuperTux2/userdata
- log di avvio: ux0:data/SuperTux2/supertux.log

NOTE
- I dati esterni si possono modificare senza ricompilare la VPK.
- La VPK con i dati dentro si ottiene con WITH_DATA=1 bash ports/vita/package.sh.
EOF

echo "Istruzioni: ${OUT_DIR}/LEGGIMI.txt"

