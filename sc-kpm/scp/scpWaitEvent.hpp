/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc-memory/cpp/sc_addr.hpp"
#include "sc-memory/cpp/sc_event.hpp"
#include "concurrent_queue.hpp"

namespace scp
{

class SCPWaitEvent
{

private:
    sc_addr static resolve_sc_addr_from_pointer(sc_pointer data);
    static sc_result Run(sc_event const * evt, sc_addr edge, sc_addr other_el);
    sc_event_type ConvertEventType(ScEvent::Type type);
    sc_event * m_event;

public:
    static concurrent_deque<SCPWaitEvent*> sys_wait_events;
    void static unregister_all_sys_wait();
    SCPWaitEvent(const ScMemoryContext & ctx, const ScAddr & addr, ScEvent::Type eventType, const ScAddr & param_addr);
    ScAddr GetParamAddr();
    ~SCPWaitEvent();
};

}

