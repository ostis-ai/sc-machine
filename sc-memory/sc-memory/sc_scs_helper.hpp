/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_stream.hpp"

#include <memory>

class SCsFileInterface
{
public:
  virtual ~SCsFileInterface() = default;
  virtual ScStreamPtr GetFileContent(std::string const & fileURL) = 0;
};

using SCsFileInterfacePtr = std::shared_ptr<SCsFileInterface>;

class ScMemoryContext;

class SCsHelper final
{
public:
  _SC_EXTERN SCsHelper(ScMemoryContext & ctx, SCsFileInterfacePtr fileInterface);

  _SC_EXTERN bool GenerateBySCsText(std::string const & scsText);
  _SC_EXTERN void GenerateBySCsTextLazy(std::string const & scsText);
  _SC_EXTERN std::string const & GetLastError() const;

private:
  ScMemoryContext & m_ctx;

  SCsFileInterfacePtr m_fileInterface;
  std::string m_lastError;
};
