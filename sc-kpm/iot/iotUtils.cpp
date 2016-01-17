/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "iotUtils.hpp"
#include "iotKeynodes.hpp"

#include "wrap/sc_memory.hpp"
#include "wrap/sc_stream.hpp"

namespace iot
{
	namespace Utils
	{

		bool addToSet(sc::MemoryContext & ctx, sc::Addr const & setAddr, sc::Addr const & elAddr)
		{
			if (ctx.helperCheckArc(setAddr, elAddr, sc_type_arc_pos_const_perm))
				return false;

			sc::Addr arcAddr = ctx.createArc(sc_type_arc_pos_const_perm, setAddr, elAddr);
			assert(arcAddr.isValid());
			return true;
		}

		bool removeFromSet(sc::MemoryContext & ctx, sc::Addr const & setAddr, sc::Addr const & elAddr)
		{
			sc::Iterator3Ptr it = ctx.iterator3(setAddr, sc_type_arc_pos_const_perm, elAddr);
			bool result = false;
			while (it->next())
				ctx.eraseElement(it->value(1));

			return result;
		}

		void setMass(sc::MemoryContext & ctx, sc::Addr const & objAddr, sc::Addr const & valueAddr)
		{
			sc::Addr massAddr;

			sc::Iterator5Ptr itMass = ctx.iterator5(objAddr,
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
				sc::Addr arcCommon = ctx.createArc(sc_type_const | sc_type_arc_common, objAddr, massAddr);
				assert(arcCommon.isValid());
				sc::Addr arc = ctx.createArc(sc_type_arc_pos_const_perm, Keynodes::nrel_mass, arcCommon);
				assert(arc.isValid());
			}

			sc::Iterator5Ptr itValue = ctx.iterator5(massAddr,
				SC_TYPE(sc_type_arc_pos_const_perm),
				SC_TYPE(sc_type_link),
				SC_TYPE(sc_type_arc_pos_const_perm),
				Keynodes::rrel_gram);

			if (itValue->next())
			{
				sc::Addr linkAddr = itValue->value(2);
				sc::Stream stream;
				bool res = ctx.getLinkContent(valueAddr, stream);
				assert(res);
				res = ctx.setLinkContent(linkAddr, stream);
				assert(res);
			}
			else
			{
				sc::Addr arc = ctx.createArc(sc_type_arc_pos_const_perm, massAddr, valueAddr);
				assert(arc.isValid());
				arc = ctx.createArc(sc_type_arc_pos_const_perm, Keynodes::rrel_gram, arc);
				assert(arc.isValid());
			}

		}


		sc::Addr findMainIdtf(sc::MemoryContext & ctx, sc::Addr const & elAddr, sc::Addr const langAddr)
		{
			assert(elAddr.isValid());
			assert(langAddr.isValid());

			sc::Addr result;
			sc::Iterator5Ptr it5 = ctx.iterator5(
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