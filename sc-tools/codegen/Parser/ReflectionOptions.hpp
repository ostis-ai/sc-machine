#pragma once

#include <string>
#include <vector>

struct ReflectionOptions
{
  std::string targetName;
  std::string inputPath;
  std::string outputPath;
  std::string buildDirectory;
  std::string generatorPath;

  std::vector<std::string> arguments;
  bool displayDiagnostic = false;
  bool useCache = false;
};
