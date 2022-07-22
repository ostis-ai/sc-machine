/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "translator.hpp"

class SCsTranslator : public Translator
{

public:
  explicit SCsTranslator(class ScMemoryContext & context);
  virtual ~SCsTranslator() = default;

  bool TranslateImpl(Params const & params) override;
};
