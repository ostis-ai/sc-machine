/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory_context_manager.h"

#include "sc-store/sc-base/sc_allocator.h"

#include "sc_memory.h"
#include "sc_memory_private.h"
#include "sc-store/sc-event/sc_event_private.h"

struct _sc_memory_context_manager
{
  sc_hash_table * context_hash_table;
  sc_uint32 last_context_id;
  sc_uint32 context_count;
  sc_monitor context_monitor;
};

struct _sc_event_emit_params
{
  sc_addr el;
  sc_access_levels el_access;
  sc_event_type type;
  sc_addr edge;
  sc_addr other_el;
};

struct _sc_memory_context
{
  sc_uint32 id;
  sc_access_levels access_levels;
  sc_uint8 flags;
  sc_hash_table_list * pend_events;
  sc_monitor monitor;
};

#define SC_CONTEXT_FLAG_PENDING_EVENTS 0x1

void _sc_memory_context_manager_initialize(sc_memory_context_manager ** manager)
{
  sc_memory_info("Initialize context manager");

  *manager = sc_mem_new(sc_memory_context_manager, 1);
  (*manager)->context_hash_table = sc_hash_table_init(g_direct_hash, g_direct_equal, null_ptr, null_ptr);
  (*manager)->last_context_id = 0;
  (*manager)->context_count = 0;
  sc_monitor_init(&(*manager)->context_monitor);

  s_memory_default_ctx = sc_memory_context_new(sc_access_lvl_make_min);
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

  sc_mem_free(manager);
}

sc_memory_context * _sc_memory_context_new(sc_memory_context_manager * manager)
{
  sc_memory_context * ctx = sc_mem_new(sc_memory_context, 1);
  sc_uint32 index;

  // setup concurrency id
  sc_monitor_acquire_write(&manager->context_monitor);
  if (manager->context_count >= G_MAXUINT32)
    goto error;

  if (manager->context_hash_table == null_ptr)
    goto error;

  index = (manager->last_context_id + 1) % G_MAXUINT32;
  while (index == 0 ||
         (index != manager->last_context_id && sc_hash_table_get(manager->context_hash_table, GINT_TO_POINTER(index))))
    index = (index + 1) % G_MAXUINT32;

  if (index != manager->last_context_id)
  {
    ctx->id = index;
    sc_monitor_init(&ctx->monitor);

    manager->last_context_id = index;
    sc_hash_table_insert(manager->context_hash_table, GINT_TO_POINTER(ctx->id), (sc_pointer)ctx);
  }
  else
    goto error;

  ++manager->context_count;
  goto result;

error:
{
  sc_mem_free(ctx);
  ctx = null_ptr;
}

result:
  sc_monitor_release_write(&manager->context_monitor);

  return ctx;
}

sc_memory_context * _sc_memory_context_new_impl(sc_memory_context_manager * manager, sc_access_levels levels)
{
  if (manager == null_ptr)
    return null_ptr;

  sc_memory_context * ctx = _sc_memory_context_new(manager);
  if (ctx == null_ptr)
    return null_ptr;

  ctx->access_levels = levels;

  return ctx;
}

void _sc_memory_context_free_impl(sc_memory_context_manager * manager, sc_memory_context * ctx)
{
  if (ctx == null_ptr)
    return;

  sc_monitor_acquire_write(&manager->context_monitor);

  if (manager->context_hash_table == null_ptr)
    goto error;

  sc_memory_context * context = sc_hash_table_get(manager->context_hash_table, GINT_TO_POINTER(ctx->id));
  if (context == null_ptr)
    goto error;

  sc_monitor_destroy(&ctx->monitor);
  sc_hash_table_remove(manager->context_hash_table, GINT_TO_POINTER(ctx->id));
  --manager->context_count;

error:
  sc_monitor_release_write(&manager->context_monitor);

  sc_mem_free(ctx);
}

sc_bool _sc_memory_context_is_pending(sc_memory_context const * ctx)
{
  sc_monitor_acquire_write((sc_monitor *)&ctx->monitor);
  sc_bool result = ((sc_memory_context *)ctx)->flags & SC_CONTEXT_FLAG_PENDING_EVENTS;
  sc_monitor_release_write((sc_monitor *)&ctx->monitor);
  return result;
}

void _sc_memory_context_pend_event(
    sc_memory_context const * ctx,
    sc_event_type type,
    sc_addr element,
    sc_addr edge,
    sc_addr other_element)
{
  sc_event_emit_params * params = sc_mem_new(sc_event_emit_params, 1);
  params->type = type;
  params->el = element;
  params->el_access = sc_access_lvl_make_max;
  params->edge = edge;
  params->other_el = other_element;

  sc_monitor_acquire_write((sc_monitor *)&ctx->monitor);
  ((sc_memory_context *)ctx)->pend_events = g_slist_append(ctx->pend_events, params);
  sc_monitor_release_write((sc_monitor *)&ctx->monitor);
}

void _sc_memory_context_emit_events(sc_memory_context const * ctx)
{
  GSList * item = null_ptr;
  sc_event_emit_params * evt_params = null_ptr;

  while (ctx->pend_events)
  {
    item = ctx->pend_events;
    evt_params = (sc_event_emit_params *)item->data;

    sc_event_emit_impl(
        ctx, evt_params->el, evt_params->el_access, evt_params->type, evt_params->edge, evt_params->other_el);

    sc_mem_free(evt_params);

    ((sc_memory_context *)ctx)->pend_events = g_slist_delete_link(ctx->pend_events, ctx->pend_events);
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
  ctx->flags = ctx->flags & (~SC_CONTEXT_FLAG_PENDING_EVENTS);
  _sc_memory_context_emit_events(ctx);
  sc_monitor_release_write(&ctx->monitor);
}
