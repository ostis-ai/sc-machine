/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_dictionary_private_h_
#define _sc_dictionary_private_h_

#include "../../sc_types.h"
#include "../sc-list/sc_list.h"

sc_dictionary_node * _sc_dictionary_node_initialize(sc_uint8 children_size);

sc_dictionary_node * _sc_dictionary_get_next_node(sc_dictionary * dictionary, sc_dictionary_node * node, sc_char ch);

sc_addr * sc_list_to_addr_array(sc_list * list);

sc_addr_hash * sc_list_to_hashes_array(sc_list * list);

#endif
