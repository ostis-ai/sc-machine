/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "iotKeynodes.hpp"
#include "iotUtils.hpp"

#include "iotAbout.hpp"

namespace iot
{
	SC_AGENT_ACTION_IMPLEMENTATION(AWhoAreYouAgent)
	{
		return mMemoryCtx.createEdge(SC_TYPE(sc_type_arc_pos_const_perm), resultAddr, Keynodes::self).isValid() ? SC_RESULT_OK : SC_RESULT_ERROR;
	}
}