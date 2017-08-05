/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _search_defines_h_
#define _search_defines_h_

#define NOT_IN_CONTEXT(context_node, el) (sc_helper_check_arc(s_default_ctx, context_node, el, sc_type_arc_pos_const_perm) == SC_FALSE)

#endif // SEARCH_DEFINES_H
