/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "scs_translator.hpp"

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_scs_helper.hpp"

#include <boost/filesystem/path.hpp>

#include <regex>

namespace impl
{

class FileProvider : public SCsFileInterface
{
public:
  explicit FileProvider(std::string const& parentPath)
    : m_parentPath(parentPath)
  {
  }

  virtual ~FileProvider() = default;

  virtual ScStreamPtr GetFileContent(std::string const & fileURL)
  {
    std::regex const pattern("(\\w+):(\\/{2,3})(.+)");

    std::smatch match;

    if (std::regex_match(fileURL, match, pattern))
    {
      std::string const protocol = match[1];
      bool const isRelative = (match[2] == "//");

      std::string fullPath;
      if (isRelative)
      {
        boost::filesystem::path parentFullPath = boost::filesystem::path(m_parentPath).parent_path();
        fullPath = (parentFullPath / std::string(match[3])).string();
      }
      else
      {
        fullPath = match[3];
      }

      return std::make_shared<ScStream>(fullPath, SC_STREAM_FLAG_READ);
    }
    else
    {
      SC_THROW_EXCEPTION(utils::ExceptionParseError,
                         "Can't process file content by url " << fileURL);
    }

    return {};
  }

private:
  std::string m_parentPath;
};

} // namespace impl

SCsTranslator::SCsTranslator(ScMemoryContext & context)
  : Translator(context)
{
}

bool SCsTranslator::TranslateImpl(Params const & params)
{
  std::string data;
  GetFileContent(params.m_fileName, data);

  SCsHelper scs(m_ctx, std::make_shared<impl::FileProvider>(params.m_fileName));
  
  if (!scs.GenerateBySCsText(data))
  {
    SC_THROW_EXCEPTION(utils::ExceptionParseError, scs.GetLastError());
  }

  return true;
}