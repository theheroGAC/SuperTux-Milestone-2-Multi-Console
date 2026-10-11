# SuperTux2-MultiPort

A multi-platform port of **SuperTux 2** (based on the Wii port by DeltaResero, upstream version 0.5.1) for **Nintendo Switch**, **PlayStation Vita**, and **PlayStation 3**.

This project unifies and extends console support from a single modern C++ codebase. Each target receives dedicated optimizations, native SDL2/hardware-accelerated rendering without desktop OpenGL or heavy shader dependencies, and seamless integration with each console's operating system.

Each release is branded as **"SuperTux Classic Milestone 2 &lt;Console&gt; Edition"** (Version **1.00**, ported and maintained by **theheroGAC**).

---

## Table of Contents

- [Console Features & Highlights](#console-features--highlights)
- [In-Game Controls](#in-game-controls)
- [Installation Guide](#installation-guide)
  - [Nintendo Switch](#nintendo-switch)
  - [PlayStation Vita](#playstation-vita)
  - [PlayStation 3](#playstation-3)
- [Project Structure](#project-structure)
- [Architecture & Platform Abstraction](#architecture--platform-abstraction)
- [Physics Calibration & 64 Hz Timestep](#physics-calibration--64-hz-timestep)
- [Building from Source](#building-from-source)
  - [Prerequisites](#prerequisites)
  - [Unified Build Script](#unified-build-script)
  - [Packaging](#packaging)
- [Technical Notes & Troubleshooting](#technical-notes--troubleshooting)
- [Credits & License](#credits--license)

---

## Console Features & Highlights

### Nintendo Switch (`.nro`)
- **Native Resolution**: 1280x720 (720p).
- **Video Backend**: Optimized SDL2 renderer using NPOT (non-power-of-two) textures with 1-byte alignment.
- **Audio Backend**: Custom-compiled SDL_mixer 2.8.1 with Tremor/Vorbis support, resolving seek issues (`ov_time_seek`) and ensuring pristine background music playback.
- **Embedded Game Data (RomFS)**: Completely self-contained `.nro` executable with game assets bundled inside RomFS (~116 MB). External data located on the microSD card (`sdmc:/switch/supertux2/data/`) is also supported with priority override.
- **System Integration**: Native `appletMainLoop()` loop handling, sleep mode and focus switching support, saves stored in `sdmc:/switch/supertux2/userdata/`, and startup diagnostics logged to `sdmc:/switch/supertux2/supertux.log`.
- **Clean Release HUD**: On-screen debug overlays removed for clean presentation.

### PlayStation Vita (`.vpk`)
- **Native Resolution**: 960x544 (native OLED / LCD display).
- **Video Backend**: SDL2 renderer accelerated via **vitaGL**.
- **Performance Profile**: Overclocked to maximum hardware limits (ARM CPU 444 MHz, Bus 222 MHz, GPU 222 MHz) for a locked 64 FPS experience.
- **Custom LiveArea**: Custom icon (`icon0.png`), splash banner (`pic0.png`), background (`bg.png`), startup animation, and XML template matching official PlayStation standards.
- **Flexible Packaging**: Available both as a lightweight VPK (`supertux2.vpk`) loading assets from the memory card (`ux0:data/SuperTux2/data`) and as an all-in-one standalone package (`supertux2_full.vpk`).

### PlayStation 3 (`.pkg`)
- **Native Resolution**: 1280x720 HD.
- **Video Backend**: SDL2 running directly on top of the native RSX graphics driver.
- **XMB Integration**: Installable NPDRM `.pkg` package (TITLE_ID: `STUX00001`) with custom XMB icons (`ICON0.PNG`, `PIC1.PNG`) and clean exit to the XMB menu via PSL1GHT `sysutil`.
- **Storage Support**: Reads data from internal HDD (`/dev_hdd0/game/STUX00001/USRDIR/data`) with automatic fallback to external USB drives (`/dev_usb000/SuperTux2/data`).

---

## In-Game Controls

Controls have been carefully mapped and calibrated for each console's controller:

| Action | Nintendo Switch | PlayStation Vita | PlayStation 3 | Keyboard (PC) |
|---|---|---|---|---|
| **Move / Duck** | D-Pad or Left Analog Stick | D-Pad or Left Analog Stick | D-Pad or Left Analog Stick | Arrow Keys |
| **Jump** | `B` (or `A`) | `Cross` (or `Circle`) | `Cross` | `Spacebar` |
| **Action / Run / Shoot** | `Y` (o `X`) | `Square` | `Square` | `Ctrl` / `Alt` |
| **Backflip** | Duck + Jump (standing still) | Duck + Jump (standing still) | Duck + Jump (standing still) | Down + Space (standing still) |
| **Butt Jump** | Down (while in the air) | Down (while in the air) | Down (while in the air) | Down Arrow (in air) |
| **Look Left / Right** | `L` / `R` or Right Stick | `L` / `R` | `L1` / `R1` or Right Stick | `Delete` / `Page Down` |
| **Look Up / Down** | Hold Up / Down | Hold Up / Down | Hold Up / Down | `Home` / `End` |
| **Pause / Menu** | `+` (Start) | `START` | `START` | `Esc` / `P` |
| **Confirm Menu** | `A` (or `B`) | `Cross` (or `Circle`) | `Cross` | `Enter` / `Space` |
| **Cancel / Back Menu** | `B` | `Circle` | `Circle` | `Esc` |

---

## Installation Guide

### Nintendo Switch
1. Download `supertux2.nro` from `dist/switch/switch/supertux2/` (or the Releases page).
2. Copy the `supertux2` directory onto your SD card under `switch/` (resulting in `sdmc:/switch/supertux2/supertux2.nro`).
3. Open the **Homebrew Menu** on your Switch.
   > **Tip**: Launch the Homebrew Menu in **Title Override** mode (hold `R` while launching any installed official game) to grant SuperTux 2 access to full system memory.
4. Launch **SuperTux 2**. Game saves and configuration are written to `sdmc:/switch/supertux2/userdata/`.

### PlayStation Vita
1. Download `supertux2.vpk` and `SuperTux2-data.zip` from `dist/vita/`.
2. Install `supertux2.vpk` using **VitaShell**.
3. Extract `SuperTux2-data.zip` and copy the `data` folder to `ux0:data/SuperTux2/data/` (or `ux0:data/data/`).
4. Alternatively, if you downloaded the standalone `supertux2_full.vpk`, simply install the `.vpk` (the data files are pre-packaged).
5. Launch the game from your LiveArea. Saves and config files will reside in `ux0:data/SuperTux2/userdata/`.

### PlayStation 3
1. Download `supertux2_PS3.pkg` from `dist/ps3/`.
2. Copy the `.pkg` file to the root of a FAT32-formatted USB drive (or transfer it over FTP to `/dev_hdd0/packages/`).
3. On a PS3 running Custom Firmware (CFW) or PS3HEN, navigate to **Package Manager > Install Package Files > Standard**.
4. Select `supertux2_PS3.pkg` to install it.
5. Launch SuperTux 2 from the Game column on the XMB.

---

## Project Structure

```text
SuperTux2-MultiPort/
├── upstream/                 # SuperTux 2 game source code (Wii fork + multi-platform patches)
│   ├── cmake/toolchains/     # Console CMake toolchain files (Switch.cmake, Vita.cmake, PS3.cmake)
│   ├── data/                 # Game assets (levels, graphics, music, sounds)
│   ├── packaging/            # Platform metadata, icons, and branding logos
│   └── src/
│       ├── util/ports.hpp    # Cross-platform console abstraction layer
│       ├── util/ports_switch.cpp # Nintendo Switch implementation (libnx)
│       ├── util/ports_vita.cpp   # PlayStation Vita implementation (VitaSDK)
│       └── util/ports_ps3.cpp    # PlayStation 3 implementation (PSL1GHT)
├── ports/                    # Platform-specific build and packaging scripts
│   ├── switch/               # build.sh, package.sh, build_sdl2_mixer.sh
│   ├── vita/                 # build.sh, package.sh
│   └── ps3/                  # build.sh, package.sh (Docker-backed)
├── deps/                     # Pre-compiled console dependencies (Tremor-enabled SDL_mixer, etc.)
├── build/                    # Intermediate build outputs (per platform)
├── dist/                     # Distribution packages (.nro, .vpk, .pkg)
├── tools/                    # Build utilities (brand_logo.sh, pkg_ps3.py, etc.)
└── build.sh                  # Root build launcher: ./build.sh switch|vita|ps3|all
```

---

## Architecture & Platform Abstraction

To keep the game logic pristine and maintainable across all platforms:
1. **Unified Console Layer (`ports.hpp`)**:
   - Provides an abstract interface for lifecycle hooks (`Ports::init()`, `Ports::tick()`), asset resolution (`Ports::get_data_dir()`, `Ports::get_user_dir()`), and native display dimensions (`Ports::native_width()`, `Ports::native_height()`).
   - Platform implementations (`ports_switch.cpp`, `ports_vita.cpp`, `ports_ps3.cpp`) isolate platform-specific APIs (libnx, VitaSDK, PSL1GHT), preventing `#ifdef` sprawl.
2. **Minimal Upstream Invasions**:
   - `src/main.cpp`: Early bootstrap of the console hardware subsystem and redirection of log outputs.
   - `src/supertux/main.cpp`: Resolution of console datadir and savegame userdir paths.
   - `src/supertux/screen_manager.cpp`: Per-frame lifecycle polling (`Ports::tick()`) to handle homebrew applet events and OS shutdown requests.
   - `src/supertux/gameconfig.cpp`: Enforces console-appropriate default resolutions and fullscreen flags.
3. **Desktop OpenGL Bypass**:
   - The desktop OpenGL/GLEW rendering pipeline is conditionally replaced with native SDL2 renderers for Switch and PS3, and vitaGL for PlayStation Vita.

---

## Physics Calibration & 64 Hz Timestep

Two key adjustments ensure an authentic, responsive console gameplay feel:

1. **Exact 64 Hz Timestep Synchronization**:
   - SuperTux's physics engine operates on a fixed logical step of `1 / LOGICAL_FPS` (1 / 64 s = 15.625 ms). In the upstream engine, frame timing was computed using integer millisecond division (`(Uint32)(1000 / 64) = 15 ms`), resulting in 66.67 ticks per second and frame pacing jitter.
   - The loop timing now uses a double-precision accumulator and rounded frame pacing (`ceil`), synchronizing the game loop to 64 ticks/second and eliminating micro-stuttering.
2. **Jump Velocity Scale (`JUMP_VELOCITY_SCALE`)**:
   - On console builds, jump velocity is scaled by `1.19f` (matching the proven Milestone 1 PS Vita port tuning). This accounts for 60/64 Hz console gamepad response curves, delivering an authentic apex height (~197 px) and eliminating sluggishness during precise platforming jumps.

---

## Building from Source

### Prerequisites

- **Nintendo Switch**:
  - [devkitPro](https://devkitpro.org/) with `devkitA64`, `libnx`, and Switch portlibs (`switch-sdl2`, `switch-sdl2_image`, `switch-sdl2_mixer`, `switch-libvorbis`, `switch-libogg`).
  - The build script automatically compiles SDL_mixer 2.8.1 with Tremor support inside `deps/switch`.
- **PlayStation Vita**:
  - [VitaSDK](https://vitasdk.org/) installed (e.g. `C:/msys64/usr/local/vitasdk`), with `vita-sdl2`, `vita-sdl2_image`, `vita-sdl2_mixer`, `vita-libvorbis`, and `vitagl`.
- **PlayStation 3**:
  - [Docker Desktop](https://www.docker.com/) with the `hldtux/ps3dev-sdl2` image (contains PSL1GHT and pre-built RSX-enabled SDL2).

### Unified Build Script

Run from the root directory:

```bash
# Build Nintendo Switch
./build.sh switch

# Build PlayStation Vita
./build.sh vita

# Build PlayStation 3 (requires Docker Desktop running)
./build.sh ps3

# Build all targets
./build.sh all
```

*Memory Tip*: On machines with 4 GB of RAM, maintain `JOBS=2` (the default) or set `JOBS=1` to prevent linker out-of-memory errors.

### Packaging

Once the executables are built, package the distribution releases:

```bash
# Nintendo Switch -> generates dist/switch/switch/supertux2/supertux2.nro
./ports/switch/package.sh

# PS Vita -> generates dist/vita/supertux2.vpk and dist/vita/SuperTux2-data.zip
./ports/vita/package.sh

# PS Vita (All-in-one with embedded data) -> generates dist/vita/supertux2_full.vpk
WITH_DATA=1 ./ports/vita/package.sh

# PlayStation 3 -> generates dist/ps3/supertux2_PS3.pkg
./ports/ps3/package.sh
```

---

## Technical Notes & Troubleshooting

- **Nintendo Switch (SDL_mixer & Tremor)**:
  devkitPro's binary repository includes `switch-sdl2_mixer` 2.0.4, which lacks `Mix_MusicDuration` and `Mix_GetMusicPosition`. Furthermore, devkitPro unconditionally links `libvorbisidec` (Tremor). Standard libvorbis builds fail on music seek with `ov_time_seek: OV_EINVAL`. SuperTux2-MultiPort compiles SDL_mixer 2.8.1 with Tremor support enabled in `deps/switch` to avoid this conflict.
- **Nintendo Switch (RomFS Root Mapping)**:
  `elf2nro` with `--romfsdir` flattens the contents of `data/` directly into the root of `romfs:/`. The engine checks both `romfs:/` and `romfs:/data/` before falling back to SD storage, ensuring seamless asset loading.
- **PlayStation Vita (MSYS2 & CMake Shell Environment)**:
  `arm-vita-eabi-pkg-config` is a Unix shell script. Under Windows, builds must be invoked from a clean MSYS2 bash environment (`/usr/bin/bash`) to prevent MINGW64 native CMake from interpreting shell scripts as invalid executables.
- **PlayStation 3 (NPDRM SELF & Package Hierarchy)**:
  `USRDIR/EBOOT.BIN` must be encrypted as a signed NPDRM SELF file using `make_self_npdrm`. Unsigned `fself` executables fail installation on retail/HEN PS3 units with error `80029566` at 4%. In addition, `PARAM.SFO`, `ICON0.PNG`, and `PIC1.PNG` are mirrored at the root of the package structure to guarantee clean installation across all firmware revisions.

---

## Credits & License

- **Original SuperTux Game**: [SuperTux Development Team](https://www.supertux.org/)
- **Wii Port & Milestone 2 Optimizations**: DeltaResero ([SuperTux2-Wii](https://github.com/DeltaResero/SuperTux2-Wii))
- **Nintendo Switch, PlayStation Vita & PlayStation 3 Ports**: **theheroGAC**
- **License**: GNU General Public License v3.0 or later ([GPLv3+](LICENSE))
