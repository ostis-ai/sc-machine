/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc_memory.h"

extern "C"
{
	_SC_EXT_EXTERN sc_result initialize();
	//_SC_EXT_EXTERN sc_uint32 load_priority();
	_SC_EXT_EXTERN sc_result shutdown();
}