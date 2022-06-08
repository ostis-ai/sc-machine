/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_assert_h_
#define _sc_assert_h_

#define GLIB

#ifdef GLIB
#  include <glib.h>
#endif

#define sc_assert(expr) g_assert(expr)

#undef GLIB

#endif
