/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "AOpenValveAgent.hpp"

#include <iostream>

namespace bottle
{

SC_AGENT_IMPLEMENTATION(AOpenValveAgent)
{
    if (!edgeAddr.IsValid())
        return SC_RESULT_ERROR;

    ScAddr mybutton = ms_context->GetArcEnd(edgeAddr);
    if (!ms_context->HelperCheckArc(Keynodes::button, mybutton, sc_type_arc_pos_const_perm))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    std::cout << "Button pressed: " << ms_context->HelperGetSystemIdtf(mybutton) << std::endl;

    ScIterator5Ptr iter_valve = ms_context->Iterator5(mybutton, SC_TYPE(sc_type_arc_common | sc_type_const), sc_type_node, sc_type_arc_pos_const_perm, Keynodes::nrel_equipment_connection);
    while (iter_valve->Next())
    {
        ScAddr myvalve = iter_valve->Get(2);
        if (ms_context->HelperCheckArc(Keynodes::valve, myvalve, sc_type_arc_pos_const_perm))
            ms_context->CreateArc(sc_type_arc_pos_const_perm, Keynodes::opened, myvalve);

        std::cout << "Valve opened: " << ms_context->HelperGetSystemIdtf(myvalve) << std::endl;
    }

    return SC_RESULT_OK;
}

} // namespace bottle
