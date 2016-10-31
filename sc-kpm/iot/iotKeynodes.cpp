/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "iotKeynodes.hpp"
#include "sc-memory/cpp/sc_memory.hpp"

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

	ScAddr Keynodes::command_get_content;

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
	ScAddr Keynodes::nrel_period;
	ScAddr Keynodes::nrel_time;

	ScAddr Keynodes::rrel_1;
	ScAddr Keynodes::rrel_2;
	ScAddr Keynodes::rrel_3;
	ScAddr Keynodes::rrel_gram;
	ScAddr Keynodes::rrel_seconds;
	ScAddr Keynodes::rrel_epoch;

	ScAddr Keynodes::binary_float;
	ScAddr Keynodes::binary_int8;
	ScAddr Keynodes::binary_int16;
	ScAddr Keynodes::binary_int32;
	ScAddr Keynodes::binary_int64;
}