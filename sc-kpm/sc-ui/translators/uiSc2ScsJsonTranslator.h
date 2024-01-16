/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _ui_translator_sc2scs_h_
#define _ui_translator_sc2scs_h_

#include "uiTranslatorFromSc.h"

/*!
 * \brief Class that translates sc-construction into
 * SCs-code.
 */
class uiSc2ScsTranslator : public uiTranslateFromSc
{
public:
  explicit uiSc2ScsTranslator();
  virtual ~uiSc2ScsTranslator();

  static sc_result ui_translate_sc2scs(sc_event const * event, sc_addr arg);

protected:
  //! @copydoc uiTranslateFromSc::runImpl
  void runImpl();

  //! Get main or system identifier for specified sc-addr
  bool getIdentifier(sc_addr const & addr, sc_addr const & lang_addr, String & idtf);

protected:
  //! Map of resolved system identifiers
  typedef std::map<sc_addr, String> tSystemIdentifiersMap;
  tSystemIdentifiersMap mIdentifiers;
};

#endif
