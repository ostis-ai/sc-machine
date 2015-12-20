/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "wrap/sc_addr.hpp"


namespace sc
{
	class MemoryContext;
}

namespace iot
{
	class Keynodes
	{
	public:

		static sc::Addr device;
		static sc::Addr device_enabled;
		static sc::Addr device_energy_usage;
		static sc::Addr device_standby_energy_usage;
		static sc::Addr device_real_energy_usage;

		/// Commands
		static sc::Addr device_group_enable_command;
		static sc::Addr device_group_disable_command;

		/// common
		static sc::Addr group_volume;
		static sc::Addr command_initiated;

		static bool initialize();
		static bool shutdown();
		static bool resolveKeynode(std::string const & sysIdtf, sc::Addr & outAddr);

	private:
		static sc::MemoryContext * memory_ctx;
	};

}