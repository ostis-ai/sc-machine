/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_object.hpp>

#include "eraseElementsKeynodes.generated.hpp"

namespace scAgentsCommon
{
class EraseElementsKeynodes : public ScObject
{
  SC_CLASS()
  SC_GENERATED_BODY()

public:
  SC_PROPERTY(Keynode("action_erase_elements"), ForceCreate)
  static ScAddr action_erase_elements;

  SC_PROPERTY(Keynode("unerasable_elements"), ForceCreate)
  static ScAddr unerasable_elements;
};

}  // namespace scAgentsCommon
