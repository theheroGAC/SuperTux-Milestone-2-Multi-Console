#ifndef HEADER_SUPERTUX_UTIL_PORTS_HPP
#define HEADER_SUPERTUX_UTIL_PORTS_HPP

#if defined(SUPERTUX_PLATFORM_SWITCH) || defined(SUPERTUX_PLATFORM_VITA) || defined(SUPERTUX_PLATFORM_PS3)
#define SUPERTUX_PLATFORM_CONSOLE 1
#endif

#ifdef SUPERTUX_PLATFORM_CONSOLE

#include <string>

namespace Ports {

void init();
void tick();

/* Progress markers for the port's own log file: a hang writes the last stage
   it reached, which a debugger would otherwise be needed to find. */
void stage(const char* name);
void log_text(const char* text);

std::string get_base_dir();
std::string get_data_dir();
std::string get_user_dir();
int native_width();
int native_height();

}

#endif

#endif
