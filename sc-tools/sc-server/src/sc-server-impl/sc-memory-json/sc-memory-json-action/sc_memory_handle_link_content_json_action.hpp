/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_memory_json_action.hpp"

#include <sc-memory/sc_type.hpp>

class ScMemoryHandleLinkContentJsonAction : public ScMemoryJsonAction
{
public:
  ScMemoryJsonPayload Complete(
      ScAgentContext * context,
      ScMemoryJsonPayload requestPayload,
      ScMemoryJsonPayload & errorsPayload) override;

private:
  ScMemoryJsonPayload SetLinkContent(
      ScAgentContext * context,
      ScMemoryJsonPayload const & parameters,
      ScMemoryJsonPayload & error);

  ScMemoryJsonPayload GetLinkContent(
      ScAgentContext * context,
      ScMemoryJsonPayload const & parameters,
      ScMemoryJsonPayload & error);

  ScMemoryJsonPayload SearchLinksByContent(
      ScAgentContext * context,
      ScMemoryJsonPayload const & parameters,
      ScMemoryJsonPayload & error);

  ScMemoryJsonPayload SearchLinksByContentSubstring(
      ScAgentContext * context,
      ScMemoryJsonPayload const & parameters,
      ScMemoryJsonPayload & error);

  ScMemoryJsonPayload SearchLinksContentsByContentSubstring(
      ScAgentContext * context,
      ScMemoryJsonPayload const & parameters,
      ScMemoryJsonPayload & error);

private:
  sc_uint32 const maxLengthToSearchAsPrefix = 4;
};
