#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <regex>
#include <fstream>
#include <unordered_set>
#include <filesystem>

#include "scg_to_scs_el.hpp"
#include "gwf_parser.hpp"

class Buffer
{
public:
  Buffer();

  void Write(std::string const & s);
  void AddTabs(std::size_t count);

  std::string GetValue() const;

private:
  std::string value;
};

class Utils
{
public:
  static std::string MakeAlias(std::string const & prefix, std::string const & element_id);

  static bool IsVariable(std::string const & el_type);

  static std::string ReplaceAll(std::string const & str, std::string const & from, std::string const & to);

private:
};

class SCSWriter
{
public:
  std::string Write(
      std::unordered_map<std::string, std::shared_ptr<SCGElement>> const & elementsList,
      std::string const & filePath);

private:
  std::unordered_map<std::string, std::shared_ptr<SCGElement>> writtenList;

  void ProcessElementsList(
      std::unordered_map<std::string, std::shared_ptr<SCGElement>> const & elementsList,
      Buffer & buffer,
      std::string const & filePath);
  void WriteNode(Buffer & buffer, std::shared_ptr<SCGElement> const & element, std::string const & filePath);
  void WriteConnector(Buffer & buffer, std::shared_ptr<Connector> const & element);
  void WriteContour(Buffer & buffer, std::shared_ptr<Contour> const & element, int tabLevel);
  void WriteLink(Buffer & buffer, std::shared_ptr<Link> const & element, std::string const & filePath);
  void WriteNodeForContour(
      Buffer & buffer,
      std::shared_ptr<SCGElement> const & node,
      std::shared_ptr<Contour> const & contour);

  void CorrectIdtf(Buffer & buffer, std::shared_ptr<SCGElement> const & element);

  bool CheckForNode(std::shared_ptr<SCGElement> refElement, std::vector<std::shared_ptr<SCGElement>> contourElements);
};
