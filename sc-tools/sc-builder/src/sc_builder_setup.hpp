/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-config/sc_memory_config.hpp>

void PrintStartMessage();

sc_int BuildAndRunBuilder(sc_int argc, sc_char * argv[]);
