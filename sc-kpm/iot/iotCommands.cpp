/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "iotCommands.hpp"
#include "iotKeynodes.hpp"
#include "iotContent.hpp"
#include "iotAbout.hpp"
#include "iotSpeech.hpp"
#include "iotTV.hpp"

#include "sc-memory/cpp/sc_memory.hpp"

sc_event * event_device_group_enable_command = 0;

namespace iot
{
    ScMemoryContext * Commands::memory_ctx = 0;

    void handler_device_group_enable_state_command_recursion(ScAddr const & addr, ScMemoryContext & ctx, bool isNeedEnable)
	{
		// if current element is a group_volume, then call the same function recursively for all childs
		if (ctx.helperCheckArc(Keynodes::group_volume, addr, sc_type_arc_pos_const_perm))
		{
			ScIterator3Ptr iterator = ctx.iterator3(addr, sc_type_arc_pos_const_perm, (sc_type)(sc_type_node | sc_type_const));
			while (iterator->next())
				handler_device_group_enable_state_command_recursion(iterator->value(2), ctx, isNeedEnable);
		}
		else
		{
			if (isNeedEnable)
			{
				// check if device doesn't already exists in enabled devices set and then append it
				if (!ctx.helperCheckArc(Keynodes::device_enabled, addr, sc_type_arc_pos_const_perm))
				{
					ctx.createEdge(sc_type_arc_pos_const_perm, Keynodes::device_enabled, addr);
				}
			}
			else
			{
				// disable device
				ScIterator3Ptr it = ctx.iterator3(Keynodes::device_enabled, sc_type_arc_pos_const_perm, addr);
				while (it->next())
					ctx.eraseElement(it->value(1));
			}
		}		
	}

	sc_result handler_device_group_enable_state_command(sc_event const * event, sc_addr arg)
	{
        ScMemoryContext ctx(sc_access_lvl_make_min, "handler_device_group_enable_command");

        ScAddr commandInstance = ctx.getEdgeTarget(ScAddr(arg));
		if (!commandInstance.isValid())
			return SC_RESULT_ERROR;

		bool isEnable = ctx.helperCheckArc(Keynodes::command_device_group_enable, commandInstance, sc_type_arc_pos_const_perm);
		bool isDisable = ctx.helperCheckArc(Keynodes::command_device_group_disable, commandInstance, sc_type_arc_pos_const_perm);

		if (!isDisable && !isEnable)
			return SC_RESULT_ERROR;
		
		ScIterator3Ptr it = ctx.iterator3(commandInstance, sc_type_arc_pos_const_perm, (sc_type)(sc_type_node | sc_type_const));
		while (it->next())
		{
			handler_device_group_enable_state_command_recursion(it->value(2), ctx, !isDisable);
		}

		return SC_RESULT_OK;
	}

	bool Commands::initialize()
	{
        memory_ctx = new ScMemoryContext(sc_access_lvl_make_min, "iotCommands");
		if (!memory_ctx)
			return false;

		event_device_group_enable_command = sc_event_new(memory_ctx->getRealContext(), Keynodes::command_initiated.getRealAddr(), SC_EVENT_ADD_OUTPUT_ARC, 0, &handler_device_group_enable_state_command, 0);
		if (!event_device_group_enable_command)
			return false;

		SC_AGENT_REGISTER(AAddContentAgent)
		SC_AGENT_REGISTER(AGetContentAgent)
		SC_AGENT_REGISTER(ATVChangeProgram)
		SC_AGENT_REGISTER(AWhoAreYouAgent)
		SC_AGENT_REGISTER(AGenerateTextByTemplate)

		/// power
		SC_AGENT_REGISTER(AUpdateUsedPowerSum)

		return true;
	}

	bool Commands::shutdown()
	{

		if (event_device_group_enable_command)
		{
			sc_event_destroy(event_device_group_enable_command);
			event_device_group_enable_command = 0;
		}

		SC_AGENT_UNREGISTER(AAddContentAgent)
		SC_AGENT_UNREGISTER(AGetContentAgent)
		SC_AGENT_UNREGISTER(ATVChangeProgram)
		SC_AGENT_UNREGISTER(AWhoAreYouAgent)
		SC_AGENT_UNREGISTER(AGenerateTextByTemplate)

		/// Power
		SC_AGENT_UNREGISTER(AUpdateUsedPowerSum)

		if (memory_ctx)
		{
			delete memory_ctx;
			memory_ctx = 0;
		}

		return true;
	}
}