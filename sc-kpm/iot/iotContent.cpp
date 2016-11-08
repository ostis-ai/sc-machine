/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "iotKeynodes.hpp"
#include "iotUtils.hpp"

#include "sc-memory/cpp/sc_memory.hpp"

#include "iotContent.hpp"

namespace iot
{

	SC_AGENT_ACTION_IMPLEMENTATION(AAddContentAgent)
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
			ScAddr arcCommon = mMemoryCtx.createEdge(sc_type_const | sc_type_arc_common, contentSet, deviceAddr);
			assert(arcCommon.isValid());
			ScAddr arc = mMemoryCtx.createEdge(sc_type_arc_pos_const_perm, Keynodes::nrel_content, arcCommon);
			assert(arc.isValid());
		} 
		else
		{
			contentSet = iterContent->value(0);
		}

		// create product instance
        ScAddr product = mMemoryCtx.createNode(sc_type_node_material);
		assert(product.isValid());
		ScAddr arcClass = mMemoryCtx.createEdge(sc_type_arc_pos_const_perm, productClassAddr, product);
		assert(arcClass.isValid());

		Utils::setMass(mMemoryCtx, product, massLinkAddr);

		ScAddr arc = mMemoryCtx.createEdge(sc_type_arc_pos_const_perm, contentSet, product);
		assert(arc.isValid());

		return SC_RESULT_OK;
	}

	// ------------------------------------
	SC_AGENT_ACTION_IMPLEMENTATION(AGetContentAgent)
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
			ScAddr const arc = mMemoryCtx.createEdge(sc_type_arc_pos_const_perm, resultAddr, iterContent->value(2));
			assert(arc.isValid());
		}

		return SC_RESULT_OK;
	}

}