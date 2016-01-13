/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "iotSpeech.hpp"
#include "iotKeynodes.hpp"
#include "iotUtils.hpp"

#include "wrap/sc_memory.hpp"

namespace iot
{

	IMPLEMENT_AGENT(GenerateSpeechText, COMMAND_AGENT)
	{
		sc::Iterator5Ptr itCmd = mMemoryCtx.iterator5(
			requestAddr,
			SC_TYPE(sc_type_arc_pos_const_perm),
			SC_TYPE(sc_type_node | sc_type_const),
			SC_TYPE(sc_type_arc_pos_const_perm),
			Keynodes::rrel_1
			);

		if (!itCmd->next())
			return SC_RESULT_ERROR_INVALID_PARAMS;
		// got command addr
		sc::Addr const commandAddr = itCmd->value(2);

		sc::Iterator5Ptr itLang = mMemoryCtx.iterator5(
			requestAddr,
			SC_TYPE(sc_type_arc_pos_const_perm),
			SC_TYPE(sc_type_node | sc_type_const),
			SC_TYPE(sc_type_arc_pos_const_perm),
			Keynodes::rrel_2
			);
		if (!itLang->next())
			return SC_RESULT_ERROR_INVALID_PARAMS;

		sc::Addr const langAddr = itLang->value(2);

		sc::Iterator5Ptr itAttr = mMemoryCtx.iterator5(
			requestAddr,
			SC_TYPE(sc_type_arc_pos_const_perm),
			SC_TYPE(sc_type_node | sc_type_const),
			SC_TYPE(sc_type_arc_pos_const_perm),
			Keynodes::rrel_3
			);
		if (!itAttr->next())
			return SC_RESULT_ERROR_INVALID_PARAMS;
		sc::Addr const attrAddr = itAttr->value(2);

		/// TODO: make commond method to get arguments with custom role

		// check if there are speech templates for a specified command
		sc::Iterator5Ptr itTemplatesSet = mMemoryCtx.iterator5(
			SC_TYPE(sc_type_node | sc_type_const | sc_type_node_tuple),
			SC_TYPE(sc_type_arc_common | sc_type_const),
			commandAddr,
			SC_TYPE(sc_type_arc_pos_const_perm),
			Keynodes::nrel_speech_templates
			);

		
		if (!itTemplatesSet->next())
			return SC_RESULT_ERROR_INVALID_PARAMS;
		// got templates set
		sc::Addr const templatesAddr = itTemplatesSet->value(0);
		
		// try to find template for a specified language
		sc::Iterator5Ptr itTempl = mMemoryCtx.iterator5(
			templatesAddr,
			SC_TYPE(sc_type_arc_pos_const_perm),
			SC_TYPE(sc_type_link),
			SC_TYPE(sc_type_arc_pos_const_perm),
			attrAddr
			);

		/// TODO: possible select random template from a set (more then one template for language and result attr)
		while (itTempl->next())
		{
			sc::Addr const linkAddr = itTempl->value(2);
			if (mMemoryCtx.helperCheckArc(langAddr, linkAddr, sc_type_arc_pos_const_perm))
			{
				/// TODO: process template and replace parameters by value

				// for a fast test, just use template as an answer
				sc::Addr const edge = mMemoryCtx.createArc(sc_type_arc_pos_const_perm, resultAddr, linkAddr);
				assert(edge.isValid());

				return SC_RESULT_OK;
			}
		}


		return SC_RESULT_ERROR;
	}

	sc_result handler_generate_text_command(sc_event const * event, sc_addr arg)
	{
		RUN_AGENT(GenerateSpeechText, Keynodes::command_generate_text_from_template, sc_access_lvl_make_min, sc::Addr(arg));
	}

}