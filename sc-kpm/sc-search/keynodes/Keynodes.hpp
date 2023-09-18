/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_addr.hpp"
#include "sc-memory/sc_object.hpp"

#include "Keynodes.generated.hpp"

namespace scSearch
{
class Keynodes : public ScObject
{
  SC_CLASS()
  SC_GENERATED_BODY()

public:
  SC_PROPERTY(Keynode("action_get_decomposition"), ForceCreate)
  static ScAddr action_get_decomposition;

  SC_PROPERTY(Keynode("nrel_entity_decomposition"), ForceCreate)
  static ScAddr nrel_entity_decomposition;
};

}  // namespace subjDomainModule
