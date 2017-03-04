/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "AUnpressButtonAgent.hpp"

#include <iostream>

namespace bottle
{

SC_AGENT_IMPLEMENTATION(AUnpressButtonAgent)
{
    if (!edgeAddr.IsValid())
        return SC_RESULT_ERROR;

    ScAddr myvalve = ms_context->GetArcEnd(edgeAddr);
    if (!ms_context->HelperCheckArc(Keynodes::valve, myvalve, sc_type_arc_pos_const_perm))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    std::cout << "Valve closed: " << ms_context->HelperGetSystemIdtf(myvalve) << std::endl;

    ScIterator5Ptr iter_valve = ms_context->Iterator5(sc_type_node, SC_TYPE(sc_type_arc_common | sc_type_const), myvalve, sc_type_arc_pos_const_perm, Keynodes::nrel_equipment_connection);
    while (iter_valve->Next())
    {
        ScAddr mybutton = iter_valve->Get(0);
        if (ms_context->HelperCheckArc(Keynodes::button, mybutton, sc_type_arc_pos_const_perm))
        {
            ScIterator3Ptr iter_pressed = ms_context->Iterator3(Keynodes::button_pressed, sc_type_arc_pos_const_perm, mybutton);
            while (iter_pressed->Next())
            {
                ms_context->EraseElement(iter_pressed->Get(1));
            }
            std::cout << "Button unpressed: " << ms_context->HelperGetSystemIdtf(mybutton) << std::endl;
        }
    }

    return SC_RESULT_OK;
}

} // namespace bottle
