/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory_context_manager.h"

#include "sc_memory_context_private.h"
#include "sc_memory_context_permissions.h"

#include "sc-store/sc_storage_private.h"

#include "sc_keynodes.h"

#include "sc_memory.h"
#include "sc_memory_private.h"

#include "sc-store/sc-base/sc_allocator.h"

/*! Structure representing parameters for emitting a sc-event.
 * @note This structure holds the parameters required for emitting a sc-event in a memory context.
 */
struct _sc_event_emit_params
{
  sc_addr subscription_addr;  ///< sc-address representing the subscription associated with the event.
  sc_event_type type;         ///< Type of the event to be emitted.
  sc_addr connector_addr;     ///< sc-address representing the connector associated with the event.
  sc_type connector_type;     ///< sc-type of the connector associated with the event.
  sc_addr other_addr;         ///< sc-address representing the other element associated with the event.
};

#define SC_CONTEXT_FLAG_PENDING_EVENTS 0x1
#define SC_CONTEXT_FLAG_BLOCKING_EVENTS 0x2

#define SC_CONTEXT_PERMISSIONS_FULL 0xff

void _sc_memory_context_manager_initialize(sc_memory_context_manager ** manager, sc_bool user_mode)
{
  sc_memory_info("Initialize context manager");

  *manager = sc_mem_new(sc_memory_context_manager, 1);
  (*manager)->context_hash_table = sc_hash_table_init(g_direct_hash, g_direct_equal, null_ptr, null_ptr);
  (*manager)->context_count = 0;
  sc_monitor_init(&(*manager)->context_monitor);
  (*manager)->user_mode = user_mode;
  (*manager)->user_global_permissions = sc_hash_table_init(g_direct_hash, g_direct_equal, null_ptr, null_ptr);
  sc_monitor_init(&(*manager)->user_global_permissions_monitor);
  (*manager)->basic_action_classes = sc_hash_table_init(g_direct_hash, g_direct_equal, null_ptr, null_ptr);
  (*manager)->user_local_permissions =
      sc_hash_table_init(g_direct_hash, g_direct_equal, null_ptr, (GDestroyNotify)g_hash_table_destroy);
  sc_monitor_init(&(*manager)->user_local_permissions_monitor);

  s_memory_default_ctx = sc_memory_context_new_ext(SC_ADDR_EMPTY);
  s_memory_default_ctx->global_permissions = SC_CONTEXT_PERMISSIONS_FULL;
  s_memory_default_ctx->flags |= SC_CONTEXT_FLAG_SYSTEM;
}

void _sc_memory_context_assign_context_for_system(sc_memory_context_manager * manager, sc_addr * myself_addr_ptr)
{
  sc_unused(manager);

  *myself_addr_ptr = myself_addr;
  _sc_context_set_permissions_for_element(*myself_addr_ptr, SC_CONTEXT_PERMISSIONS_TO_ALL_PERMISSIONS);
  sc_memory_context_free(s_memory_default_ctx);
  s_memory_default_ctx = sc_memory_context_new_ext(myself_addr);
  s_memory_default_ctx->global_permissions = SC_CONTEXT_PERMISSIONS_FULL;
  s_memory_default_ctx->flags |= SC_CONTEXT_FLAG_SYSTEM;
}

void _sc_memory_context_manager_shutdown(sc_memory_context_manager * manager)
{
  sc_memory_info("Shutdown context manager");

  sc_memory_context_free(s_memory_default_ctx);
  s_memory_default_ctx = null_ptr;

  if (manager->context_hash_table == null_ptr)
    return;

  sc_monitor_acquire_read(&manager->context_monitor);
  sc_uint32 context_count = sc_hash_table_size(manager->context_hash_table);
  sc_monitor_release_read(&manager->context_monitor);
  if (context_count > 0)
    sc_memory_warning("There are %d contexts, wasn't destroyed before sc-memory shutdown", context_count);

  sc_monitor_acquire_write(&manager->context_monitor);
  sc_hash_table_destroy(manager->context_hash_table);
  manager->context_hash_table = null_ptr;
  sc_monitor_release_write(&manager->context_monitor);

  sc_monitor_destroy(&manager->context_monitor);

  sc_monitor_destroy(&manager->user_global_permissions_monitor);
  sc_hash_table_destroy(manager->user_global_permissions);

  sc_monitor_destroy(&manager->user_local_permissions_monitor);
  sc_hash_table_destroy(manager->user_local_permissions);

  sc_hash_table_destroy(manager->basic_action_classes);

  sc_mem_free(manager);
}

sc_memory_context * _sc_memory_context_new_impl(sc_memory_context_manager * manager, sc_addr user_addr)
{
  if (manager == null_ptr)
    return null_ptr;

  sc_memory_context * ctx = sc_mem_new(sc_memory_context, 1);

  sc_monitor_acquire_write(&manager->context_monitor);

  if (manager->context_hash_table == null_ptr)
    goto error;

  sc_monitor_init(&ctx->monitor);
  ctx->user_addr = SC_ADDR_IS_EMPTY(user_addr) ? _sc_memory_context_manager_create_guest_user(manager) : user_addr;
  ctx->ref_count = 0;
  ctx->global_permissions = (sc_uint64)sc_hash_table_get(
      manager->user_global_permissions, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(ctx->user_addr)));
  ctx->local_permissions =
      sc_hash_table_get(manager->user_local_permissions, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(ctx->user_addr)));
  ctx->pend_events = null_ptr;

  sc_hash_table_insert(
      manager->context_hash_table, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(ctx->user_addr)), (sc_pointer)ctx);
  ++manager->context_count;
  goto result;

error:
  sc_mem_free(ctx);
  ctx = null_ptr;

