/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <antlr3.hpp>

namespace scs
{
  class scsLexer;
  class scsParser;

  //code for overriding
  template <typename ImplTraits>
  class UserTraits : public antlr3::CustomTraitsBase<ImplTraits>
  {
  };

  typedef antlr3::Traits<scsLexer, scsParser, UserTraits> scsLexerTraits;
  typedef antlr3::Traits<scsLexer, scsParser, UserTraits> scsParserTraits;
}