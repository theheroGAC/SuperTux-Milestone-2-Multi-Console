IF(DEFINED ENV{DEVKITPRO})
  SET(DEVKITPRO "$ENV{DEVKITPRO}")
ELSE()
  SET(DEVKITPRO "/opt/devkitpro")
ENDIF()

IF(NOT EXISTS "${DEVKITPRO}/cmake/Switch.cmake")
  MESSAGE(FATAL_ERROR "No devkitPro Switch support under ${DEVKITPRO}. "
                      "Install devkitA64 and libnx, or point DEVKITPRO at them.")
ENDIF()

SET(SUPERTUX_TARGET_CONSOLE "switch" CACHE STRING "Console port being built")

SET(CMAKE_C_FLAGS_INIT "${CMAKE_C_FLAGS_INIT} -O3 -fomit-frame-pointer -fno-math-errno")
SET(CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT} -O3 -fomit-frame-pointer -fno-math-errno")
SET(CMAKE_EXE_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS_INIT} -Wl,--gc-sections -Wl,-O1")

INCLUDE("${DEVKITPRO}/cmake/Switch.cmake")

SET(ENABLE_OPENGL OFF CACHE BOOL "Enable OpenGL support")
SET(ENABLE_GLEW OFF CACHE BOOL "Use GLEW to load OpenGL extensions")
SET(ENABLE_LIGHTMAP_FBO OFF CACHE BOOL "Draw the lightmap into its texture with a framebuffer object")
SET(ENABLE_NPOT_TEXTURES ON CACHE BOOL "Assume the hardware accepts non-power-of-two textures")
SET(TEXTURE_ALIGNMENT 1 CACHE STRING "Round texture sides up to a multiple of this many pixels")
SET(ENABLE_OPENAL OFF CACHE BOOL "Build the OpenAL sound backend")
SET(ENABLE_SDL_MIXER ON CACHE BOOL "Build the SDL_mixer sound backend")
SET(ENABLE_CONSOLE OFF CACHE BOOL "Build the interactive console")

SET(SUPERTUX_APP_AUTHOR "theheroGAC" CACHE STRING "Author written into the application metadata" FORCE)
SET(SUPERTUX_APP_TITLE "SuperTux Classic Milestone 2 Switch Edition" CACHE STRING "Title written into the application metadata" FORCE)
SET(SUPERTUX_ROMFS_DATA OFF CACHE BOOL "Embed the game data into the NRO as romfs")
SET(SUPERTUX_LTO OFF CACHE BOOL "Compile and link with link time optimization")
