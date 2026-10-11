// src/scripting/gradient.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
//  SuperTux - Sector scripting
//  Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
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

#ifndef HEADER_SUPERTUX_SCRIPTING_GRADIENT_HPP
#define HEADER_SUPERTUX_SCRIPTING_GRADIENT_HPP

#include <string>
class Gradient;

#include <simplesquirrel/exposable_class.hpp>

namespace scripting {

class Gradient : public ssq::ExposableClass
{
private:
  ::Gradient* gradient;

public:
  Gradient(::Gradient* parent);
  ~Gradient();

private:
  Gradient(const Gradient&) = delete;
  Gradient& operator=(const Gradient&) = delete;

public:
  void set_direction(const std::string& direction);
  std::string get_direction() const;

  void set_color1(float red, float green, float blue);
  void set_color2(float red, float green, float blue);

  void swap_colors();
};

}

#endif

/* EOF */
