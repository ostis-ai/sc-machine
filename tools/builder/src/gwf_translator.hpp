/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "translator.hpp"

class GwfTranslator : public Translator
{
  
protected:
  explicit GwfTranslator(class ScMemoryContext & context);
  virtual ~GwfTranslator() = default;

private:
  bool TranslateImpl(Params const & params) override;

  ScAddr GetScAddr(std::string const & idtf);

  ScType ConvertType(std::string const & type);
};
