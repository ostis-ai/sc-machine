/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-builder/translator.hpp"

#include "scs_translator.hpp"
#include "builder_defines.hpp"

#include "gwf_parser.hpp"
#include "sc_scs_writer.hpp"
#include "gwf_translator_const.hpp"

#include <vector>
#include <string>
#include <list>

class GWFTranslator : public Translator
{
public:
  explicit GWFTranslator(class ScMemoryContext & context);
  ~GWFTranslator() override = default;

  bool TranslateImpl(Params const & params) override;

  std::string GWFToScs(std::string const & xmlStr, std::string const & filePath);
  std::string XmlFileToString(std::string const & filename);

private:
  SCsTranslator m_scsTranslator;

  std::string WriteStringToFile(std::string const & scsStr, std::string const & filePath);
};
