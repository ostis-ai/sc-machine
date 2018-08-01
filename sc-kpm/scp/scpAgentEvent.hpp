/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc-memory/cpp/sc_addr.hpp"
#include "sc-memory/cpp/sc_event.hpp"

namespace scp
{

class SCPAgentEvent
{

private:
    sc_result static runSCPAgent(sc_event const * evt, sc_addr edge, sc_addr other_el);
    sc_addr static resolve_sc_addr_from_pointer(sc_pointer data);
    sc_event * m_event;

public:
    SCPAgentEvent(const ScMemoryContext & ctx, const ScAddr & addr, ScEvent::Type eventType, const ScAddr & proc_addr);
    ~SCPAgentEvent();

};

}

