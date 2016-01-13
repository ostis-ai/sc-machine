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

#include "wrap/sc_memory.hpp"

sc_event * event_device_group_enable_command = 0;
sc_event * event_add_content_command = 0;

sc_event * event_get_content_question = 0;

sc_event * event_who_are_you = 0;
sc_event * event_generate_text = 0;

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

		bool isEnable = ctx.helperCheckArc(Keynodes::command_device_group_enable, commandInstance, sc_type_arc_pos_const_perm);
		bool isDisable = ctx.helperCheckArc(Keynodes::command_device_group_disable, commandInstance, sc_type_arc_pos_const_perm);

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

		event_add_content_command = sc_event_new(memory_ctx->getRealContext(), Keynodes::command_initiated.getRealAddr(), SC_EVENT_ADD_OUTPUT_ARC, 0, &handler_add_content_command, 0);
		if (!event_add_content_command)
			return false;

		event_get_content_question = sc_event_new(memory_ctx->getRealContext(), Keynodes::question_initiated.getRealAddr(), SC_EVENT_ADD_OUTPUT_ARC, 0, &handler_get_content_question, 0);
		if (!event_get_content_question)
			return false;

		event_who_are_you = sc_event_new(memory_ctx->getRealContext(), Keynodes::command_initiated.getRealAddr(), SC_EVENT_ADD_OUTPUT_ARC, 0, &handler_who_are_you_command, 0);
		if (!event_who_are_you)
			return false;

		event_generate_text = sc_event_new(memory_ctx->getRealContext(), Keynodes::command_initiated.getRealAddr(), SC_EVENT_ADD_OUTPUT_ARC, 0, &handler_generate_text_command, 0);
		if (!event_generate_text)
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

		if (event_add_content_command)
		{
			sc_event_destroy(event_add_content_command);
			event_add_content_command = 0;
		}

		if (event_get_content_question)
		{
			sc_event_destroy(event_get_content_question);
			event_get_content_question = 0;
		}

		if (event_who_are_you)
		{
			sc_event_destroy(event_who_are_you);
			event_who_are_you = 0;
		}

		if (event_generate_text)
		{
			sc_event_destroy(event_generate_text);
			event_generate_text = 0;
		}

		if (memory_ctx)
		{
			delete memory_ctx;
			memory_ctx = 0;
		}

		return true;
	}
}