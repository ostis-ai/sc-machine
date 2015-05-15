/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _uiDefines_h_
#define _uiDefines_h_

#define SYSTEM_ELEMENT(el) if (sc_helper_check_arc(s_default_ctx, keynode_system_element, el, sc_type_arc_pos_const_perm) == SC_FALSE) \
                                sc_memory_arc_new(s_default_ctx, sc_type_arc_pos_const_perm, keynode_system_element, el);


#endif
