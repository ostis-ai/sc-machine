/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_addr.hpp"
#include "sc-memory/sc_object.hpp"

#include "generationByTemplateKeynodes.generated.hpp"

class GenerationByTemplateKeynodes : public ScObject
{
  SC_CLASS()
  SC_GENERATED_BODY()

public:
  SC_PROPERTY(Keynode("test_concept"), ForceCreate)
  static ScAddr test_concept;

  SC_PROPERTY(Keynode("nrel_first"), ForceCreate)
  static ScAddr nrel_first;

  SC_PROPERTY(Keynode("nrel_second"), ForceCreate)
  static ScAddr nrel_second;

  SC_PROPERTY(Keynode("test_lang"), ForceCreate)
  static ScAddr test_lang;
};
