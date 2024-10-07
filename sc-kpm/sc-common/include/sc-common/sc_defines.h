/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_common_defines_h_
#define _sc_common_defines_h_

#define SYSTEM_ELEMENT(context, el) \
  if (sc_helper_check_arc(context, keynode_system_element, el, sc_type_const_perm_pos_arc) == SC_FALSE) \
    sc_memory_arc_new(context, sc_type_const_perm_pos_arc, keynode_system_element, el);

#define IS_SYSTEM_ELEMENT(context, el) \
  (sc_helper_check_arc(context, keynode_system_element, el, sc_type_const_perm_pos_arc) == SC_TRUE)

#endif  // _sc_defines_h_
