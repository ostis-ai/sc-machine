#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <regex>
#include <fstream>
#include <unordered_set>

#include "scg_to_scs_el.hpp"

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
  static std::string m_MakeAlias(std::string const & prefix, std::string const & element_id);

  static bool m_IsVariable(std::string const & el_type);

  static std::string m_ReplaceAll(std::string const & str, std::string const & from, std::string const & to);

private:
};

class ScsWriter
{
public:
  std::string Write(
      std::vector<std::unordered_map<std::string, std::string>> & elementsList,
      std::string const & filePath);

private:
  std::string filePath;
  std::unordered_set<std::string> writtenElements;

  static std::unordered_map<std::string, std::string> m_imageFormats;

  static std::string m_GetElementValue(
      std::unordered_map<std::string, std::string> const & element,
      std::string const & key);

  void ProcessElementsList(
      std::vector<std::unordered_map<std::string, std::string>> const & elementsList,
      Buffer & buffer,
      std::string const & parent,
      std::size_t nestedLevel);
  void WriteNode(Buffer & buffer, std::unordered_map<std::string, std::string> const & element);
  void WriteEdge(
      Buffer & buffer,
      std::vector<std::unordered_map<std::string, std::string>> const & elementsList,
      std::vector<std::unordered_map<std::string, std::string>> & edgesList);
  void WriteContour(
      Buffer & buffer,
      std::unordered_map<std::string, std::string> const & element,
      std::vector<std::unordered_map<std::string, std::string>> const & elementsList,
      std::size_t nestedLevel);
  void WriteLink(Buffer & buffer, std::unordered_map<std::string, std::string> const & element);
  void SaveContentToFile(
      Buffer & buffer,
      std::string const & contentData,
      std::string const & contentFileNam,
      std::string const & elementIdtf,
      std::string const & elementType);

  void CorrectIdtf(Buffer & buffer, std::unordered_map<std::string, std::string> & element);
};