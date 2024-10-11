/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>
#include <sstream>
#include <unordered_set>

#include "buffer.hpp"
#include "gwf_translator_constants.hpp"
#include "sc_scs_element.hpp"

class SCsWriter
{
public:
  static void Write(
      SCgElements const & elements,
      std::string const & filePath,
      Buffer & buffer,
      size_t depth,
      std::unordered_set<SCgElementPtr> & writtenElements);

  class SCgIdentifierCorrector
  {
  public:
    static void GenerateSCsIdentifier(SCgElementPtr const & scgElement, SCsElementPtr & scsElement);

  private:
    static bool IsRussianIdentifier(std::string const & identifier);
    static bool IsEnglishIdentifier(std::string const & identifier);

    static std::string GenerateIdentifierForUnresolvedCharacters(
        std::string & systemIdentifier,
        std::string const & elementId,
        bool isVar);
    static std::string GenerateCorrectedIdentifier(
        std::string & systemIdentifier,
        std::string const & elementId,
        bool isVar);
    static std::string GenerateSCsIdentifierForVariable(std::string & systemIdentifier);
  };

  static void WriteMainIdentifier(
      Buffer & buffer,
      size_t depth,
      std::string const & systemIdentifier,
      std::string const & mainIdentifier);
  static std::string MakeAlias(std::string const & prefix, std::string const & elementId);
  static bool IsVariable(std::string const & elementType);
};
