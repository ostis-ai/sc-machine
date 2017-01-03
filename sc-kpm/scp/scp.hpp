/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc_memory.h"
#include "sc-memory/cpp/sc_module.hpp"

#include "scp.generated.hpp"

class scpModule : public ScModule
{
    SC_CLASS(LoadOrder(50))
    SC_GENERATED_BODY()

    sc_result initializeImpl();
    sc_result shutdownImpl();
};
