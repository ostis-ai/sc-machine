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
		static sc::Addr command_device_group_enable;
		static sc::Addr command_device_group_disable;
		static sc::Addr command_add_content;
		static sc::Addr command_who_are_you;
		static sc::Addr command_generate_text_from_template;

		/// quesions
		static sc::Addr question_get_content;

		/// common
		static sc::Addr group_volume;
		static sc::Addr command_initiated;
		static sc::Addr command_finished;
		static sc::Addr command_in_progress;

		static sc::Addr question_initiated;
		static sc::Addr question_finished;
		static sc::Addr question_in_progress;

		/// relations
		static sc::Addr nrel_content;
		static sc::Addr nrel_mass;
		static sc::Addr nrel_answer;
		static sc::Addr nrel_result;
		static sc::Addr nrel_speech_templates;

		static sc::Addr rrel_1;
		static sc::Addr rrel_2;
		static sc::Addr rrel_3;
		static sc::Addr rrel_gram;

		static bool initialize();
		static bool shutdown();
		static bool resolveKeynode(std::string const & sysIdtf, sc::Addr & outAddr);

	private:
		static sc::MemoryContext * memory_ctx;
	};

}