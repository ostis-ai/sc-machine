/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _ui_Sc2SCgJsonTranslator_h_
#define _ui_Sc2SCgJsonTranslator_h_

#include "uiTranslatorFromSc.h"

/*!
 * \brief Class that translates sc-construction into
 * SCs-code.
 */
class uiSc2SCgJsonTranslator : public uiTranslateFromSc
{
public:
    explicit uiSc2SCgJsonTranslator();
    virtual ~uiSc2SCgJsonTranslator();


    static sc_result ui_translate_sc2scg_json(const sc_event *event, sc_addr arg);

protected:
    //! @copydoc uiTranslateFromSc::runImpl
    void runImpl();

private:

};

#endif // _ui_Sc2SCgJsonTranslator_h_
