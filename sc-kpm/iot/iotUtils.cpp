/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "iotUtils.hpp"
#include "iotKeynodes.hpp"

#include "sc-memory/cpp/sc_memory.hpp"
#include "sc-memory/cpp/sc_stream.hpp"

namespace iot
{
	namespace Utils
	{

        bool addToSet(ScMemoryContext & ctx, ScAddr const & setAddr, ScAddr const & elAddr)
		{
			if (ctx.helperCheckArc(setAddr, elAddr, sc_type_arc_pos_const_perm))
				return false;

			ScAddr arcAddr = ctx.createEdge(sc_type_arc_pos_const_perm, setAddr, elAddr);
			assert(arcAddr.isValid());
			return true;
		}

        bool removeFromSet(ScMemoryContext & ctx, ScAddr const & setAddr, ScAddr const & elAddr)
		{
			ScIterator3Ptr it = ctx.iterator3(setAddr, sc_type_arc_pos_const_perm, elAddr);
			bool result = false;
			while (it->next())
				ctx.eraseElement(it->value(1));

			return result;
		}

		void setMass(ScMemoryContext & ctx, ScAddr const & objAddr, ScAddr const & valueAddr)
		{
			ScAddr massAddr;

			ScIterator5Ptr itMass = ctx.iterator5(objAddr,
				SC_TYPE(sc_type_arc_common | sc_type_const),
				SC_TYPE(sc_type_const | sc_type_node | sc_type_node_abstract),
				SC_TYPE(sc_type_arc_pos_const_perm),
				Keynodes::nrel_mass);

			/// TODO: check if there is a more than one result
			if (itMass->next())
			{
				massAddr = itMass->value(2);
			}
			else
			{
				massAddr = ctx.createNode(sc_type_const | sc_type_node_abstract);
				assert(massAddr.isValid());
				ScAddr arcCommon = ctx.createEdge(sc_type_const | sc_type_arc_common, objAddr, massAddr);
				assert(arcCommon.isValid());
				ScAddr arc = ctx.createEdge(sc_type_arc_pos_const_perm, Keynodes::nrel_mass, arcCommon);
				assert(arc.isValid());
			}

			ScIterator5Ptr itValue = ctx.iterator5(massAddr,
				SC_TYPE(sc_type_arc_pos_const_perm),
				SC_TYPE(sc_type_link),
				SC_TYPE(sc_type_arc_pos_const_perm),
				Keynodes::rrel_gram);

			if (itValue->next())
			{
                ScAddr linkAddr = itValue->value(2);
                ScStream stream;
				bool res = ctx.getLinkContent(valueAddr, stream);
				assert(res);
				res = ctx.setLinkContent(linkAddr, stream);
				assert(res);
			}
			else
			{
				ScAddr arc = ctx.createEdge(sc_type_arc_pos_const_perm, massAddr, valueAddr);
				assert(arc.isValid());
				arc = ctx.createEdge(sc_type_arc_pos_const_perm, Keynodes::rrel_gram, arc);
				assert(arc.isValid());
			}

		}


        ScAddr findMainIdtf(ScMemoryContext & ctx, ScAddr const & elAddr, ScAddr const langAddr)
		{
			assert(elAddr.isValid());
			assert(langAddr.isValid());

            ScAddr result;
			ScIterator5Ptr it5 = ctx.iterator5(
				elAddr,
				SC_TYPE(sc_type_arc_common | sc_type_const),
				SC_TYPE(sc_type_link),
				SC_TYPE(sc_type_arc_pos_const_perm),
				Keynodes::nrel_main_idtf
				);

			while (it5->next())
			{
				if (ctx.helperCheckArc(langAddr, it5->value(2), sc_type_arc_pos_const_perm))
				{
					result = it5->value(2);
					break;
				}
			}

			return result;
		}
	}
}