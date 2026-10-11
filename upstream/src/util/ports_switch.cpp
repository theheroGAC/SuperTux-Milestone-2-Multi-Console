#ifdef SUPERTUX_PLATFORM_SWITCH

#include "util/ports.hpp"

#include <cstdio>
#include <dirent.h>
#include <fcntl.h>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

#include <SDL.h>
#include <switch.h>

namespace Ports {

namespace {

const char* const SD_BASE = "sdmc:/switch/supertux2/";
const char* const ROMFS_ROOT = "romfs:/";
const char* const ROMFS_DATA = "romfs:/data/";

bool initialized = false;
bool romfs_mounted = false;
bool quit_sent = false;

bool dir_exists(const std::string& directory)
{
  DIR* dir = opendir(directory.c_str());
  if (dir == nullptr)
  {
    return false;
  }

  closedir(dir);
  return true;
}

}

void log_text(const char* text)
{
  fputs(text != nullptr ? text : "(null)", stdout);
  fputs("\n", stdout);
}

void stage(const char* name)
{
  fputs("stage: ", stdout);
  fputs(name != nullptr ? name : "(null)", stdout);
  fputs("\n", stdout);
}

void init()
{
  if (initialized)
  {
    return;
  }

  mkdir(SD_BASE, 0777);
  mkdir((std::string(SD_BASE) + "userdata").c_str(), 0777);

  const int log_fd = open("sdmc:/switch/supertux2/supertux.log",
                          O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (log_fd >= 0)
  {
    dup2(log_fd, 1);
    dup2(log_fd, 2);
    close(log_fd);
    setvbuf(stdout, nullptr, _IONBF, 0);
    setvbuf(stderr, nullptr, _IONBF, 0);
  }

  fputs("supertux2 switch boot\n", stdout);

  socketInitializeDefault();
  fputs("socket ready\n", stdout);

  if (R_SUCCEEDED(romfsInit()))
  {
    romfs_mounted = true;
    fputs("romfs mounted\n", stdout);
  }
  else
  {
    fputs("romfs unavailable\n", stdout);
  }

  initialized = true;
  fputs("ports ready\n", stdout);
}

void tick()
{
  if (!initialized || quit_sent)
  {
    return;
  }

  if (appletMainLoop())
  {
    return;
  }

  quit_sent = true;

  SDL_Event event;
  event.type = SDL_QUIT;
  SDL_PushEvent(&event);
}

std::string get_base_dir()
{
  return SD_BASE;
}

std::string get_data_dir()
{
  const std::string sd_data = std::string(SD_BASE) + "data/";
  if (dir_exists(sd_data))
  {
    return sd_data;
  }

  if (romfs_mounted)
  {
    if (dir_exists(ROMFS_DATA))
    {
      return ROMFS_DATA;
    }

    if (dir_exists(ROMFS_ROOT))
    {
      return ROMFS_ROOT;
    }
  }

  throw std::runtime_error("No data directory found in " + sd_data + ", " + ROMFS_DATA + " or " + ROMFS_ROOT);
}

std::string get_user_dir()
{
  return std::string(SD_BASE) + "userdata/";
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
