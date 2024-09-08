/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>
#include <sstream>

#include "sc_scg_to_scs_types.hpp"
#include "gwf_parser.hpp"

class Buffer
{
public:
  Buffer();

  Buffer & operator<<(std::string const & string);
  Buffer & AddTabs(std::size_t const & count);

  std::string GetValue() const;

private:
  std::stringstream m_value;
};

class SCgElement;
class SCsElement;
class SCgContour;
class SCsContour;

class SCsElementFactory
{
public:
  static std::shared_ptr<SCsElement> CreateAndConvertElementFromSCgElement(
      std::shared_ptr<SCgElement> const & scgElement);

  static std::shared_ptr<SCsElement> CreateAndConvertElementFromSCgElement(
      std::shared_ptr<SCgElement> const & scgElement,
      std::list<std::shared_ptr<SCgElement>> const & scgElements,
      std::shared_ptr<SCgContour> const & scgContour,
      std::shared_ptr<SCsContour> const & scsContour);
};

class SCsWriter
{
public:
  void Write(SCgElements const & elementsList, std::string const & filePath, Buffer & buffer) const;

  class SCgIdentifierCorrector
  {
  public:
    static void CorrectIdentifier(
        std::shared_ptr<SCgElement> const & scgElement,
        std::shared_ptr<SCsElement> & scsElement);

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
    static std::string CorrectIdentifierForVariable(std::string & systemIdentifier);
    static std::string CorrectIdentifierForNonVariable(std::string & systemIdentifier);
  };

  static std::list<std::shared_ptr<SCgElement>> ConvertMapToList(SCgElements const & scgElements);
  static void WriteMainIdentifier(
      Buffer & buffer,
      std::string const & systemIdentifier,
      std::string const & mainIdentifier);
  static std::string MakeAlias(std::string const & prefix, std::string const & elementId);
  static bool IsVariable(std::string const & elementType);
};
