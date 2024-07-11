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

  void write(std::string const & s);
  void add_tabs(std::size_t count);

  std::string get_value() const;

private:
  std::string value;
};

class Utils
{
public:
  static std::string make_alias(std::string const & prefix, std::string const & element_id);

  static bool is_variable(std::string const & el_type);

  static std::string replace_all(std::string const & str, std::string const & from, std::string const & to);

private:
};

class ScsWriter
{
public:
  std::string write(
      std::vector<std::unordered_map<std::string, std::string>> & elementsList,
      std::string const & filePath);

private:
  std::string filePath;
  std::unordered_set<std::string> writtenElements;

  static std::unordered_map<std::string, std::string> imageFormats;

  static std::string getElementValue(
      std::unordered_map<std::string, std::string> const & element,
      std::string const & key);

  void processElementsList(
      std::vector<std::unordered_map<std::string, std::string>> const & elementsList,
      Buffer & buffer,
      std::string const & parent,
      std::size_t nestedLevel);
  void writeNode(Buffer & buffer, std::unordered_map<std::string, std::string> const & element);
  void writeEdge(
      Buffer & buffer,
      std::vector<std::unordered_map<std::string, std::string>> const & elementsList,
      std::vector<std::unordered_map<std::string, std::string>> & edgesList);
  void writeContour(
      Buffer & buffer,
      std::unordered_map<std::string, std::string> const & element,
      std::vector<std::unordered_map<std::string, std::string>> const & elementsList,
      std::size_t nestedLevel);
  void writeLink(Buffer & buffer, std::unordered_map<std::string, std::string> const & element);
  void saveContentToFile(
      Buffer & buffer,
      std::string const & contentData,
      std::string const & contentFileNam,
      std::string const & elementIdtf,
      std::string const & elementType);

  void correctIdtf(Buffer & buffer, std::unordered_map<std::string, std::string> & element);
};