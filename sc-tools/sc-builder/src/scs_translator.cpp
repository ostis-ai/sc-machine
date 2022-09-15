/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "scs_translator.hpp"

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_scs_helper.hpp"

#include "sc-memory/utils/sc_base64.hpp"

#include <boost/filesystem/path.hpp>

#include <regex>
#include <utility>

namespace impl
{

class FileProvider : public SCsFileInterface
{
public:
  explicit FileProvider(std::string parentPath, std::set<std::string> base64Formats)
    : m_parentPath(std::move(parentPath)), m_base64Formats(std::move(base64Formats))
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

      std::string const extension = fullPath.substr(fullPath.rfind('.'));

      if (m_base64Formats.find(extension) == m_base64Formats.cend())
      {
        return std::make_shared<ScStream>(fullPath, SC_STREAM_FLAG_READ);
      }
      else
      {
        std::ifstream fin(fullPath, std::ios::in | std::ios::binary);
        std::ostringstream oss;
        oss << fin.rdbuf();
        std::string data(oss.str());

        data = ScBase64::Encode(reinterpret_cast<sc_uchar const *>(data.c_str()), data.size());
        auto * rowData = new sc_char[data.size()];
        memcpy(rowData, data.c_str(), data.size());

        return std::make_shared<ScStream>(rowData, data.size(), SC_STREAM_FLAG_READ);
      }
    }
    else
    {
      SC_THROW_EXCEPTION(utils::ExceptionParseError, "Can't process file content by url " << fileURL);
    }

    return {};
  }

private:
  std::string m_parentPath;
  std::set<std::string> m_base64Formats;
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

  SCsHelper scs(
      m_ctx,
      std::make_shared<impl::FileProvider>(
          params.m_fileName, std::set<std::string>({".pdf", ".png", ".jpeg", ".jpg", ".gif"})));
  
  if (!scs.GenerateBySCsText(data))
  {
    SC_THROW_EXCEPTION(utils::ExceptionParseError, scs.GetLastError());
  }

  return true;
}