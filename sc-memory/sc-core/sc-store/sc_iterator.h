/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
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

#endif // SC_ITERATOR_H
