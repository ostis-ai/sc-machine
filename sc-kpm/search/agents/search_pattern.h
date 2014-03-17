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

#ifndef SEARCH_PATTERN_H
#define SEARCH_PATTERN_H

#include <sc_memory.h>

/*!
 * Function that implement sc-agent to search of all constructions, which are completely isomorphic to given pattern
 */
sc_result agent_full_pattern_search(const sc_event *event, sc_addr arg);

#endif // SEARCH_PATTERN_H
