/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "iotContent.hpp"
#include "iotKeynodes.hpp"
#include "iotUtils.hpp"

#include "wrap/sc_memory.hpp"

namespace iot
{

	IMPLEMENT_AGENT(AddContent, KPM_COMMAND_AGENT)
	{
		assert(requestAddr.isValid());

		// determine device class
		ScIterator5Ptr iterDevice = mMemoryCtx.iterator5(requestAddr,
			SC_TYPE(sc_type_arc_pos_const_perm),
			SC_TYPE(sc_type_const | sc_type_node | sc_type_node_material),
			SC_TYPE(sc_type_arc_pos_const_perm),
			Keynodes::rrel_1);

		if (!iterDevice->next())
			return SC_RESULT_ERROR_INVALID_PARAMS;

        ScAddr deviceAddr = iterDevice->value(2);

		// determine product class
		ScIterator5Ptr iterProductClass = mMemoryCtx.iterator5(requestAddr,
			SC_TYPE(sc_type_arc_pos_const_perm),
			SC_TYPE(sc_type_const | sc_type_node | sc_type_node_class),
			SC_TYPE(sc_type_arc_pos_const_perm),
			Keynodes::rrel_2);

		if (!iterProductClass->next())
			return SC_RESULT_ERROR_INVALID_PARAMS;

        ScAddr productClassAddr = iterProductClass->value(2);

		// determine mass
        ScIterator5Ptr iterMass = mMemoryCtx.iterator5(requestAddr,
			SC_TYPE(sc_type_arc_pos_const_perm),
			SC_TYPE(sc_type_link),
			SC_TYPE(sc_type_arc_pos_const_perm),
			Keynodes::rrel_3);

		if (!iterMass->next())
			return SC_RESULT_ERROR_INVALID_PARAMS;

        ScAddr massLinkAddr = iterMass->value(2);

		// create content set if it doesn't exists
        ScIterator5Ptr iterContent = mMemoryCtx.iterator5(
			SC_TYPE(sc_type_node | sc_type_const | sc_type_node_tuple),
			SC_TYPE(sc_type_const | sc_type_arc_common),
			deviceAddr,
			SC_TYPE(sc_type_arc_pos_const_perm),
			Keynodes::nrel_content);

        ScAddr contentSet;
		if (!iterContent->next())
		{
			contentSet = mMemoryCtx.createNode(sc_type_const | sc_type_node_tuple);
			assert(contentSet.isValid());
            ScAddr arcCommon = mMemoryCtx.createArc(sc_type_const | sc_type_arc_common, contentSet, deviceAddr);
			assert(arcCommon.isValid());
            ScAddr arc = mMemoryCtx.createArc(sc_type_arc_pos_const_perm, Keynodes::nrel_content, arcCommon);
			assert(arc.isValid());
		} 
		else
		{
			contentSet = iterContent->value(0);
		}

		// create product instance
        ScAddr product = mMemoryCtx.createNode(sc_type_node_material);
		assert(product.isValid());
        ScAddr arcClass = mMemoryCtx.createArc(sc_type_arc_pos_const_perm, productClassAddr, product);
		assert(arcClass.isValid());

		Utils::setMass(mMemoryCtx, product, massLinkAddr);

        ScAddr arc = mMemoryCtx.createArc(sc_type_arc_pos_const_perm, contentSet, product);
		assert(arc.isValid());

		return SC_RESULT_OK;
	}

	// ------------------------------------
	IMPLEMENT_AGENT(GetContent, KPM_QUESTION_AGENT)
	{
		assert(requestAddr.isValid());

		ScIterator3Ptr iter = mMemoryCtx.iterator3(requestAddr,
			SC_TYPE(sc_type_arc_pos_const_perm),
			SC_TYPE(sc_type_node | sc_type_const | sc_type_node_material));

		if (!iter->next())
			return SC_RESULT_ERROR_INVALID_PARAMS;

        ScAddr const deviceAddr = iter->value(2);
		ScIterator5Ptr iter5 = mMemoryCtx.iterator5(
			SC_TYPE(sc_type_const | sc_type_node | sc_type_node_tuple),
			SC_TYPE(sc_type_const | sc_type_arc_common),
			deviceAddr,
			SC_TYPE(sc_type_arc_pos_const_perm),
			Keynodes::nrel_content);

		if (!iter5->next())
			return SC_RESULT_ERROR;

        ScAddr const contentSet = iter5->value(0);

		ScIterator3Ptr iterContent = mMemoryCtx.iterator3(
			contentSet,
			SC_TYPE(sc_type_arc_pos_const_perm),
			SC_TYPE(sc_type_const | sc_type_node | sc_type_node_material));

		while (iterContent->next())
		{
            ScAddr const arc = mMemoryCtx.createArc(sc_type_arc_pos_const_perm, resultAddr, iterContent->value(2));
			assert(arc.isValid());
		}

		return SC_RESULT_OK;
	}


	// --------- Handlers ---------
	sc_result handler_add_content_command(sc_event const * event, sc_addr arg)
	{
        RUN_AGENT(AddContent, Keynodes::command_add_content, sc_access_lvl_make_min, ScAddr(arg));
	}

	sc_result handler_get_content_question(sc_event const * event, sc_addr arg)
	{
        RUN_AGENT(GetContent, Keynodes::question_get_content, sc_access_lvl_make_min, ScAddr(arg));
	}

}