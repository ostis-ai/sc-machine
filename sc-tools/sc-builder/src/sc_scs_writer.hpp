#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <regex>
#include <fstream>
#include <unordered_set>
#include <filesystem>

#include <sc-memory/sc_utils.hpp>

#include "scg_to_scs_el.hpp"
#include "gwf_parser.hpp"


class SCsElement
{
public:
  virtual void ConvertFromSCgElement(SCgElement* element);
  virtual std::string Dump() const = 0;
  virtual ~SCsElement() = default;
};

class SCsNode : public SCsElement
{
  void ConvertFromSCgElement(SCgElement* element) override;

  std::string Dump() const override;
};


class Buffer
{
public:
  Buffer();

  void Write(std::string const & s);
  void AddTabs(std::size_t const & count);

  std::string GetValue() const;

private:
  std::string value;
};

class SCsWriter
{
public:
  std::string Write(
      std::unordered_map<std::string, std::shared_ptr<SCgElement>> const & elementsList,
      std::string const & filePath);

private:
  void ProcessElementsList(
      std::unordered_map<std::string, std::shared_ptr<SCgElement>> const & elementsList,
      Buffer & buffer,
      std::string const & filePath);
  void WriteNode(Buffer & buffer, std::shared_ptr<SCgElement> const & element, std::string const & filePath);
  void WriteConnector(Buffer & buffer, std::shared_ptr<SCgConnector> const & element);
  void WriteContour(Buffer & buffer, std::shared_ptr<SCgContour> const & element, int tabLevel);
  void WriteLink(Buffer & buffer, std::shared_ptr<SCgLink> const & element, std::string const & filePath);
  void WriteNodeForContour(
      Buffer & buffer,
      std::shared_ptr<SCgElement> const & node,
      std::shared_ptr<SCgContour> const & contour);

  void CorrectIdtf(Buffer & buffer, std::shared_ptr<SCgElement> const & element);

  bool CheckForNode(
    std::shared_ptr<SCgElement> const refElement,
    std::vector<std::shared_ptr<SCgElement>> const & contourElements);

  bool IsRussianIdtf(const std::string& idtf);

  bool IsEnglishIdtf(const std::string& idtf);

  std::string GenerateIdtfForUnresolvedCharacters(std::string & systemIdtf, const std::string & elementId, bool isVar);
  std::string GenerateCorrectedIdtf(std::string & systemIdtf, std::string & elementId, bool isVar);
  std::string CorrectIdtfForVariable(std::string  & systemIdtf);
  std::string CorrectIdtfForNonVariable(std::string & systemIdtf);
  void WriteMainIdtf(Buffer & buffer, const std::string & systemIdtf, const std::string & mainIdtf);
  void ProcessSpecialTags(std::shared_ptr<SCgElement> const & element);

  // Utils

  std::string MakeAlias(std::string const & prefix, std::string const & element_id);
  bool IsVariable(std::string const & el_type);
};