result:
  sc_monitor_release_write(&manager->context_monitor);

  return ctx;
}

sc_memory_context * _sc_memory_context_get_impl(sc_memory_context_manager * manager, sc_addr user_addr)
{
  if (manager == null_ptr)
    return null_ptr;

  sc_memory_context * ctx = null_ptr;

  sc_monitor_acquire_read(&manager->context_monitor);

  if (manager->context_hash_table == null_ptr)
    goto error;

  ctx = sc_hash_table_get(manager->context_hash_table, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(user_addr)));

error:
  sc_monitor_release_read(&manager->context_monitor);

  return ctx;
}

sc_memory_context * _sc_memory_context_resolve_impl(sc_memory_context_manager * manager, sc_addr user_addr)
{
  if (manager == null_ptr)
    return null_ptr;

  sc_memory_context * ctx = _sc_memory_context_get_impl(manager, user_addr);
  if (ctx == null_ptr)
    ctx = _sc_memory_context_new_impl(manager, user_addr);

  if (ctx == null_ptr)
    return ctx;

  sc_monitor_acquire_write(&ctx->monitor);
  ++ctx->ref_count;
  sc_monitor_release_write(&ctx->monitor);

  return ctx;
}

void _sc_memory_context_free_impl(sc_memory_context_manager * manager, sc_memory_context * ctx)
{
  if (ctx == null_ptr)
    return;

  sc_monitor_acquire_write(&manager->context_monitor);

  if (manager->context_hash_table == null_ptr)
    goto error;

  sc_monitor_acquire_write(&ctx->monitor);
  sc_uint32 ref_count = --ctx->ref_count;
  sc_monitor_release_write(&ctx->monitor);

  if (ref_count > 0)
    goto error;

  sc_monitor_destroy(&ctx->monitor);
  sc_hash_table_remove(manager->context_hash_table, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(ctx->user_addr)));
  --manager->context_count;

  sc_mem_free(ctx);
error:
  sc_monitor_release_write(&manager->context_monitor);
}

sc_addr _sc_memory_context_get_user_addr(sc_memory_context * ctx)
{
  sc_monitor_acquire_read(&ctx->monitor);
  sc_addr const user_addr = ctx->user_addr;
  sc_monitor_release_read(&ctx->monitor);
  return user_addr;
}

sc_bool _sc_memory_context_are_events_pending(sc_memory_context const * ctx)
{
  sc_monitor_acquire_read((sc_monitor *)&ctx->monitor);
  sc_bool result = ((sc_memory_context *)ctx)->flags & SC_CONTEXT_FLAG_PENDING_EVENTS;
  sc_monitor_release_read((sc_monitor *)&ctx->monitor);
  return result;
}

void _sc_memory_context_pend_event(
    sc_memory_context const * ctx,
    sc_event_type type,
    sc_addr subscription_addr,
    sc_addr connector_addr,
    sc_type connector_type,
    sc_addr other_addr)
{
  sc_event_emit_params * params = sc_mem_new(sc_event_emit_params, 1);
  params->type = type;
  params->subscription_addr = subscription_addr;
  params->connector_addr = connector_addr;
  params->connector_type = connector_type;
  params->other_addr = other_addr;

  sc_monitor_acquire_write((sc_monitor *)&ctx->monitor);
  ((sc_memory_context *)ctx)->pend_events = sc_hash_table_list_append(ctx->pend_events, params);
  sc_monitor_release_write((sc_monitor *)&ctx->monitor);
}

void _sc_memory_context_emit_events(sc_memory_context const * ctx)
{
  GSList * item = null_ptr;
  sc_event_emit_params * event_params = null_ptr;

  // Emit all saved events
  while (ctx->pend_events)
  {
    item = ctx->pend_events;
    event_params = (sc_event_emit_params *)item->data;

    sc_event_emit_impl(
        ctx,
        event_params->subscription_addr,
        event_params->type,
        event_params->connector_addr,
        event_params->connector_type,
        event_params->other_addr);
    sc_mem_free(event_params);

    ((sc_memory_context *)ctx)->pend_events = sc_hash_table_list_remove_sublist(ctx->pend_events, ctx->pend_events);
  }
}

void _sc_memory_context_pending_begin(sc_memory_context * ctx)
{
  sc_monitor_acquire_write(&ctx->monitor);
  ctx->flags |= SC_CONTEXT_FLAG_PENDING_EVENTS;
  sc_monitor_release_write(&ctx->monitor);
}

void _sc_memory_context_pending_end(sc_memory_context * ctx)
{
  sc_monitor_acquire_write(&ctx->monitor);
  ctx->flags &= ~SC_CONTEXT_FLAG_PENDING_EVENTS;
  _sc_memory_context_emit_events(ctx);
  sc_monitor_release_write(&ctx->monitor);
}

sc_bool _sc_memory_context_are_events_blocking(sc_memory_context const * ctx)
{
  sc_monitor_acquire_read((sc_monitor *)&ctx->monitor);
  sc_bool result = ((sc_memory_context *)ctx)->flags & SC_CONTEXT_FLAG_BLOCKING_EVENTS;
  sc_monitor_release_read((sc_monitor *)&ctx->monitor);
  return result;
}

void _sc_memory_context_blocking_begin(sc_memory_context * ctx)
{
  sc_monitor_acquire_write(&ctx->monitor);
  ctx->flags |= SC_CONTEXT_FLAG_BLOCKING_EVENTS;
  sc_monitor_release_write(&ctx->monitor);
}

void _sc_memory_context_blocking_end(sc_memory_context * ctx)
{
  sc_monitor_acquire_write(&ctx->monitor);
  ctx->flags &= ~SC_CONTEXT_FLAG_BLOCKING_EVENTS;
  sc_monitor_release_write(&ctx->monitor);
}
