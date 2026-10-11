IF(DEFINED ENV{VITASDK} AND EXISTS "$ENV{VITASDK}/share/vita.toolchain.cmake")
  SET(VITASDK "$ENV{VITASDK}")
ELSEIF(EXISTS "C:/msys64/usr/local/vitasdk/share/vita.toolchain.cmake")
  SET(VITASDK "C:/msys64/usr/local/vitasdk")
ELSEIF(EXISTS "/usr/local/vitasdk/share/vita.toolchain.cmake")
  SET(VITASDK "/usr/local/vitasdk")
ELSE()
  SET(VITASDK "")
ENDIF()

IF(NOT VITASDK)
  MESSAGE(FATAL_ERROR "No VitaSDK found. Set VITASDK to the VitaSDK install root, "
                      "adding its bin directory to PATH.")
ENDIF()

SET(ENV{VITASDK} "${VITASDK}")

SET(SUPERTUX_TARGET_CONSOLE "vita" CACHE STRING "Console port being built")

SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_PROCESSOR arm)

FIND_PROGRAM(VITA_PKG_CONFIG NAMES arm-vita-eabi-pkg-config HINTS "${VITASDK}/bin")
IF(VITA_PKG_CONFIG)
  SET(PKG_CONFIG_EXECUTABLE "${VITA_PKG_CONFIG}" CACHE FILEPATH "pkg-config for the Vita target" FORCE)
ELSE()
  MESSAGE(FATAL_ERROR "No arm-vita-eabi-pkg-config under ${VITASDK}/bin")
ENDIF()

SET(CMAKE_C_FLAGS_INIT "${CMAKE_C_FLAGS_INIT} -O3 -fomit-frame-pointer -fno-math-errno -mfpu=neon -mfloat-abi=hard -ffunction-sections -fdata-sections")
SET(CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT} -O3 -fomit-frame-pointer -fno-math-errno -mfpu=neon -mfloat-abi=hard -ffunction-sections -fdata-sections")
SET(CMAKE_EXE_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS_INIT} -Wl,--gc-sections -Wl,-O1")

FIND_PROGRAM(VITA_CC NAMES arm-vita-eabi-gcc HINTS "${VITASDK}/bin")
FIND_PROGRAM(VITA_CXX NAMES arm-vita-eabi-g++ HINTS "${VITASDK}/bin")
IF(NOT VITA_CC OR NOT VITA_CXX)
  MESSAGE(FATAL_ERROR "No arm-vita-eabi-gcc/arm-vita-eabi-g++ under ${VITASDK}/bin")
ENDIF()
SET(CMAKE_C_COMPILER "${VITA_CC}" CACHE FILEPATH "Vita C compiler" FORCE)
SET(CMAKE_CXX_COMPILER "${VITA_CXX}" CACHE FILEPATH "Vita C++ compiler" FORCE)

INCLUDE("${VITASDK}/share/vita.toolchain.cmake")

SET(ENABLE_OPENGL ON CACHE BOOL "Enable OpenGL support")
SET(ENABLE_GLEW OFF CACHE BOOL "Use GLEW to load OpenGL extensions")
SET(ENABLE_LIGHTMAP_FBO OFF CACHE BOOL "Draw the lightmap into its texture with a framebuffer object")
SET(ENABLE_NPOT_TEXTURES ON CACHE BOOL "Assume the hardware accepts non-power-of-two textures")
SET(TEXTURE_ALIGNMENT 1 CACHE STRING "Round texture sides up to a multiple of this many pixels")
SET(ENABLE_OPENAL OFF CACHE BOOL "Build the OpenAL sound backend")
SET(ENABLE_SDL_MIXER ON CACHE BOOL "Build the SDL_mixer sound backend")
SET(ENABLE_CONSOLE OFF CACHE BOOL "Build the interactive console")
SET(SUPERTUX_LTO OFF CACHE BOOL "Compile and link with link time optimization")

SET(CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES "" CACHE STRING "Implicit C include directories" FORCE)
SET(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES "" CACHE STRING "Implicit C++ include directories" FORCE)
