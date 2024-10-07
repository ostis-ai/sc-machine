/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _uiTranslatorFromSc_h_
#define _uiTranslatorFromSc_h_

#include "uiTypes.h"

/*! Base class for translators that translate from SC-code to external
 * language
 */
class uiTranslateFromSc
{
public:
  explicit uiTranslateFromSc();
  virtual ~uiTranslateFromSc();

  /*! Run translation.
   * @param input_addr sc-addr of sc-construction, that need to be translated
   * @param format_addr sc-addr of output format
   * @param lang_addr sc-addr of user language
   */
  void translate(sc_addr const & input_addr, sc_addr const & format_addr, sc_addr const & lang_addr);

protected:
  //! Collect objects that need to be translated
  void collectObjects();

  //! Process input data and generate output
  virtual void runImpl() = 0;

  //! Check if sc-element need to be translated
  bool isNeedToTranslate(sc_addr const & addr) const;

public:
  //! Build id from specified sc-addr
  static String buildId(sc_addr const & addr);

protected:
  //! Sc-addr of input construction
  sc_addr mInputConstructionAddr;
  //! Sc-addr of output format
  sc_addr mOutputFormatAddr;
  //! Sc-addr of output language
  sc_addr mOutputLanguageAddr;

  // Maps of elements to translate
  tScAddrToScTypeMap mEdges;

  //! Output scs
  String mOutputData;
};

#endif  // _uiTranslator_h_
