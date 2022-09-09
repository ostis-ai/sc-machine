/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_element.h"

#include "sc_types.h"

sc_bool sc_element_is_request_deletion(sc_element * element)
{
  return (element->flags.type & sc_flag_request_deletion) ? SC_TRUE : SC_FALSE;
}

sc_bool sc_element_is_valid(sc_element * element)
{
  return (element->flags.type == 0 || element->flags.type & sc_flag_request_deletion) ? SC_FALSE : SC_TRUE;
}
