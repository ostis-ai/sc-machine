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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

#ifndef _sc_link_helpers_h_
#define _sc_link_helpers_h_

#include "sc_types.h"
#include "sc_stream.h"


/*! Caclulates checksum for data in stream
 * @param stream Pointer to data stream for checksum calculation
 * @param check_sum Pointer to stucture, that contains calculated checksum
 * @return If checksum calculated, then return SC_TRUE; otherwise return SC_FALSE
 */
sc_bool sc_link_calculate_checksum(const sc_stream *stream, sc_check_sum *check_sum);


/*! Calculates checksum for sc-link, when it is self container for it's data
 * @param el Pointer to sc-link
 * @param sum Pointer to checksum structure to contain result
 * @return If checksum calculated, then return SC_TRUE; otherwise return SC_FALSE
 */
sc_bool sc_link_self_container_calculate_checksum(sc_element *el, sc_check_sum *sum);


#endif
