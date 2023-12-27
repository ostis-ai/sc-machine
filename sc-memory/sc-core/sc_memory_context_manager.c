/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory_context_manager.h"
#include "sc_memory_context_private.h"

#include "sc-store/sc-base/sc_allocator.h"

#include "sc_memory.h"
#include "sc_memory_private.h"
#include "sc-store/sc-event/sc_event_private.h"
#include "sc_helper.h"

struct _sc_memory_context_manager
{
  sc_hash_table * context_hash_table;
  sc_uint32 context_count;
  sc_monitor context_monitor;
  sc_addr concept_authorized_user_addr;
  sc_addr concept_action_subject_addr;
  sc_hash_table * authorized_users_access_levels;
  sc_event * on_authorized_user_subscription;
  sc_event * on_unauthorized_user_subscription;
  sc_bool user_mode;
};

struct _sc_event_emit_params
{
  sc_addr subscription_addr;
  sc_event_type type;
  sc_addr edge_addr;
  sc_addr other_addr;
};

#define SC_CONTEXT_FLAG_PENDING_EVENTS 0x1

#define SC_CONTEXT_ACCESS_LEVEL_FULL 0xff
#define SC_CONTEXT_ACCESS_LEVEL_AUTHORIZED 0x1

#define sc_context_add_context_access_levels(_access_levels, _adding_levels) _access_levels |= _adding_levels

#define sc_context_remove_context_access_levels(_access_levels, _adding_levels) _access_levels &= ~_adding_levels

#define sc_context_check_context_access_levels(_access_levels, _checking_levels) \
  ((_access_levels) & (_checking_levels)) == _checking_levels

#define _sc_context_add_user_access_levels(_user_addr, _adding_levels) \
  sc_access_levels levels = (sc_uint64)sc_hash_table_get( \
      manager->authorized_users_access_levels, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_user_addr))); \
  levels |= (_adding_levels); \
  sc_hash_table_insert( \
      manager->authorized_users_access_levels, \
      GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_user_addr)), \
      GINT_TO_POINTER(levels))

#define _sc_context_remove_user_access_levels(_user_addr, _adding_levels) \
  sc_access_levels levels = (sc_uint64)sc_hash_table_get( \
      manager->authorized_users_access_levels, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_user_addr))); \
  levels &= ~(_adding_levels); \
  sc_hash_table_insert( \
      manager->authorized_users_access_levels, \
      GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_user_addr)), \
      GINT_TO_POINTER(levels))

sc_result _sc_memory_context_manager_on_authorized_user(sc_event const * event, sc_addr arc_addr, sc_addr user_addr)
{
  sc_unused(&arc_addr);

  sc_memory_context_manager * manager = sc_event_get_data(event);
  sc_memory_context * ctx = _sc_memory_context_get_impl(manager, user_addr);
  if (ctx == null_ptr)
  {
    _sc_context_add_user_access_levels(user_addr, SC_CONTEXT_ACCESS_LEVEL_AUTHORIZED);
    sc_memory_arc_new(
        s_memory_default_ctx, sc_type_arc_pos_const_temp, manager->concept_action_subject_addr, user_addr);

    return SC_RESULT_OK;
  }

  sc_monitor_acquire_write(&ctx->monitor);
  sc_context_add_context_access_levels(ctx->access_levels, SC_CONTEXT_ACCESS_LEVEL_AUTHORIZED);
  sc_monitor_release_write(&ctx->monitor);

  sc_memory_arc_new(s_memory_default_ctx, sc_type_arc_pos_const_temp, manager->concept_action_subject_addr, user_addr);

  return SC_RESULT_OK;
}

sc_result _sc_memory_context_manager_on_unauthorized_user(sc_event const * event, sc_addr arc_addr, sc_addr user_addr)
{
  sc_unused(&arc_addr);

  sc_memory_context_manager * manager = sc_event_get_data(event);
  sc_memory_context * ctx = _sc_memory_context_get_impl(manager, user_addr);
  if (ctx == null_ptr)
  {
    _sc_context_remove_user_access_levels(user_addr, SC_CONTEXT_ACCESS_LEVEL_AUTHORIZED);
    return SC_RESULT_OK;
  }

  sc_monitor_acquire_write(&ctx->monitor);
  sc_context_remove_context_access_levels(ctx->access_levels, SC_CONTEXT_ACCESS_LEVEL_AUTHORIZED);
  sc_monitor_release_write(&ctx->monitor);

  return SC_RESULT_OK;
}

