#pragma once

#include <unordered_map>
#include <string>
#include <vector>

class ScgToScsElement
{
public:
  static std::string GetElement(std::string const & scgElement, std::string const & dict);

private:
  static std::string m_FindValue(
      std::unordered_map<std::string, std::string> const & dictionary,
      std::string const & key);

  static const std::unordered_map<std::string, std::string> m_nodeTypeSets;
  static const std::unordered_map<std::string, std::string> m_backwardNodeTypes;
  static const std::unordered_map<std::string, std::string> m_unsupportedNodeTypeSets;
  static const std::unordered_map<std::string, std::string> m_edgeTypes;
  static const std::unordered_map<std::string, std::string> m_backwardEdgeTypes;
  static const std::unordered_map<std::string, std::string> m_unsupportedEdgeTypes;
};
