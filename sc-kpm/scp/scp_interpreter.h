/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2013 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

#ifndef SCP_INTERPRETER_H
#define SCP_INTERPRETER_H

#include "scp_lib.h"

//! Module initialization function
_SC_EXT_EXTERN sc_result initialize();

//! Module shutdown function
_SC_EXT_EXTERN sc_result shutdown();

_SC_EXT_EXTERN sc_uint32 load_priority();

#endif // SCP_INTERPRETER_H
