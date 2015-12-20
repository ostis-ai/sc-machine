/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "iotCommands.hpp"
#include "iotKeynodes.hpp"

#include "wrap/sc_memory.hpp"

sc_event * event_device_group_enable_command = 0;

namespace iot
{
	sc::MemoryContext * Commands::memory_ctx = 0;

	void handler_device_group_enable_state_command_recursion(sc::Addr const & addr, sc::MemoryContext & ctx, bool isNeedEnable)
	{
		// if current element is a group_volume, then call the same function recursively for all childs
		if (ctx.helperCheckArc(Keynodes::group_volume, addr, sc_type_arc_pos_const_perm))
		{
			sc::Iterator3Ptr iterator = ctx.iterator3(addr, sc_type_arc_pos_const_perm, (sc_type)(sc_type_node | sc_type_const));
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
					ctx.createArc(sc_type_arc_pos_const_perm, Keynodes::device_enabled, addr);
				}
			}
			else
			{
				// disable device
				sc::Iterator3Ptr it = ctx.iterator3(Keynodes::device_enabled, sc_type_arc_pos_const_perm, addr);
				while (it->next())
					ctx.eraseElement(it->value(1));
			}
		}


		
	}

	sc_result handler_device_group_enable_state_command(sc_event const * event, sc_addr arg)
	{
		sc::MemoryContext ctx(sc_access_lvl_make_min, "handler_device_group_enable_command");

		sc::Addr commandInstance = ctx.getArcEnd(sc::Addr(arg));
		if (!commandInstance.isValid())
			return SC_RESULT_ERROR;

		bool isEnable = ctx.helperCheckArc(Keynodes::device_group_enable_command, commandInstance, sc_type_arc_pos_const_perm);
		bool isDisable = ctx.helperCheckArc(Keynodes::device_group_disable_command, commandInstance, sc_type_arc_pos_const_perm);

		if (!isDisable && !isEnable)
			return SC_RESULT_ERROR;
		
		sc::Iterator3Ptr it = ctx.iterator3(commandInstance, sc_type_arc_pos_const_perm, (sc_type)(sc_type_node | sc_type_const));
		while (it->next())
		{
			handler_device_group_enable_state_command_recursion(it->value(2), ctx, !isDisable);
		}

		return SC_RESULT_OK;
	}

	bool Commands::initialize()
	{
		memory_ctx = new sc::MemoryContext(sc_access_lvl_make_min, "iotCommands");
		if (!memory_ctx)
			return false;

		event_device_group_enable_command = sc_event_new(memory_ctx->getRealContext(), Keynodes::command_initiated.getRealAddr(), SC_EVENT_ADD_OUTPUT_ARC, 0, &handler_device_group_enable_state_command, 0);
		if (!event_device_group_enable_command)
			return false;

		return true;
	}

	bool Commands::shutdown()
	{

		if (event_device_group_enable_command)
		{
			sc_event_destroy(event_device_group_enable_command);
			event_device_group_enable_command = 0;
		}

		if (memory_ctx)
		{
			delete memory_ctx;
			memory_ctx = 0;
		}

		return true;
	}
}