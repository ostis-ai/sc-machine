/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_memory_json_action.hpp"

class ScMemoryHandleLinkContentJsonAction : public ScMemoryJsonAction
{
public:
  ScMemoryJsonPayload Complete(
      ScMemoryContext * context,
      ScMemoryJsonPayload requestPayload,
      ScMemoryJsonPayload & errorsPayload) override;

private:
  sc_bool SetContent(ScMemoryContext * context, ScMemoryJsonPayload const & atom);

  ScMemoryJsonPayload GetContent(ScMemoryContext * context, ScMemoryJsonPayload const & atom);

  std::vector<size_t> FindLinksByContent(ScMemoryContext * context, ScMemoryJsonPayload const & atom);

  std::vector<size_t> FindLinksByContentSubstring(ScMemoryContext * context, ScMemoryJsonPayload const & atom);

  std::vector<std::string> FindLinksContentsByContentSubstring(
      ScMemoryContext * context,
      ScMemoryJsonPayload const & atom);

private:
  sc_uint32 const maxLengthToSearchAsPrefix = 4;
};
