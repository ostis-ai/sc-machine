/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "wrap/sc_addr.hpp"
#include "wrap/sc_object.hpp"

#include "iotKeynodes.generated.hpp"

namespace iot
{
    SC_CLASS()
	class Keynodes : public ScObject
	{
        SC_GENERATED_BODY()

	public:

        SC_PROPERTY(Keynode, SysIdtf("device"))
        static ScAddr device;

        SC_PROPERTY(Keynode, SysIdtf("device_enabled"))
        static ScAddr device_enabled;

        SC_PROPERTY(Keynode, SysIdtf("nrel_energy_usage"))
        static ScAddr device_energy_usage;

        SC_PROPERTY(Keynode, SysIdtf("nrel_standby_energy_usage"))
        static ScAddr device_standby_energy_usage;

        SC_PROPERTY(Keynode, SysIdtf("nrel_real_energy_usage"))
        static ScAddr device_real_energy_usage;

		/// Commands
        SC_PROPERTY(Keynode, SysIdtf("command_device_group_enable"))
        static ScAddr command_device_group_enable;

        SC_PROPERTY(Keynode, SysIdtf("command_device_group_disable"))
        static ScAddr command_device_group_disable;

        SC_PROPERTY(Keynode, SysIdtf("command_add_content"))
        static ScAddr command_add_content;

        SC_PROPERTY(Keynode, SysIdtf("command_who_are_you"))
        static ScAddr command_who_are_you;

        SC_PROPERTY(Keynode, SysIdtf("command_generate_text_from_template"))
        static ScAddr command_generate_text_from_template;

        SC_PROPERTY(Keynode, SysIdtf("command_change_tv_program"))
        static ScAddr command_change_tv_program;

		/// quesions
        SC_PROPERTY(Keynode, SysIdtf("question_get_content"))
		static ScAddr question_get_content;
		
		/// common
        SC_PROPERTY(Keynode, SysIdtf("self"))
		static ScAddr self;

        SC_PROPERTY(Keynode, SysIdtf("group_volume"))
		static ScAddr group_volume;

        SC_PROPERTY(Keynode, SysIdtf("command_initiated"))
		static ScAddr command_initiated;

        SC_PROPERTY(Keynode, SysIdtf("command"))
		static ScAddr command;

        SC_PROPERTY(Keynode, SysIdtf("question_initiated"))
		static ScAddr question_initiated;

		/// relations
        SC_PROPERTY(Keynode, SysIdtf("nrel_content"))
		static ScAddr nrel_content;

        SC_PROPERTY(Keynode, SysIdtf("nrel_mass"))
		static ScAddr nrel_mass;

        SC_PROPERTY(Keynode, SysIdtf("nrel_answer"))
		static ScAddr nrel_answer;

        SC_PROPERTY(Keynode, SysIdtf("nrel_result"))
		static ScAddr nrel_result;

        SC_PROPERTY(Keynode, SysIdtf("nrel_speech_templates"))
		static ScAddr nrel_speech_templates;

        SC_PROPERTY(Keynode, SysIdtf("nrel_main_idtf"))
		static ScAddr nrel_main_idtf;

        SC_PROPERTY(Keynode, SysIdtf("nrel_tv_program"))
		static ScAddr nrel_tv_program;

        SC_PROPERTY(Keynode, SysIdtf("rrel_1"))
		static ScAddr rrel_1;

        SC_PROPERTY(Keynode, SysIdtf("rrel_2"))
		static ScAddr rrel_2;

        SC_PROPERTY(Keynode, SysIdtf("rrel_3"))
		static ScAddr rrel_3;

        SC_PROPERTY(Keynode, SysIdtf("rrel_gram"))
		static ScAddr rrel_gram;
	};

}