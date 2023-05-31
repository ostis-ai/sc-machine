/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "translator.hpp"
#include "scs_translator.hpp"
#include "builder_defines.hpp"

class GWFTranslator : public Translator
{
public:
  explicit GWFTranslator(class ScMemoryContext & context);
  ~GWFTranslator() override = default;

  bool TranslateImpl(Params const & params) override;

private:
  SCsTranslator m_scsTranslator;

  std::string ConvertToSCsPath(std::string const & path) const;

  bool ErrorsExist(std::string const & path) const;

  std::string GetError(std::string const & path) const;
};
