/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "iotKeynodes.hpp"
#include "wrap/sc_memory.hpp"

namespace iot
{
	sc::Addr Keynodes::device;
	sc::Addr Keynodes::device_enabled;
	sc::Addr Keynodes::device_energy_usage;
	sc::Addr Keynodes::device_standby_energy_usage;
	sc::Addr Keynodes::device_real_energy_usage;

	sc::Addr Keynodes::command_device_group_enable;
	sc::Addr Keynodes::command_device_group_disable;
	sc::Addr Keynodes::command_add_content;

	sc::Addr Keynodes::question_get_content;

	sc::Addr Keynodes::group_volume;
	sc::Addr Keynodes::command_initiated;
	sc::Addr Keynodes::command_finished;
	sc::Addr Keynodes::command_in_progress;

	sc::Addr Keynodes::question_initiated;
	sc::Addr Keynodes::question_finished;
	sc::Addr Keynodes::question_in_progress;

	sc::Addr Keynodes::nrel_content;
	sc::Addr Keynodes::nrel_mass;
	sc::Addr Keynodes::nrel_result;

	sc::Addr Keynodes::rrel_1;
	sc::Addr Keynodes::rrel_2;
	sc::Addr Keynodes::rrel_3;
	sc::Addr Keynodes::rrel_gram;

	sc::MemoryContext * Keynodes::memory_ctx = 0;

	void _resolveKeynodeImpl(char const * str, sc::Addr & addr, bool & result)
	{
		result = result && Keynodes::resolveKeynode(str, addr);
	}

	bool Keynodes::initialize()
	{
		memory_ctx = new sc::MemoryContext(sc_access_lvl_make_min, "IoTKeynodes");
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

		_resolveKeynodeImpl("question_get_content", question_get_content, result);

		_resolveKeynodeImpl("group_volume", group_volume, result);
		_resolveKeynodeImpl("command_initiated", command_initiated, result);
		_resolveKeynodeImpl("command_finished", command_finished, result);
		_resolveKeynodeImpl("command_in_progress", command_in_progress, result);

		_resolveKeynodeImpl("question_initiated", question_initiated, result);
		_resolveKeynodeImpl("question_finished", question_finished, result);
		_resolveKeynodeImpl("question_in_progress", question_in_progress, result);

		_resolveKeynodeImpl("nrel_content", nrel_content, result);
		_resolveKeynodeImpl("nrel_mass", nrel_mass, result);
		_resolveKeynodeImpl("nrel_result", nrel_result, result);

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

	bool Keynodes::resolveKeynode(std::string const & sysIdtf, sc::Addr & outAddr)
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