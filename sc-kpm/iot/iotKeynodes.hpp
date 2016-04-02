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

        SC_PROPERTY(Keynode, SysIdtf("device"), ForceCreation)
        static ScAddr device;

        SC_PROPERTY(Keynode, SysIdtf("device_enabled"), ForceCreation)
        static ScAddr device_enabled;

        SC_PROPERTY(Keynode, SysIdtf("nrel_energy_usage"), ForceCreation)
        static ScAddr device_energy_usage;

        SC_PROPERTY(Keynode, SysIdtf("nrel_standby_energy_usage"), ForceCreation)
        static ScAddr device_standby_energy_usage;

        SC_PROPERTY(Keynode, SysIdtf("nrel_real_energy_usage"), ForceCreation)
        static ScAddr device_real_energy_usage;

		/// Commands
        SC_PROPERTY(Keynode, SysIdtf("command_device_group_enable"), ForceCreation)
        static ScAddr command_device_group_enable;

        SC_PROPERTY(Keynode, SysIdtf("command_device_group_disable"), ForceCreation)
        static ScAddr command_device_group_disable;

        SC_PROPERTY(Keynode, SysIdtf("command_add_content"), ForceCreation)
        static ScAddr command_add_content;

        SC_PROPERTY(Keynode, SysIdtf("command_who_are_you"), ForceCreation)
        static ScAddr command_who_are_you;

        SC_PROPERTY(Keynode, SysIdtf("command_generate_text_from_template"), ForceCreation)
        static ScAddr command_generate_text_from_template;

        SC_PROPERTY(Keynode, SysIdtf("command_change_tv_program"), ForceCreation)
        static ScAddr command_change_tv_program;

		/// quesions
        SC_PROPERTY(Keynode, SysIdtf("question_get_content"), ForceCreation)
		static ScAddr question_get_content;
		
		/// common
        SC_PROPERTY(Keynode, SysIdtf("self"), ForceCreation)
		static ScAddr self;

        SC_PROPERTY(Keynode, SysIdtf("group_volume"), ForceCreation)
		static ScAddr group_volume;

        SC_PROPERTY(Keynode, SysIdtf("command_initiated"), ForceCreation)
		static ScAddr command_initiated;

        SC_PROPERTY(Keynode, SysIdtf("command"), ForceCreation)
		static ScAddr command;

        SC_PROPERTY(Keynode, SysIdtf("question_initiated"), ForceCreation)
		static ScAddr question_initiated;

		/// relations
        SC_PROPERTY(Keynode, SysIdtf("nrel_content"), ForceCreation)
		static ScAddr nrel_content;

        SC_PROPERTY(Keynode, SysIdtf("nrel_mass"), ForceCreation)
		static ScAddr nrel_mass;

        SC_PROPERTY(Keynode, SysIdtf("nrel_answer"), ForceCreation)
		static ScAddr nrel_answer;

        SC_PROPERTY(Keynode, SysIdtf("nrel_result"), ForceCreation)
		static ScAddr nrel_result;

        SC_PROPERTY(Keynode, SysIdtf("nrel_speech_templates"), ForceCreation)
		static ScAddr nrel_speech_templates;

        SC_PROPERTY(Keynode, SysIdtf("nrel_main_idtf"), ForceCreation)
		static ScAddr nrel_main_idtf;

        SC_PROPERTY(Keynode, SysIdtf("nrel_tv_program"), ForceCreation)
		static ScAddr nrel_tv_program;

        SC_PROPERTY(Keynode, SysIdtf("rrel_1"), ForceCreation)
		static ScAddr rrel_1;

        SC_PROPERTY(Keynode, SysIdtf("rrel_2"), ForceCreation)
		static ScAddr rrel_2;

        SC_PROPERTY(Keynode, SysIdtf("rrel_3"), ForceCreation)
		static ScAddr rrel_3;

        SC_PROPERTY(Keynode, SysIdtf("rrel_gram"), ForceCreation)
		static ScAddr rrel_gram;

		SC_PROPERTY(Keynode, SysIdtf("binary_float"), ForceCreation)
		static ScAddr binary_float;
	};

}