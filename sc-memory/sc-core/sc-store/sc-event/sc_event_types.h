/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_event_type_h_
#define _sc_event_type_h_

#include "../sc_types.h"

typedef sc_uint16 sc_event_type;

// events
#define sc_event_unknown (sc_event_type)0x0
#define sc_event_add_input_arc (sc_event_type)0x1
#define sc_event_add_output_arc (sc_event_type)0x2
#define sc_event_add_edge (sc_event_type)0x4
#define sc_event_remove_input_arc (sc_event_type)0x8
#define sc_event_remove_output_arc (sc_event_type)0x10
#define sc_event_remove_edge (sc_event_type)0x20
#define sc_event_erase_element (sc_event_type)0x40
#define sc_event_change_content (sc_event_type)0x80

#endif
