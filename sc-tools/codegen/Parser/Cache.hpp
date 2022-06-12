#pragma once

#include "Types.hpp"

#include <unordered_map>

// Class works with cache of checksums of processed files
class SourceCache
{
public:
  SourceCache(std::string const & path, std::string const & targetName);

  void Load();
  void Save();

  void CheckGenerator(std::string const & fileName);
  bool RequestGenerate(std::string const & fileName);

  static std::string FileChecksum(std::string const & fileName);

  void Reset();

private:
  using CacheMap = std::unordered_map<std::string, std::string>;
  CacheMap m_cache;

  std::string m_cacheFileName;
};