void _sc_memory_context_manager_initialize(
    sc_memory_context_manager ** manager,
    sc_addr my_self_addr,
    sc_bool user_mode)
{
  sc_memory_info("Initialize context manager");

  *manager = sc_mem_new(sc_memory_context_manager, 1);
  (*manager)->context_hash_table = sc_hash_table_init(g_direct_hash, g_direct_equal, null_ptr, null_ptr);
  (*manager)->context_count = 0;
  sc_monitor_init(&(*manager)->context_monitor);
  (*manager)->user_mode = user_mode;
  (*manager)->authorized_users_access_levels = sc_hash_table_init(g_direct_hash, g_direct_equal, null_ptr, null_ptr);

  s_memory_default_ctx = sc_memory_context_new(my_self_addr);
  s_memory_default_ctx->access_levels = SC_CONTEXT_ACCESS_LEVEL_FULL;
}

void _sc_memory_context_manager_register_user_events(sc_memory_context_manager * manager)
{
  sc_helper_resolve_system_identifier(
      s_memory_default_ctx, "concept_authorized_user", &manager->concept_authorized_user_addr);
  sc_helper_resolve_system_identifier(
      s_memory_default_ctx, "concept_action_subject_addr", &manager->concept_action_subject_addr);

  manager->on_authorized_user_subscription = sc_event_new_ex(
      s_memory_default_ctx,
      manager->concept_authorized_user_addr,
      SC_EVENT_ADD_OUTPUT_ARC,
      manager,
      _sc_memory_context_manager_on_authorized_user,
      null_ptr);
  manager->on_unauthorized_user_subscription = sc_event_new_ex(
      s_memory_default_ctx,
      manager->concept_authorized_user_addr,
      SC_EVENT_REMOVE_OUTPUT_ARC,
      manager,
      _sc_memory_context_manager_on_unauthorized_user,
      null_ptr);
}

void _sc_memory_context_manager_unregister_user_events(sc_memory_context_manager * manager)
{
  sc_event_destroy(manager->on_authorized_user_subscription);
  sc_event_destroy(manager->on_unauthorized_user_subscription);
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

  sc_hash_table_destroy(manager->authorized_users_access_levels);

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
  ctx->user_addr = user_addr;
  ctx->ref_count = 0;
  ctx->access_levels = (sc_uint64)sc_hash_table_get(
      manager->authorized_users_access_levels, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(ctx->user_addr)));

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

sc_memory_context * _sc_memory_context_resolve_impl_ext(
    sc_memory_context_manager * manager,
    sc_char const * user_system_idtf)
{
  sc_addr user_addr;

  sc_monitor_acquire_write(&manager->context_monitor);
  sc_bool result = sc_helper_resolve_system_identifier(s_memory_default_ctx, user_system_idtf, &user_addr);
  sc_monitor_release_write(&manager->context_monitor);

  if (result != SC_TRUE)
    return null_ptr;

  return _sc_memory_context_resolve_impl(manager, user_addr);
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

sc_bool _sc_memory_context_is_authorized(sc_memory_context_manager * manager, sc_memory_context const * ctx)
{
  if (manager->user_mode == SC_FALSE)
    return SC_TRUE;

  sc_monitor_acquire_read((sc_monitor *)&ctx->monitor);
  sc_access_levels access_levels = ctx->access_levels;
  sc_monitor_release_read((sc_monitor *)&ctx->monitor);

  sc_bool is_authorized = sc_context_check_context_access_levels(access_levels, SC_CONTEXT_ACCESS_LEVEL_AUTHORIZED);
  return is_authorized;
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
  params->subscription_addr = element;
  params->edge_addr = edge;
  params->other_addr = other_element;

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
        ctx,
        evt_params->subscription_addr,
        evt_params->type,
        ctx->access_levels,
        evt_params->edge_addr,
        evt_params->other_addr);
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
  ctx->flags &= ~SC_CONTEXT_FLAG_PENDING_EVENTS;
  _sc_memory_context_emit_events(ctx);
  sc_monitor_release_write(&ctx->monitor);
}
