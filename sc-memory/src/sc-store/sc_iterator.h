/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2012 OSTIS

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


#ifndef _sc_iterator_h_
#define _sc_iterator_h_

#include "sc_iterator3.h"
#include "sc_iterator5.h"

/* All sc-iterators contains timestamp, so sc-elements can't be physicaly
 * deleted as garbage, while there are some iterators, that can include them in
 * iteration results. So we store sequence of timestamps, that was used to create
 * iterators.
 */

/*! Append timestamp, that used in iterator.
 * @param time_stamp Timestamp that need to be added
 */
void sc_iterator_add_used_timestamp(sc_uint32 time_stamp);

/*! Remove timestamp, that was used for iterator creation.
 * @param time_stamp Timestamp that need to be removed
 */
void sc_iterator_remove_used_timestamp(sc_uint32 time_stamp);

/*! Return oldest timestamp from list
 * This function return 0, when sequence is empty @see sc_iterator_has_any_timestamp
 */
sc_uint32 sc_iterator_get_oldest_timestamp();

//! Check if there are any used timestamp in list
sc_bool sc_iterator_has_any_timestamp();

#endif // SC_ITERATOR_H
