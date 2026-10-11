#ifdef SUPERTUX_PLATFORM_PS3

#include "util/ports.hpp"

#include <cstdarg>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <dirent.h>
#include <stdexcept>
#include <string>
#include <unistd.h>

#include <SDL.h>
#include <sys/process.h>

SYS_PROCESS_PARAM(1001, 0x200000);

extern "C" {
float SDL_roundf(float x)
{
  return (x >= 0.0f) ? std::floor(x + 0.5f) : std::ceil(x - 0.5f);
}

int SDL_RenderSetVSync(SDL_Renderer* renderer, int vsync)
{
  (void) renderer;
  (void) vsync;
  return 0;
}

void ps3_sdl_render_cp(const char* what)
{
  (void) what;
}

void ps3_sdl_flip_timeout(unsigned int ms)
{
  Ports::stage("render: flip timed out");
  (void) ms;
}
}

namespace Ports {

namespace {

const char* const HDD_BASE = "/dev_hdd0/game/STUX00001/USRDIR";
const char* const USB_BASE = "/dev_usb000/SuperTux2";

/* Written to all at once: whichever of them the console lets us create ends up
   with the whole story, and /dev_hdd0/tmp is writable even when the install
   directory is not. */
const char* const LOG_PATHS[] = {
  "/dev_hdd0/tmp/supertux.log",
  "/dev_hdd0/game/STUX00001/USRDIR/supertux.log",
  "/dev_usb000/supertux.log"
};

const int NUM_SINKS = (int)(sizeof(LOG_PATHS) / sizeof(LOG_PATHS[0]));

FILE* s_sinks[NUM_SINKS] = { nullptr, nullptr, nullptr };
bool s_sinks_opened = false;
bool initialized = false;
bool video_logged = false;
int s_stages = 0;

void open_sinks()
{
  if (s_sinks_opened)
  {
    return;
  }

  s_sinks_opened = true;

  /* Everything the game prints, std::cerr included, goes to the same first
     log file. freopen and not dup2, which psl1ght's libc does not have; the
     first path is opened in append mode below, so redirecting stdout to it
     does not discard the lines already written. */
  if (freopen(LOG_PATHS[0], "w", stdout) != nullptr)
  {
    setvbuf(stdout, nullptr, _IONBF, 0);
  }

  if (freopen(LOG_PATHS[0], "a", stderr) != nullptr)
  {
    setvbuf(stderr, nullptr, _IONBF, 0);
  }

  for (int i = 0; i < NUM_SINKS; ++i)
  {
    s_sinks[i] = fopen(LOG_PATHS[i], "a");
    if (s_sinks[i] != nullptr)
    {
      /* Unbuffered: every line reaches the card before the next thing runs, so
         a hang still leaves the last stage written out. */
      setvbuf(s_sinks[i], nullptr, _IONBF, 0);
    }
  }
}

void raw_write(const char* msg)
{
  open_sinks();

  for (int i = 0; i < NUM_SINKS; ++i)
  {
    if (s_sinks[i] != nullptr)
    {
      fputs(msg, s_sinks[i]);
    }
  }
}

void raw_printf(const char* fmt, ...)
{
  char buffer[512];
  va_list ap;

  va_start(ap, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, ap);
  va_end(ap);

  raw_write(buffer);
}

std::string strip_trailing_slashes(const std::string& path)
{
  std::string clean = path;
  while (clean.length() > 1 && (clean.back() == '/' || clean.back() == '\\'))
  {
    clean.pop_back();
  }
  return clean;
}

bool dir_exists(const std::string& directory)
{
  const std::string clean = strip_trailing_slashes(directory);
  DIR* dir = opendir(clean.c_str());
  if (dir == nullptr)
  {
    return false;
  }

  closedir(dir);
  return true;
}

void log_video()
{
  const char* driver = SDL_GetCurrentVideoDriver();
  raw_printf("ps3: video driver: %s\n", driver != nullptr ? driver : "unknown");
  raw_printf("ps3: displays: %d\n", SDL_GetNumVideoDisplays());

  SDL_DisplayMode mode;
  if (SDL_GetCurrentDisplayMode(0, &mode) == 0)
  {
    raw_printf("ps3: display mode: %dx%d@%d %s\n", mode.w, mode.h,
               mode.refresh_rate, SDL_GetPixelFormatName(mode.format));
  }

  SDL_Window* window = SDL_GetKeyboardFocus();
  if (window == nullptr)
  {
    raw_write("ps3: window: none focused\n");
    return;
  }

  int width = 0;
  int height = 0;
  SDL_GetWindowSize(window, &width, &height);
  raw_printf("ps3: window: %dx%d flags 0x%x\n", width, height,
             (unsigned int) SDL_GetWindowFlags(window));
}

/* Runs before main, so a line in the log proves the ELF itself started. */
struct EarlyBoot
{
  EarlyBoot()
  {
    raw_write("ps3: ELF loaded, constructors running\n");
  }
};

EarlyBoot s_early_boot;

}

void log_text(const char* text)
{
  raw_write(text != nullptr ? text : "(null)");
  raw_write("\n");
}

void stage(const char* name)
{
  ++s_stages;
  raw_printf("stage %02d: %s\n", s_stages, name != nullptr ? name : "(null)");
}

void init()
{
  if (initialized)
  {
    return;
  }

  initialized = true;

  SDL_version sdl_version;
  SDL_GetVersion(&sdl_version);
  raw_printf("ps3: main reached, SDL %d.%d.%d\n",
             sdl_version.major, sdl_version.minor, sdl_version.patch);

  for (int i = 0; i < NUM_SINKS; ++i)
  {
    if (s_sinks[i] != nullptr)
    {
      raw_printf("ps3: log at %s\n", LOG_PATHS[i]);
    }
  }

  raw_printf("ps3: video drivers available: %d\n", SDL_GetNumVideoDrivers());
  raw_printf("ps3: base dir: %s\n", get_base_dir().c_str());

  stage("ports ready");
}

void tick()
{
  if (!video_logged)
  {
    video_logged = true;
    log_video();
  }
}

std::string get_base_dir()
{
  if (dir_exists(std::string(HDD_BASE) + "/data"))
  {
    return HDD_BASE;
  }

  if (dir_exists(std::string(USB_BASE) + "/data"))
  {
    return USB_BASE;
  }

  return HDD_BASE;
}

std::string get_data_dir()
{
  const std::string data = get_base_dir() + "/data";
  if (!dir_exists(data))
  {
    raw_printf("ps3: no data directory at %s\n", data.c_str());
    throw std::runtime_error("No data directory at " + data);
  }

  raw_printf("ps3: data dir: %s\n", data.c_str());
  return data;
}

std::string get_user_dir()
{
  const std::string user = get_base_dir() + "/userdata";
  raw_printf("ps3: user dir: %s\n", user.c_str());
  return user;
}

int native_width()
{
  return 1280;
}

int native_height()
{
  return 720;
}

}
#endif
