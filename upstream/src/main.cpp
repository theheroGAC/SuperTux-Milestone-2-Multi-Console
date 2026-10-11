// src/main.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
//  SuperTux
//  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "SDL.h"

#include "supertux/main.hpp"
#include "util/ports.hpp"
#ifdef __wii__
#include "util/wii.hpp"
#endif

#ifdef SUPERTUX_PLATFORM_PS3
#include <sys/process.h>
#endif

int main(int argc, char** argv)
{
#ifdef __wii__
  Wii::take_power_buttons();
  Wii::fit_overscan();
#endif
#ifdef SUPERTUX_PLATFORM_CONSOLE
  Ports::init();
#endif

  int result = Main().run(argc, argv);

#ifdef __wii__
  // Main has saved the config by now.
  Wii::power_off_if_requested();
#endif
#ifdef SUPERTUX_PLATFORM_PS3
  sysProcessExit(result);
#endif

  return result;
}

/* EOF */
