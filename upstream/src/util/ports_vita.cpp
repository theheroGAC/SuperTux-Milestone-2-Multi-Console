#ifdef SUPERTUX_PLATFORM_VITA

#include "util/ports.hpp"

#include <cstdio>
#include <dirent.h>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

#include <psp2/power.h>
#include <cstring>

extern "C" {
unsigned int _newlib_heap_size_user = 128 * 1024 * 1024;
unsigned int _pthread_stack_default_user = 1 * 1024 * 1024;
}

namespace Ports {

namespace {

const char* const USER_BASE = "ux0:data/SuperTux2/";

bool initialized = false;

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
  std::string clean = strip_trailing_slashes(directory);
  DIR* dir = opendir(clean.c_str());
  if (dir == nullptr)
  {
    return false;
  }
  closedir(dir);
  return true;
}

bool has_data_files(const std::string& directory)
{
  std::string base = directory;
  if (!base.empty() && base.back() != '/')
  {
    base += '/';
  }
  FILE* f = fopen((base + "credits.txt").c_str(), "rb");
  if (f != nullptr)
  {
    fclose(f);
    return true;
  }
  f = fopen((base + "images/engine/menu/logo.png").c_str(), "rb");
  if (f != nullptr)
  {
    fclose(f);
    return true;
  }
  f = fopen((base + "levels/world1/worldmap.stwm").c_str(), "rb");
  if (f != nullptr)
  {
    fclose(f);
    return true;
  }
  f = fopen((base + "scripts/default.nut").c_str(), "rb");
  if (f != nullptr)
  {
    fclose(f);
    return true;
  }
  return false;
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

  scePowerSetArmClockFrequency(444);
  scePowerSetBusClockFrequency(222);
  scePowerSetGpuClockFrequency(222);

  mkdir("ux0:data/SuperTux2", 0777);
  mkdir("ux0:data/SuperTux2/userdata", 0777);

  FILE* log_out = freopen("ux0:data/SuperTux2/supertux.log", "w", stdout);
  if (log_out == nullptr)
  {
    log_out = freopen("ux0:supertux.log", "w", stdout);
  }
  if (log_out != nullptr)
  {
    setvbuf(log_out, nullptr, _IONBF, 0);
  }

  FILE* log_err = freopen("ux0:data/SuperTux2/supertux.log", "a", stderr);
  if (log_err == nullptr)
  {
    log_err = freopen("ux0:supertux.log", "a", stderr);
  }
  if (log_err != nullptr)
  {
    setvbuf(log_err, nullptr, _IONBF, 0);
  }

  fputs("supertux2 vita boot\n", stdout);

  initialized = true;
}

void tick()
{
}

std::string get_base_dir()
{
  return USER_BASE;
}

std::string get_data_dir()
{
  const char* const candidates[] = {
    "app0:data",
    "app0:",
    "ux0:data/SuperTux2/data",
    "ux0:data/data",
    "ux0:data/SuperTux2-data/data",
    "ux0:data/SuperTux2-data",
    "ux0:data/SuperTux2",
    "ux0:data/supertux2/data",
    "ux0:data/supertux2",
    "ux0:data/SuperTux/data",
    "ux0:data/SuperTux",
    "ux0:data/Data",
    "ux0:data",
    "uma0:data/SuperTux2/data",
    "uma0:data/data",
    "uma0:data/SuperTux2",
    "imc0:data/SuperTux2/data",
    "imc0:data/data"
  };

  for (const char* candidate : candidates)
  {
    if (has_data_files(candidate))
    {
      std::string result = candidate;
      if (result.back() != '/')
      {
        result += '/';
      }
      fputs("data dir (verified): ", stdout);
      fputs(result.c_str(), stdout);
      fputs("\n", stdout);
      return result;
    }
  }

  for (const char* candidate : candidates)
  {
    if (dir_exists(candidate) && strcmp(candidate, "ux0:data/SuperTux2") != 0 && strcmp(candidate, "app0:") != 0)
    {
      std::string result = candidate;
      if (result.back() != '/')
      {
        result += '/';
      }
      fputs("data dir (fallback): ", stdout);
      fputs(result.c_str(), stdout);
      fputs("\n", stdout);
      return result;
    }
  }

  fputs("error: no data directory found\n", stdout);
  throw std::runtime_error("No SuperTux2 data files found. Please copy data files to ux0:data/SuperTux2/data or ux0:data/data");
}

std::string get_user_dir()
{
  return std::string(USER_BASE) + "userdata/";
}

int native_width()
{
  return 960;
}

int native_height()
{
  return 544;
}

}
#endif
