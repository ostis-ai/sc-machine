/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2014 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS. If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
 */

#ifndef _search_semantic_neighborhood_h_
#define _search_semantic_neighborhood_h_

#include <sc_memory.h>

/*!
 * Function that implements sc-agent to search full semantic neighborhood of specified sc-element
 */
sc_result agent_search_full_semantic_neighborhood(const sc_event *event, sc_addr arg);

/*!
 * Function that implements sc-agent to search all link of given relation, which are connected with given element
 */
sc_result agent_search_links_of_relation_connected_with_element(const sc_event *event, sc_addr arg);

#endif
