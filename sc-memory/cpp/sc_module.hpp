/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_types.hpp"
#include "sc_object.hpp"
#include "sc_addr.hpp"
#include "kpm/sc_agent.hpp"

#include "generated/sc_module.generated.hpp"

class ScModule : public ScObject
{
	SC_CLASS()
	SC_GENERATED_BODY()

	virtual sc_result initializeImpl() = 0;
	virtual sc_result shutdownImpl() = 0;
};

#define _SC_MODULE_INSTANCE(__ModuleName) __ModuleName##Instance

#define SC_IMPLEMENT_MODULE(__ModuleName) \
__ModuleName _SC_MODULE_INSTANCE(__ModuleName); \
extern "C" \
{ \
	_SC_EXT_EXTERN sc_result initialize() \
	{ \
		return _SC_MODULE_INSTANCE(__ModuleName)._initialize(); \
	} \
	_SC_EXT_EXTERN sc_uint32 load_priority() \
	{ \
		return _SC_MODULE_INSTANCE(__ModuleName)._getLoadPriority(); \
	} \
	_SC_EXT_EXTERN sc_result shutdown() \
	{ \
		return _SC_MODULE_INSTANCE(__ModuleName)._shutdown(); \
	} \
}
