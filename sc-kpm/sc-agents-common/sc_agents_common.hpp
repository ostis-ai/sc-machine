/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/kpm/sc_module.hpp"

#include "sc-agents-common/keynodes/coreKeynodes.hpp"

SC_MODULE(ScAgentsCommonModule)->Keynodes(new scAgentsCommon::CoreKeynodes());
