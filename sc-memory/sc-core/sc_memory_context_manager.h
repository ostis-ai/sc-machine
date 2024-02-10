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

#define SC_CONTEXT_ACCESS_LEVEL_AUTHENTICATED 0x1

#define SC_CONTEXT_ACCESS_LEVEL_READ 0x4
#define SC_CONTEXT_ACCESS_LEVEL_WRITE 0x8
#define SC_CONTEXT_ACCESS_LEVEL_ERASE 0x10

#define SC_CONTEXT_ACCESS_LEVEL_TO_READ_ACCESS_LEVELS 0x20
#define SC_CONTEXT_ACCESS_LEVEL_TO_WRITE_ACCESS_LEVELS 0x40
#define SC_CONTEXT_ACCESS_LEVEL_TO_ERASE_ACCESS_LEVELS 0x80
#define SC_CONTEXT_ACCESS_LEVEL_TO_ALL_ACCESS_LEVELS \
  (SC_CONTEXT_ACCESS_LEVEL_TO_READ_ACCESS_LEVELS | SC_CONTEXT_ACCESS_LEVEL_TO_WRITE_ACCESS_LEVELS \
   | SC_CONTEXT_ACCESS_LEVEL_TO_ERASE_ACCESS_LEVELS)

/*! Function that initializes the sc-memory context manager with specified parameters.
 * @param manager Pointer to a pointer that will store the newly created memory context manager.
 * @param user_mode Boolean indicating whether the system is in user mode (SC_TRUE) or not (SC_FALSE).
 * @note This function initializes the context manager, creates a default memory context, and sets up event
 * subscriptions.
 */
void _sc_memory_context_manager_initialize(sc_memory_context_manager ** manager, sc_bool user_mode);

void _sc_memory_context_assign_context_for_system(sc_memory_context_manager * manager, sc_addr * myself_addr);

/*! Function that unregisters event subscriptions for user authentication and unauthentication.
 * @param manager Pointer to the sc-memory context manager for which events are unregistered.
 * @note This function releases resources associated with event subscriptions for user authentication and
 * unauthentication.
 */
void _sc_memory_context_manager_shutdown(sc_memory_context_manager * manager);

/*! Function that create memory context with specified params
 * @param levels Access levels, you can create it with macros @see sc_access_level_make
 * @returns Returns pointer to create memory context. If there were any errors during
 * context creation, then function returns 0
 * @note Do not use one context in different threads.
 */
sc_memory_context * _sc_memory_context_new_impl(sc_memory_context_manager * manager, sc_addr user_addr);

/*! Function that retrieves an existing memory context for a specified user.
 * @param manager Pointer to the sc-memory context manager responsible for context retrieval.
 * @param user_addr sc-address representing the user for whom the context is retrieved.
 * @returns Returns a pointer to the existing memory context for the specified user. If the context does not exist,
 * returns null_ptr.
 * @note This function retrieves an existing memory context for the specified user from the manager's context hash
 * table.
 */
sc_memory_context * _sc_memory_context_get_impl(sc_memory_context_manager * manager, sc_addr user_addr);

/*! Function that resolves a memory context for a specified user, creating a new one if it does not exist.
 * @param manager Pointer to the sc-memory context manager responsible for context resolution.
 * @param user_addr sc_addr representing the user for whom the context is resolved.
 * @returns Returns a pointer to the resolved memory context. If an error occurs during resolution, returns null_ptr.
 * @note This function resolves a memory context for the specified user. If the context does not exist, it creates a new
 * one.
 */
sc_memory_context * _sc_memory_context_resolve_impl(sc_memory_context_manager * manager, sc_addr user_addr);

/*! Function that frees a memory context, removing it from the context manager.
 * @param manager Pointer to the sc-memory context manager responsible for context removal.
 * @param ctx Pointer to the sc-memory context to be freed.
 * @note This function frees a memory context, removing it from the manager's context hash table and releasing
 * associated resources.
 */
void _sc_memory_context_free_impl(sc_memory_context_manager * manager, sc_memory_context * ctx);

sc_bool _sc_memory_context_is_pending(sc_memory_context const * ctx);

/*! Function that marks the beginning of a pending events block in a memory context.
 * @param ctx Pointer to the sc-memory context for which the pending events block begins.
 * @note This function marks the beginning of a pending events block in the sc-memory context.
 */
void _sc_memory_context_pending_begin(sc_memory_context * ctx);

/*! Function that marks the end of a pending events block in a memory context, emitting pending events.
 * @param ctx Pointer to the sc-memory context for which the pending events block ends.
 * @note This function marks the end of a pending events block in the sc-memory context, emitting all pending events.
 */
void _sc_memory_context_pending_end(sc_memory_context * ctx);

/*! Function that adds an event to the pending events list in a memory context.
 * @param ctx Pointer to the sc-memory context to which the event is added.
 * @param type Type of the event to be added.
 * @param subscription_addr sc_addr representing the sc-element associated with the event.
 * @param connector_addr sc-address representing the sc-connector associated with the event.
 * @param connector_type sc-type representing the sc-connector associated with the event.
 * @param other_addr sc-address representing the other sc-element associated with the event.
 * @note This function adds an event to the pending events list in the sc-memory context, to be emitted later.
 */
void _sc_memory_context_pend_event(
    sc_memory_context const * ctx,
    sc_event_type type,
    sc_addr subscription_addr,
    sc_addr connector_addr,
    sc_type connector_type,
    sc_addr other_addr);

/*! Function that emits pending events in a memory context.
 * @param ctx Pointer to the sc-memory context for which pending events are emitted.
 * @note This function emits all pending events in the sc-memory context, clearing the pending events list afterward.
 */
void _sc_memory_context_emit_events(sc_memory_context const * ctx);

#endif
