/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "AFillBottle.hpp"
#include "sc-memory/cpp/sc_memory.hpp"
#include "sc-memory/cpp/sc_stream.hpp"

#include <iostream>
#include <sstream>
#include <thread>

namespace bottle
{

int readInt(ScMemoryContext &ms_context, ScAddr &addr)
{
    ScStream stream;
    if (ms_context.GetLinkContent(addr, stream))
    {
        std::string str;
        if (ScStreamConverter::StreamToString(stream, str))
        {
            std::stringstream ss(str);
            int result;
            ss >> result;
            return result;
        }
    }
    return -1;
}

void writeInt(ScMemoryContext &ms_context, ScAddr &addr, int value)
{
    std::stringstream ss;
    ss << value;
    std::string str = ss.str();
    ScStream write_stream((sc_char*)str.c_str(), str.length(), SC_STREAM_FLAG_READ);
    ms_context.SetLinkContent(addr, write_stream);
}

int getIntParameter(ScMemoryContext &ms_context, ScAddr &elem, ScAddr &param)
{
    ScTemplate mytempl;
    mytempl.Triple(
        ScType::NodeVar >> "_quantity",
        ScType::EdgeAccessVarPosPerm,
        elem
    );
    mytempl.Triple(
        param,
        ScType::EdgeAccessVarPosPerm,
        "_quantity"
    );
    mytempl.TripleWithRelation(
        "_quantity",
        ScType::EdgeDCommonVar,
        ScType::NodeVar >> "_number",
        ScType::EdgeAccessVarPosPerm,
        Keynodes::measurement_of_fixed_measure
    );
    ScTemplateSearchResult result;
    ms_context.HelperSearchTemplate(mytempl, result);
    if (result.Size() > 0)
    {
        ScTemplateSearchResultItem item = result[0];
        ScAddr number = item["_number"];
        ScIterator5Ptr iter_value = ms_context.Iterator5(number, SC_TYPE(sc_type_arc_common | sc_type_const), SC_TYPE(0), sc_type_arc_pos_const_perm, Keynodes::nrel_idtf);
        if (iter_value->Next())
        {
            ScAddr value = iter_value->Get(2);
            if (ms_context.HelperCheckArc(Keynodes::decimal_numeral_system, value, sc_type_arc_pos_const_perm))
            {
                return readInt((ScMemoryContext&)ms_context, value);
            }
        }
    }
    return -1;
}

SC_AGENT_IMPLEMENTATION(AFillBottle)
{
    if (!edgeAddr.IsValid())
        return SC_RESULT_ERROR;

    ScAddr myvalve = ms_context->GetArcEnd(edgeAddr);
    if (!ms_context->HelperCheckArc(Keynodes::valve, myvalve, sc_type_arc_pos_const_perm))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    int speed = getIntParameter((ScMemoryContext&)ms_context, myvalve, Keynodes::pass_speed);

    ScIterator5Ptr iter_valve = ms_context->Iterator5(myvalve, SC_TYPE(sc_type_arc_common | sc_type_const), sc_type_node, sc_type_arc_pos_const_perm, Keynodes::nrel_equipment_connection);
    while (iter_valve->Next())
    {
        ScAddr mybottle = iter_valve->Get(2);
        if (ms_context->HelperCheckArc(Keynodes::bottle, mybottle, sc_type_arc_pos_const_perm))
        {
            std::cout << "Filling bottle: " << ms_context->HelperGetSystemIdtf(mybottle) << std::endl;

            int max_capacity = getIntParameter((ScMemoryContext&)ms_context, mybottle, Keynodes::max_capacity);

            ScTemplate mytempl;
            mytempl.TripleWithRelation(
                mybottle,
                ScType::EdgeDCommonVar,
                ScType::NodeVar >> "_scales",
                ScType::EdgeAccessVarPosPerm,
                Keynodes::nrel_direct_contact
            );
            mytempl.Triple(
                Keynodes::scales,
                ScType::EdgeAccessVarPosPerm,
                "_scales"
            );
            mytempl.Triple(
                ScType::NodeVar >> "_quantity",
                ScType::EdgeAccessVarPosPerm,
                "_scales"
            );
            mytempl.Triple(
                Keynodes::registration,
                ScType::EdgeAccessVarPosPerm,
                "_quantity"
            );
            mytempl.TripleWithRelation(
                "_quantity",
                ScType::EdgeDCommonVar,
                ScType::NodeVar >> "_number",
                ScType::EdgeAccessVarPosPerm,
                Keynodes::measurement_of_fixed_measure
            );

            ScTemplateSearchResult result;
            ms_context->HelperSearchTemplate(mytempl, result);
            //std::cout << "Search executed" << std::endl;
            if (result.Size() > 0)
            {
                ScTemplateSearchResultItem item = result[0];
                ScAddr number = item["_number"];
                ScIterator5Ptr iter_value = ms_context->Iterator5(number, SC_TYPE(sc_type_arc_common | sc_type_const), SC_TYPE(0), sc_type_arc_pos_const_perm, Keynodes::nrel_idtf);
                if (iter_value->Next())
                {
                    ScAddr value = iter_value->Get(2);
                    if (ms_context->HelperCheckArc(Keynodes::decimal_numeral_system, value, sc_type_arc_pos_const_perm))
                    {
                        //Creating event
                        auto const processChange = [&ms_context, &myvalve, &max_capacity](ScAddr const & listenAddr,
                                                   ScAddr const & edgeAddr,
                                                   ScAddr const & otherAddr)
                        {
                            ScAddr localAddr = listenAddr;
                            int curr_value = readInt((ScMemoryContext&)ms_context, localAddr);
                            std::cout << curr_value << std::endl;
                            if (curr_value >= max_capacity)
                            {
                                ScIterator3Ptr iter_open = ms_context->Iterator3(Keynodes::opened, sc_type_arc_pos_const_perm, myvalve);
                                while (iter_open->Next())
                                {
                                    ms_context->EraseElement(iter_open->Get(1));
                                }
                                ms_context->CreateArc(sc_type_arc_pos_const_perm, Keynodes::closed, myvalve);
                            }
                            return true;
                        };
                        ScEventContentChanged myevent((ScMemoryContext&)ms_context, value, processChange);

                        for (int i = 0; i < 1000; i++)
                        {
                            if (ms_context->HelperCheckArc(Keynodes::closed, myvalve, sc_type_arc_pos_const_perm))
                                break;

                            int count = readInt((ScMemoryContext&)ms_context, value);
                            //std::cout << count << std::endl;
                            writeInt((ScMemoryContext&)ms_context, value, count + (speed / 2));
                            std::this_thread::sleep_for(std::chrono::milliseconds(500));
                        }
                    }
                }
            }
        }
    }

    return SC_RESULT_OK;
}

} // namespace bottle
