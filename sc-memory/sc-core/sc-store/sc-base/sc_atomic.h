/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_atomic_h_
#define _sc_atomic_h_

#define GLIB

#ifdef GLIB
#  include <glib.h>
#endif

#define sc_atomic_int_get(atomic) g_atomic_int_get(atomic)

#define sc_atomic_int_set(atomic, newval) g_atomic_int_set(atomic, newval)

#define sc_atomic_pointer_get(atomic) g_atomic_pointer_get(atomic)

#define sc_atomic_pointer_set(atomic, newval) g_atomic_pointer_set(atomic, newval)

#define sc_atomic_int_inc(atomic) g_atomic_int_inc(atomic)

#define sc_atomic_int_dec_and_test(atomic) g_atomic_int_dec_and_test(atomic)

#define sc_atomic_int_compare_and_exchange(atomic, oldval, newval) \
  g_atomic_int_compare_and_exchange(atomic, oldval, newval)

#define sc_atomic_int_add(atomic, val) g_atomic_int_add(atomic, val)

#define sc_atomic_int_and(atomic, val) g_atomic_int_and(atomic, val)

#define sc_atomic_int_or(atomic, val) g_atomic_int_or(atomic, val)

#define sc_atomic_int_xor(atomic, val) g_atomic_int_xor(atomic, val)

#define sc_atomic_pointer_compare_and_exchange(atomic, oldval, newval) \
  g_atomic_pointer_compare_and_exchange(atomic, oldval, newval)

#define sc_atomic_pointer_add(atomic, val) g_atomic_pointer_add(atomic, val)

#define sc_atomic_pointer_and(atomic, val) g_atomic_pointer_and(atomic, val)

#define sc_atomic_pointer_or(atomic, val) g_atomic_pointer_or(atomic, val)

#define sc_atomic_pointer_xor(atomic, val) g_atomic_pointer_xor(atomic, val)

#undef GLIB

#endif
