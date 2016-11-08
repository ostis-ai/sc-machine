/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc-memory/cpp/sc_object.hpp"
#include "nlKeynodes.generated.hpp"

namespace nl
{
    class Keynodes : public ScObject
    {
        SC_CLASS()
        SC_GENERATED_BODY()

    public:
        SC_PROPERTY(Keynode("main_nl_dialogue_instance"), ForceCreate)
        static ScAddr msMainNLDialogueInstance;

        SC_PROPERTY(Keynode("command_process_user_text_message"), ForceCreate)
        static ScAddr msCmdProcessUserTextMessage;

        SC_PROPERTY(Keynode("command_initiated"), ForceCreate)
        static ScAddr msCommandInitiated;

        SC_PROPERTY(Keynode("command_finished"), ForceCreate)
        static ScAddr msCommandFinished;

        SC_PROPERTY(Keynode("nrel_result"), ForceCreate)
        static ScAddr msNrelResult;

        SC_PROPERTY(Keynode("nrel_translation"), ForceCreate)
        static ScAddr msNrelTranslation;

        SC_PROPERTY(Keynode("nrel_item_order"), ForceCreate)
        static ScAddr msNrelItemOrder;

        SC_PROPERTY(Keynode("rrel_last_item"), ForceCreate)
        static ScAddr msRrelLastItem;
    };
}