/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc-memory/cpp/sc_addr.hpp"
#include "sc-memory/cpp/sc_object.hpp"

#include "bottleKeynodes.generated.hpp"

namespace bottle
{

class Keynodes : public ScObject
{
    SC_CLASS()
    SC_GENERATED_BODY()

public:

    SC_PROPERTY(Keynode("button_pressed"), ForceCreate)
    static ScAddr button_pressed;

    SC_PROPERTY(Keynode("button"), ForceCreate)
    static ScAddr button;

    SC_PROPERTY(Keynode("valve"), ForceCreate)
    static ScAddr valve;

    SC_PROPERTY(Keynode("registration"), ForceCreate)
    static ScAddr registration;

    SC_PROPERTY(Keynode("nrel_idtf"), ForceCreate)
    static ScAddr nrel_idtf;

    SC_PROPERTY(Keynode("nrel_equipment_connection"), ForceCreate)
    static ScAddr nrel_equipment_connection;

    SC_PROPERTY(Keynode("measurement_of_fixed_measure"), ForceCreate)
    static ScAddr measurement_of_fixed_measure;

    SC_PROPERTY(Keynode("closed"), ForceCreate)
    static ScAddr closed;

    SC_PROPERTY(Keynode("open"), ForceCreate)
    static ScAddr opened;

    SC_PROPERTY(Keynode("scales"), ForceCreate)
    static ScAddr scales;

    SC_PROPERTY(Keynode("nrel_direct_contact"), ForceCreate)
    static ScAddr nrel_direct_contact;

    SC_PROPERTY(Keynode("bottle"), ForceCreate)
    static ScAddr bottle;

    SC_PROPERTY(Keynode("rrel_1"), ForceCreate)
    static ScAddr rrel_1;

    SC_PROPERTY(Keynode("rrel_2"), ForceCreate)
    static ScAddr rrel_2;

    SC_PROPERTY(Keynode("rrel_3"), ForceCreate)
    static ScAddr rrel_3;
};

} // namespace bottle
