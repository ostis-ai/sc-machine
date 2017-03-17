/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <sc-memory/cpp/sc_object.hpp>

#include "nlTextTemplProcessor.generated.hpp"

namespace nl
{
namespace util
{

class TextTemplProcessor : public ScObject
{
  SC_CLASS()
  SC_GENERATED_BODY()

public:
  TextTemplProcessor(ScMemoryContext & ctx);

  /* textTempl - ScAddr of sc-link, that contains text template
   * lang - ScAddr of language, that sould to be used (optional)
   * inputData - ScAddr of struct, that contains input data (optional)
   * parseTempl - ScAddr of template for parsing input data
   */
  sc_result operator() (ScAddr const & textTempl, 
                        ScAddr const & lang,
                        ScAddr const & inputData,
                        ScAddr const & parseTempl);

  std::string const & GetResult() const { return m_result; }

private:
  ScMemoryContext & m_memoryCtx;
  std::string m_result;

  // used keynodes
  SC_PROPERTY(Keynode("nrel_main_idtf"), ForceCreate(ScType::NodeConstNoRole))
  static ScAddr ms_nrelMainIdtf;
};

} // namespace util
} // namespace nl