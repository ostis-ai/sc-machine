/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "scs_translator.hpp"

#include <regex>
#include <utility>
#include <filesystem>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_scs_helper.hpp>

extern "C"
{
#include <sc-core/sc-container/sc_string.h>
}

namespace impl
{

class FileProvider : public SCsFileInterface
{
public:
  explicit FileProvider(std::string parentPath)
    : m_parentPath(std::move(parentPath))
  {
  }

  virtual ~FileProvider() = default;

  virtual ScStreamPtr GetFileContent(std::string const & fileURL)
  {
    std::regex const pattern("(\\w+):(\\/{2,3})(.+)");

    std::smatch match;
    if (!std::regex_match(fileURL, match, pattern))
      SC_THROW_EXCEPTION(utils::ExceptionParseError, "Can't process file content by url " << fileURL);

    std::string const protocol = match[1];
    bool const isRelative = (match[2] == "//");

    std::string fullPath;
    if (isRelative)
    {
      std::filesystem::path parentFullPath = std::filesystem::path(m_parentPath).parent_path();
      fullPath = std::filesystem::absolute(parentFullPath / std::string(match[3]));
    }
    else
      fullPath = match[3];

    std::string const extension = utils::StringUtils::GetFileExtension(fullPath);
    sc_char * copied;
    sc_str_cpy(copied, fullPath.c_str(), fullPath.size());
    return std::make_shared<ScStream>(copied, fullPath.size(), SC_STREAM_FLAG_READ, SC_TRUE);
  }

private:
  std::string m_parentPath;
};

}  // namespace impl

SCsTranslator::SCsTranslator(ScMemoryContext & context)
  : Translator(context)
{
}

bool SCsTranslator::TranslateImpl(Params const & params)
{
  std::string data;
  GetFileContent(params.m_fileName, data);

  SCsHelper scs(m_ctx, std::make_shared<impl::FileProvider>(params.m_fileName));

  if (!scs.GenerateBySCsText(data, params.m_outputStructure))
    SC_THROW_EXCEPTION(utils::ExceptionParseError, scs.GetLastError());

  return true;
}
