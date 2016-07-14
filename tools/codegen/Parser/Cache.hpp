#pragma once

#include "Types.hpp"

// Class works with cache of checksums of processed files
class SourceCache
{
public:
	SourceCache(std::string const & path, std::string const & targetName);
	~SourceCache();

	void Load();
	void Save();

	void CheckGenerator(std::string const & fileName);
	bool RequestGenerate(std::string const & fileName);

	static std::string FileChecksum(std::string const & fileName);
private:
	typedef std::map<std::string, std::string> tCacheMap;
	tCacheMap m_cache;

	std::string m_cacheFileName;
};