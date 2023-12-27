/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_memory_context_manager_h_
#define _sc_memory_context_manager_h_

#include "sc-store/sc_element.h"

#include "sc-store/sc_types.h"
#include "sc-store/sc-base/sc_message.h"
#include "sc-store/sc-base/sc_monitor.h"

typedef struct _sc_memory_context_manager sc_memory_context_manager;
typedef struct _sc_event_emit_params sc_event_emit_params;

extern sc_memory_context * s_memory_default_ctx;

void _sc_memory_context_manager_initialize(
    sc_memory_context_manager ** manager,
    sc_addr my_self_addr,
    sc_bool user_mode);

void _sc_memory_context_manager_register_user_events(sc_memory_context_manager * manager);

void _sc_memory_context_manager_shutdown(sc_memory_context_manager * manager);

/*! Function that create memory context with specified params
 * @param levels Access levels, you can create it with macros @see sc_access_level_make
 * @returns Returns pointer to create memory context. If there were any errors during
 * context creation, then function returns 0
 * @note Do not use one context in different threads.
 */
sc_memory_context * _sc_memory_context_new_impl(sc_memory_context_manager * manager, sc_addr process_addr);

sc_memory_context * _sc_memory_context_get_impl(sc_memory_context_manager * manager, sc_addr user_addr);

sc_memory_context * _sc_memory_context_resolve_impl(sc_memory_context_manager * manager, sc_addr user_addr);

sc_memory_context * _sc_memory_context_resolve_impl_ext(
    sc_memory_context_manager * manager,
    sc_char const * user_system_idtf);

/*! Function that destroys created memory context. You can use that function
 * just for contexts, that were created with @see sc_memory_context_new
 */
void _sc_memory_context_free_impl(sc_memory_context_manager * manager, sc_memory_context * ctx);

sc_bool _sc_memory_context_is_authorized(sc_memory_context_manager * manager, sc_memory_context const * ctx);

sc_bool _sc_memory_context_is_pending(sc_memory_context const * ctx);

void _sc_memory_context_pending_begin(sc_memory_context * ctx);

void _sc_memory_context_pending_end(sc_memory_context * ctx);

/*! Pending event for context
 * @param ctx Pointer to context that should pend event emit
 * @param params Pointer to event emit parameters. Context take ownership
 * on this pointer, so you doesn't need to care about it memory free
 */
void _sc_memory_context_pend_event(
    sc_memory_context const * ctx,
    sc_event_type type,
    sc_addr element,
    sc_addr edge,
    sc_addr other_element);

/*! Emit all pended events
 */
void _sc_memory_context_emit_events(sc_memory_context const * ctx);

#endif
