/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_object.hpp"

class ScResult : public ScObject
{
private:
  sc_result m_code;

  template <class TScEvent>
  friend class ScAgent;
  friend class ScAction;

  _SC_EXTERN ScResult(sc_result code);

  _SC_EXTERN operator sc_result();
};