/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "iotKeynodes.hpp"
#include "wrap/sc_memory.hpp"

namespace iot
{
	ScAddr Keynodes::device;
	ScAddr Keynodes::device_enabled;
	ScAddr Keynodes::device_energy_usage;
	ScAddr Keynodes::device_standby_energy_usage;
	ScAddr Keynodes::device_real_energy_usage;

	ScAddr Keynodes::command_device_group_enable;
	ScAddr Keynodes::command_device_group_disable;
	ScAddr Keynodes::command_add_content;
	ScAddr Keynodes::command_who_are_you;
	ScAddr Keynodes::command_generate_text_from_template;
	ScAddr Keynodes::command_change_tv_program;

	ScAddr Keynodes::question_get_content;

	ScAddr Keynodes::self;

	ScAddr Keynodes::group_volume;
	ScAddr Keynodes::command_initiated;
	ScAddr Keynodes::command;

	ScAddr Keynodes::question_initiated;

	ScAddr Keynodes::nrel_content;
	ScAddr Keynodes::nrel_mass;
	ScAddr Keynodes::nrel_answer;
	ScAddr Keynodes::nrel_result;
	ScAddr Keynodes::nrel_speech_templates;
	ScAddr Keynodes::nrel_main_idtf;
	ScAddr Keynodes::nrel_tv_program;

	ScAddr Keynodes::rrel_1;
	ScAddr Keynodes::rrel_2;
	ScAddr Keynodes::rrel_3;
	ScAddr Keynodes::rrel_gram;

	ScMemoryContext * Keynodes::memory_ctx = 0;

	void _resolveKeynodeImpl(char const * str, ScAddr & addr, bool & result)
	{
		result = result && Keynodes::resolveKeynode(str, addr);
	}

	bool Keynodes::initialize()
	{
		memory_ctx = new ScMemoryContext(sc_access_lvl_make_min, "IoTKeynodes");
		if (!memory_ctx)
			return false;

		bool result = true;

		_resolveKeynodeImpl("device", device, result);
		_resolveKeynodeImpl("device_enabled", device_enabled, result);
		_resolveKeynodeImpl("nrel_energy_usage", device_energy_usage, result);
		_resolveKeynodeImpl("nrel_standby_energy_usage", device_standby_energy_usage, result);
		_resolveKeynodeImpl("nrel_real_energy_usage", device_real_energy_usage, result);

		_resolveKeynodeImpl("command_device_group_enable", command_device_group_enable, result);
		_resolveKeynodeImpl("command_device_group_disable", command_device_group_disable, result);
		_resolveKeynodeImpl("command_add_content", command_add_content, result);
		_resolveKeynodeImpl("command_who_are_you", command_who_are_you, result);
		_resolveKeynodeImpl("command_generate_text_from_template", command_generate_text_from_template, result);
		_resolveKeynodeImpl("command_change_tv_program", command_change_tv_program, result);

		_resolveKeynodeImpl("question_get_content", question_get_content, result);

		_resolveKeynodeImpl("self", self, result);
		_resolveKeynodeImpl("group_volume", group_volume, result);
		_resolveKeynodeImpl("command_initiated", command_initiated, result);
		_resolveKeynodeImpl("command", command, result);

		_resolveKeynodeImpl("question_initiated", question_initiated, result);

		_resolveKeynodeImpl("nrel_content", nrel_content, result);
		_resolveKeynodeImpl("nrel_mass", nrel_mass, result);
		_resolveKeynodeImpl("nrel_answer", nrel_answer, result);
		_resolveKeynodeImpl("nrel_result", nrel_result, result);
		_resolveKeynodeImpl("nrel_speech_templates", nrel_speech_templates, result);
		_resolveKeynodeImpl("nrel_main_idtf", nrel_main_idtf, result);
		_resolveKeynodeImpl("nrel_tv_program", nrel_tv_program, result);

		_resolveKeynodeImpl("rrel_1", rrel_1, result);
		_resolveKeynodeImpl("rrel_2", rrel_2, result);
		_resolveKeynodeImpl("rrel_3", rrel_3, result);
		_resolveKeynodeImpl("rrel_gram", rrel_gram, result);

		return result;
	}

	bool Keynodes::shutdown()
	{
		if (memory_ctx)
		{
			delete memory_ctx;
			memory_ctx = 0;
		}
		return true;
	}

	bool Keynodes::resolveKeynode(std::string const & sysIdtf, ScAddr & outAddr)
	{
		check_expr(memory_ctx);

		// check if node with system identifier already exists
		if (memory_ctx->helperResolveSystemIdtf(sysIdtf, outAddr))
			return true;

		// create new node with specified system identifier
		outAddr = memory_ctx->createNode(sc_type_node);
		if (outAddr.isValid())
			return memory_ctx->helperSetSystemIdtf(sysIdtf, outAddr);

		return false;
	}
}