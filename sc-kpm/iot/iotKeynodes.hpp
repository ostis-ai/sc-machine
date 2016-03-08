/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "wrap/sc_addr.hpp"


class ScMemoryContext;


namespace iot
{
	class Keynodes
	{
	public:

        static ScAddr device;
        static ScAddr device_enabled;
        static ScAddr device_energy_usage;
        static ScAddr device_standby_energy_usage;
        static ScAddr device_real_energy_usage;

		/// Commands
        static ScAddr command_device_group_enable;
        static ScAddr command_device_group_disable;
        static ScAddr command_add_content;
        static ScAddr command_who_are_you;
        static ScAddr command_generate_text_from_template;
        static ScAddr command_change_tv_program;

		/// quesions
		static ScAddr question_get_content;

		
		/// common
		static ScAddr self;

		static ScAddr group_volume;
		static ScAddr command_initiated;
		static ScAddr command_finished;
		static ScAddr command_in_progress;
		static ScAddr command;

		static ScAddr question_initiated;
		static ScAddr question_finished;
		static ScAddr question_in_progress;

		/// relations
		static ScAddr nrel_content;
		static ScAddr nrel_mass;
		static ScAddr nrel_answer;
		static ScAddr nrel_result;
		static ScAddr nrel_speech_templates;
		static ScAddr nrel_main_idtf;
		static ScAddr nrel_tv_program;

		static ScAddr rrel_1;
		static ScAddr rrel_2;
		static ScAddr rrel_3;
		static ScAddr rrel_gram;

		static bool initialize();
		static bool shutdown();
        static bool resolveKeynode(std::string const & sysIdtf, ScAddr & outAddr);

	private:
        static ScMemoryContext * memory_ctx;
	};

}