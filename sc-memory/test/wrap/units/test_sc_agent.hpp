/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "wrap/sc_addr.hpp"
#include "wrap/sc_object.hpp"
#include "wrap/kpm/sc_agent.hpp"

#include "test_sc_agent.generated.hpp"


class TestAgent : public ScAgentAction
{
	SC_CLASS(Agent, CmdClass("command_who_are_you"))
	SC_GENERATED_BODY()
};