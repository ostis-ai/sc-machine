/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc-memory/cpp/kpm/sc_agent.hpp"

#include "nlKeynodes.hpp"

#include "nlDialogue.generated.hpp"

namespace nl
{
    class ADialogueProcessMessageAgent : public ScAgent
    {
        SC_CLASS(Agent, Event(Keynodes::msMainNLDialogueInstance, SC_EVENT_ADD_OUTPUT_ARC))
        SC_GENERATED_BODY()

    private:
    };

    void runTest();
}