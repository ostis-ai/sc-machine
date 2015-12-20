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

	sc::Addr Keynodes::device_group_enable_command;
	sc::Addr Keynodes::device_group_disable_command;

	sc::Addr Keynodes::group_volume;
	sc::Addr Keynodes::command_initiated;

	sc::MemoryContext * Keynodes::memory_ctx = 0;

	bool Keynodes::initialize()
	{
		memory_ctx = new sc::MemoryContext(sc_access_lvl_make_min, "IoTKeynodes");
		if (!memory_ctx)
			return false;

		bool result = true;

		result = result && resolveKeynode("device", device);
		result = result && resolveKeynode("device_enabled", device_enabled);
		result = result && resolveKeynode("nrel_energy_usage", device_energy_usage);
		result = result && resolveKeynode("nrel_standby_energy_usage", device_standby_energy_usage);
		result = result && resolveKeynode("nrel_real_energy_usage", device_real_energy_usage);

		result = result && resolveKeynode("device_group_enable_command", device_group_enable_command);
		result = result && resolveKeynode("device_group_disable_command", device_group_disable_command);

		result = result && resolveKeynode("group_volume", group_volume);
		result = result && resolveKeynode("command_initiated", command_initiated);

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