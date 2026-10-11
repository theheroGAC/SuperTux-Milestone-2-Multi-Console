// src/scripting/supertux_api.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
//  SuperTux
//  Copyright (C) 2026 SuperTux2-Wii contributors
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

#ifndef HEADER_SUPERTUX_SCRIPTING_SUPERTUX_API_HPP
#define HEADER_SUPERTUX_SCRIPTING_SUPERTUX_API_HPP

#include <squirrel.h>
#include <simplesquirrel/vm.hpp>

#include <sstream>
#include <typeinfo>

#include "scripting/squirrel_error.hpp"

namespace scripting {

/** Puts every class, function and constant a level script can reach into the
    root table of the given VM. */
void register_supertux_api(HSQUIRRELVM v);

/** Deletes an object squirrel was handed ownership of. */
template<typename T>
SQInteger release_hook(SQUserPointer ptr, SQInteger)
{
  delete reinterpret_cast<T*>(ptr);
  return 0;
}

/** Makes a squirrel instance of a registered class standing for an object we
    own. The class comes from the registry simplesquirrel fills in as it binds,
    so a type is looked up by what it is rather than by its name. */
template<typename T>
void create_squirrel_instance(HSQUIRRELVM v, T* object,
                              bool setup_releasehook = false)
{
  sq_pushobject(v, ssq::VM::getClassObj(typeid(T*).hash_code()));

  if(SQ_FAILED(sq_createinstance(v, -1)) ||
     SQ_FAILED(sq_setinstanceup(v, -1, object))) {
    std::ostringstream msg;
    msg << "Couldn't set up squirrel instance for object of type '"
        << typeid(T).name() << "'";
    throw SquirrelError(v, msg.str());
  }
  sq_remove(v, -2); // remove the class

  if(setup_releasehook) {
    sq_setreleasehook(v, -1, &release_hook<T>);
  }
}

} // namespace scripting

#endif

/* EOF */
