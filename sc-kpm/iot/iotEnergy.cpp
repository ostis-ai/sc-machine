/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "iotKeynodes.hpp"

#include "sc-memory/cpp/sc_stream.hpp"

#include "iotEnergy.hpp"

namespace iot
{
	struct UsedPowerSum
	{
		UsedPowerSum(ScAddr const & groupAddr, ScMemoryContext & ctx)
			: mGroupAddr(groupAddr)
			, mMemoryCtx(ctx)
		{
		}

		float operator() ()
		{
			float result = 0.f;
			// calculate for child groups
			ScIterator3Ptr itGroups = mMemoryCtx.iterator3(
				mGroupAddr,
				SC_TYPE(sc_type_arc_pos_const_perm),
				SC_TYPE(sc_type_node | sc_type_const | sc_type_node_class));

			while (itGroups->next())
			{
				UsedPowerSum sum(itGroups->value(2), mMemoryCtx);
				result += sum();
			}

			// calculate sum of used power for devices in this group (without childs)
			// search template: 
			//		mGroupAddr _-> _device;;
			//		_device _=> nrel_real_energy_usage: _link;;

			/// TODO: use transitive relation to device set
			ScTemplate templDevice;
			templDevice
				.triple(
					mGroupAddr,
					ScType(sc_type_arc_pos_const_perm),
					ScType(sc_type_node | sc_type_const | sc_type_node_material) >> "device")
				.triple(
					"device",
					ScType(sc_type_arc_common | sc_type_const) >> "edge",
					ScType(sc_type_link) >> "link")
				.triple(
					Keynodes::device_real_energy_usage,
					ScType(sc_type_arc_pos_const_perm),
					"edge"
				);

			// Devices
			ScTemplateSearchResult searchResult;
			if (mMemoryCtx.helperSearchTemplate(templDevice, searchResult))
			{
				size_t const resultNum = searchResult.getSize();
				for (size_t i = 0; i < resultNum; ++i)
				{
					ScTemplateSearchResultItem const res = searchResult[i];

					/// TODO: implement support of different types (int, float, ...)
					ScAddr const & link = res["link"];
					ScStream stream;
					if (mMemoryCtx.getLinkContent(link, stream) && (stream.size() == sizeof(float)))
					{
						float value = 0.f;
						if (stream.readType(value))
							result += value;
					}
				}
			}

			// update value for a current group

			ScTemplate templPowerUsage;
			templPowerUsage
				.triple(
					mGroupAddr,
					ScType(sc_type_arc_common | sc_type_const) >> "edge",
					ScType(sc_type_link) >> "link")
				.triple(
					Keynodes::device_real_energy_usage,
					ScType(sc_type_arc_pos_const_perm),
					"edge")
				.triple(
					Keynodes::binary_float,
					ScType(sc_type_arc_pos_const_perm),
					"link"
					);

			ScAddr linkAddr;
			if (mMemoryCtx.helperSearchTemplate(templPowerUsage, searchResult))
			{
				linkAddr = searchResult[0]["link"];
			}
			else
			{
				ScTemplateGenResult genResult;
				if (mMemoryCtx.helperGenTemplate(templPowerUsage, genResult))
				{
					linkAddr = genResult["link"];
				}
			}

			if (linkAddr.isValid())
			{
				ScStream stream((sc_char*)&result, sizeof(result), SC_STREAM_FLAG_READ);
				mMemoryCtx.setLinkContent(linkAddr, stream);
			}
			
			return result;
		}

	protected:
		ScAddr const & mGroupAddr;
		ScMemoryContext & mMemoryCtx;
	};


	SC_AGENT_ACTION_IMPLEMENTATION(AUpdateUsedPowerSum)
	{
		// update all device groups, that in params
		ScIterator3Ptr itGroups = mMemoryCtx.iterator3(requestAddr,
										SC_TYPE(sc_type_arc_pos_const_perm),
										SC_TYPE(sc_type_node | sc_type_const | sc_type_node_class));
		
		while (itGroups->next())
		{
			UsedPowerSum sum(itGroups->value(2), mMemoryCtx);
			sum();
		}

		return SC_RESULT_ERROR;
	}
}