#pragma once

#include <unordered_map>
#include <string>
#include <vector>

class ScgToScsElement
{
public:
  static std::string m_GetElement(std::string const & scgElement, std::string const & dict);

private:
  static std::string m_FindValue(
      std::unordered_map<std::string, std::string> const & dictionary,
      std::string const & key);

  static const std::unordered_map<std::string, std::string> NodeTypeSets;
  static const std::unordered_map<std::string, std::string> BackwardNodeTypes;
  static const std::unordered_map<std::string, std::string> UnsupportedNodeTypeSets;
  static const std::unordered_map<std::string, std::string> EdgeTypes;
  static const std::unordered_map<std::string, std::string> BackwardEdgeTypes;
  static const std::unordered_map<std::string, std::string> UnsupportedEdgeTypes;
};
