/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory_context_manager.h"
#include "sc_memory_context_private.h"
#include "sc-store/sc_storage_private.h"

#include "sc-store/sc-base/sc_allocator.h"

#include "sc_memory.h"
#include "sc-store/sc_iterator3.h"
#include "sc_memory_private.h"
#include "sc-store/sc-event/sc_event_private.h"
#include "sc_helper.h"

/*! Structure representing a memory context manager.
 * @note This structure manages memory contexts and user authentications in the sc-memory.
 */
struct _sc_memory_context_manager
{
  sc_hash_table * context_hash_table;  ///< Hash table storing memory contexts based on user addresses.
  sc_uint32 context_count;             ///< Number of currently active memory contexts.
  sc_monitor context_monitor;          ///< Monitor for synchronizing access to the hash table storing memory contexts.
  sc_addr concept_authentication_request_user_addr;  ///< sc-address representing concept node for authentication.
                                                     ///< request users.
  sc_addr concept_authenticated_user_addr;           ///< sc-address representing concept node for authenticated users.
  sc_hash_table * user_access_levels;                ///< Hash table storing access levels for authenticated users.
  sc_monitor user_access_levels_monitor;  ///< Monitor for synchronizing access to the hash table storing access levels.
                                          ///< for authenticated users
  sc_event * on_authentication_request_user_subscription;  ///< Event subscription for authenticated user events.
  sc_event * on_remove_authenticated_user_subscription;    ///< Event subscription for unauthenticated user events.
  sc_addr nrel_user_action_class_addr;     ///< sc-address representing concept node for action class of user relation.
  sc_hash_table * basic_action_classes;    ///< Hash table storing access levels for action classes in sc-memory.
  sc_event * on_new_user_action_class;     ///< Event subscription for adding new action classes for users.
  sc_event * on_remove_user_action_class;  ///< Event subscription for removing new action classes for users.
  sc_bool user_mode;  ///< Boolean indicating whether the system is in user mode (SC_TRUE) or not (SC_FALSE).
};

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

#define SC_CONTEXT_ACCESS_LEVEL_FULL 0xff

/**
 * @brief Adds access levels to a given sc-memory context.
 * @param _context Pointer to the sc-memory context.
 * @param _adding_levels Access levels to be added.
 * @return None.
 */
#define sc_context_add_context_access_levels(_context, _adding_levels) \
  ({ \
    sc_monitor_acquire_write(&_context->monitor); \
    _context->access_levels |= _adding_levels; \
    sc_monitor_release_write(&_context->monitor); \
  })

/**
 * @brief Removes access levels from a given sc-memory context.
 * @param _context Pointer to the sc-memory context.
 * @param _adding_levels Access levels to be removed.
 * @return None.
 */
#define sc_context_remove_context_access_levels(_context, _adding_levels) \
  ({ \
    sc_monitor_acquire_write(&_context->monitor); \
    _context->access_levels &= ~_adding_levels; \
    sc_monitor_release_write(&_context->monitor); \
  })

/**
 * @brief Checks if a given set of access levels is present in the context.
 * @param _access_levels Access levels of the context.
 * @param _checking_levels Access levels to be checked.
 * @return Returns true if all the specified access levels are present; otherwise, false.
 */
#define sc_context_check_context_access_levels(_access_levels, _checking_levels) \
  ((_access_levels) & (_checking_levels)) == _checking_levels

/**
 * @brief Adds access levels for a specific user in the context manager.
 * @param _user_addr Address of the user.
 * @param _adding_levels Access levels to be added.
 * @return None.
 */
#define _sc_context_add_user_access_levels(_user_addr, _adding_levels) \
  ({ \
    sc_monitor_acquire_write(&manager->user_access_levels_monitor); \
    sc_access_levels _user_levels = \
        (sc_uint64)sc_hash_table_get(manager->user_access_levels, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_user_addr))); \
    _user_levels |= (_adding_levels); \
    sc_hash_table_insert( \
        manager->user_access_levels, \
        GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_user_addr)), \
        GINT_TO_POINTER(_user_levels)); \
    sc_monitor_release_write(&manager->user_access_levels_monitor); \
  })

/**
 * @brief Removes access levels for a specific user in the context manager.
 * @param _user_addr Address of the user.
 * @param _adding_levels Access levels to be removed.
 * @return None.
 */
#define _sc_context_remove_user_access_levels(_user_addr, _adding_levels) \
  ({ \
    sc_monitor_acquire_write(&manager->user_access_levels_monitor); \
    sc_access_levels _user_levels = \
        (sc_uint64)sc_hash_table_get(manager->user_access_levels, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_user_addr))); \
    _user_levels &= ~(_adding_levels); \
    sc_hash_table_insert( \
        manager->user_access_levels, \
        GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_user_addr)), \
        GINT_TO_POINTER(_user_levels)); \
    sc_monitor_release_write(&manager->user_access_levels_monitor); \
  })

/**
 * @brief Sets access levels for a specific sc-memory element.
 * @param _element_addr Address of the sc-memory element.
 * @param _access_levels Access levels to be set.
 * @return None.
 */
#define _sc_context_set_access_levels_for_element(_element_addr, _access_levels) \
  ({ \
    sc_element * _element; \
    sc_storage_get_element_by_addr(_element_addr, &_element); \
    if (_element != null_ptr) \
      _element->flags.access_levels |= _access_levels; \
  })

/*! Function that creates a memory context for an authenticated user with specified parameters.
 * @param event Pointer to the sc-event triggering the context creation.
 * @param initiator_addr sc-address representing user that initiated this request.
 * @param connector_addr sc-address representing created sc-connector to the user.
 * @param connector_type sc-type of created sc-connector to the user.
 * @param user_addr sc-address representing the authenticated user for whom the context is created.
 * @returns Returns a result code indicating the success or failure of the operation (SC_RESULT_OK on success).
 * @note This function is called in response to a sc-event and is responsible for creating a new memory context
 * for an authenticated user and establishing a connection between user and the context.
 */
sc_result _sc_memory_context_manager_on_authentication_request_user(
    sc_event const * event,
    sc_addr initiator_addr,
    sc_addr connector_addr,
    sc_type connector_type,
    sc_addr user_addr)
{
  sc_unused(&initiator_addr);

  // Only temporal sc-arcs can be used
  if (sc_type_check(connector_type, sc_type_arc_pos_const_temp) == SC_FALSE)
    return SC_RESULT_OK;

  _sc_context_set_access_levels_for_element(connector_addr, SC_CONTEXT_ACCESS_LEVEL_TO_ALL_ACCESS_LEVELS);
  _sc_context_set_access_levels_for_element(user_addr, SC_CONTEXT_ACCESS_LEVEL_TO_ERASE_ACCESS_LEVELS);

  sc_memory_context_manager * manager = sc_event_get_data(event);
  sc_memory_context * ctx = _sc_memory_context_get_impl(manager, user_addr);
  if (ctx == null_ptr)
    _sc_context_add_user_access_levels(user_addr, SC_CONTEXT_ACCESS_LEVEL_AUTHENTICATED);
  else
    sc_context_add_context_access_levels(ctx, SC_CONTEXT_ACCESS_LEVEL_AUTHENTICATED);

  sc_memory_element_free(s_memory_default_ctx, connector_addr);

  // Remove all negative sc-arcs
  sc_iterator3 * it3 = sc_iterator3_f_a_f_new(
      s_memory_default_ctx, manager->concept_authenticated_user_addr, sc_type_arc_neg_const_temp, user_addr);
  while (sc_iterator3_next(it3))
    sc_memory_element_free(s_memory_default_ctx, sc_iterator3_value(it3, 1));
  sc_iterator3_free(it3);

  sc_addr const auth_arc_addr = sc_memory_arc_new(
      s_memory_default_ctx, sc_type_arc_pos_const_temp, manager->concept_authenticated_user_addr, user_addr);
  _sc_context_set_access_levels_for_element(auth_arc_addr, SC_CONTEXT_ACCESS_LEVEL_TO_ALL_ACCESS_LEVELS);

  return SC_RESULT_OK;
}

/*! Function that handles the removal of authentication for a user and its associated memory context.
 * @param event Pointer to the sc-event triggering the context removal.
 * @param initiator_addr sc-address representing user that initiated this request.
 * @param connector_addr sc-address representing removed sc-connector to the authenticated user.
 * @param connector_type sc-type of created sc-connector to the user.
 * @param user_addr sc-address representing the user whose authentication is being revoked.
 * @returns Returns a result code indicating the success or failure of the operation (SC_RESULT_OK on success).
 * @note This function is called in response to a sc-event and is responsible for removing authentication
 * for user and its associated memory context.
 */
sc_result _sc_memory_context_manager_on_unauthentication_request_user(
    sc_event const * event,
    sc_addr initiator_addr,
    sc_addr connector_addr,
    sc_type connector_type,
    sc_addr user_addr)
{
  sc_unused(&initiator_addr);
  sc_unused(&connector_addr);

  // Only temporal sc-arcs can be used
  if (sc_type_check(connector_type, sc_type_arc_pos_const_temp) == SC_FALSE)
    return SC_RESULT_OK;

  sc_memory_context_manager * manager = sc_event_get_data(event);
  sc_memory_context * ctx = _sc_memory_context_get_impl(manager, user_addr);
  if (ctx == null_ptr)
    _sc_context_remove_user_access_levels(user_addr, SC_CONTEXT_ACCESS_LEVEL_AUTHENTICATED);
  else
    sc_context_remove_context_access_levels(ctx, SC_CONTEXT_ACCESS_LEVEL_AUTHENTICATED);

  sc_addr const auth_arc_addr = sc_memory_arc_new(
      s_memory_default_ctx, sc_type_arc_neg_const_temp, manager->concept_authenticated_user_addr, user_addr);
  _sc_context_set_access_levels_for_element(auth_arc_addr, SC_CONTEXT_ACCESS_LEVEL_TO_ALL_ACCESS_LEVELS);

  return SC_RESULT_OK;
}

/**
 * @brief Adds basic action class access levels to the sc-memory context manager.
 * @param _action_class_system_idtf System identifier of the action class.
 * @param _access_levels Access levels to be added for the action class.
 */
#define sc_context_manager_add_basic_action_class_access_levels(_action_class_system_idtf, _access_levels) \
  ({ \
    sc_addr _action_class_addr; \
    sc_helper_resolve_system_identifier(s_memory_default_ctx, _action_class_system_idtf, &_action_class_addr); \
    sc_hash_table_insert( \
        manager->basic_action_classes, \
        GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_action_class_addr)), \
        GINT_TO_POINTER(_access_levels)); \
    _sc_context_set_access_levels_for_element(_action_class_addr, SC_CONTEXT_ACCESS_LEVEL_TO_ALL_ACCESS_LEVELS); \
  })

/**
 * @brief Gets basic action class access levels from the sc-memory context manager.
 * @param _action_class_addr sc-address of the action class.
 * @return Access levels associated with the action class.
 */
#define sc_context_manager_get_basic_action_class_access_levels(_action_class_addr) \
  (sc_uint64) \
      sc_hash_table_get(manager->basic_action_classes, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_action_class_addr)))

/*! Function that appends access levels for a memory context by adding relation between user and action class.
 * @param event Pointer to the sc-event triggering the addition of the action class.
 * @param initiator_addr sc-address representing user that initiated this request.
 * @param connector_addr sc-address representing created sc-connector to sc-pair with user and action class which this
 * use can complete.
 * @param connector_type sc-type of created sc-connector to sc-pair with user and action class.
 * @param arc_to_action_class_addr sc-address representing sc-pair with user and action class.
 * @returns Returns a result code indicating the success or failure of the operation (SC_RESULT_OK on success).
 * @note The function is called in response to a sc-event and is responsible for appending access levels for users and
 * its sc-memory contexts. It works this way:
 * 1. Some user (with sc-address `initiator_addr`) adds possible action class for other user (with sc-address
 * `user_addr`).
 *
 * user_addr ===========> action_class_addr
 *                 |.\
 *                  .
 *                  .
 *                  .
 *       nrel_user_action_class_addr
 * 2. This callback must be called by adding sc-arc from sc-relation with sc-address `nrel_user_action_class`, then
 * this callback add access levels for user sc-memory context and assigns access levels to this sc-arc.
 */
sc_result _sc_memory_context_manager_on_new_user_action_class(
    sc_event const * event,
    sc_addr initiator_addr,
    sc_addr connector_addr,
    sc_type connector_type,
    sc_addr arc_to_action_class_addr)
{
  sc_unused(&initiator_addr);
  sc_unused(&connector_addr);

  // Only temporal sc-arcs can be used
  if (sc_type_check(connector_type, sc_type_arc_pos_const_temp) == SC_FALSE)
    return SC_RESULT_OK;

  _sc_context_set_access_levels_for_element(connector_addr, SC_CONTEXT_ACCESS_LEVEL_TO_ALL_ACCESS_LEVELS);

  sc_addr user_addr, action_class_addr;
  sc_memory_get_arc_info(s_memory_default_ctx, arc_to_action_class_addr, &user_addr, &action_class_addr);

  sc_memory_context_manager * manager = sc_event_get_data(event);
  sc_access_levels const levels = sc_context_manager_get_basic_action_class_access_levels(action_class_addr);

  sc_memory_context * ctx = _sc_memory_context_get_impl(manager, user_addr);
  if (ctx == null_ptr)
    _sc_context_add_user_access_levels(user_addr, levels);
  else
    sc_context_add_context_access_levels(ctx, levels);

  return SC_RESULT_OK;
}

/**
 * @brief Handles the removal of an action class from a user in the sc-memory context manager.
 *
 * This function is triggered by an sc-event when an action class is removed from a user. It updates the
 * access levels of the user or its sc-memory context accordingly.
 *
 * @param event Pointer to the sc-event triggering the removal of the action class.
 * @param initiator_addr sc-address representing the user that initiated this removal request.
 * @param connector_addr sc-address representing the created sc-connector to sc-pair with the user and action class
 *                      which this user can complete (unused).
 * @param connector_type sc-type of the created sc-connector to sc-pair with the user and action class.
 * @param arc_to_action_class_addr sc-address representing the sc-pair with the user and action class.
 * @return Returns a result code indicating the success or failure of the operation (SC_RESULT_OK on success).
 */
sc_result _sc_memory_context_manager_on_remove_user_action_class(
    sc_event const * event,
    sc_addr initiator_addr,
    sc_addr connector_addr,
    sc_type connector_type,
    sc_addr arc_to_action_class_addr)
{
  sc_unused(&initiator_addr);
  sc_unused(&connector_addr);

  // Only temporal sc-arcs can be used
  if (sc_type_check(connector_type, sc_type_arc_pos_const_temp) == SC_FALSE)
    return SC_RESULT_OK;

  sc_addr user_addr, action_class_addr;
  sc_memory_get_arc_info(s_memory_default_ctx, arc_to_action_class_addr, &user_addr, &action_class_addr);

  sc_memory_context_manager * manager = sc_event_get_data(event);
  sc_access_levels const levels = sc_context_manager_get_basic_action_class_access_levels(action_class_addr);

  sc_memory_context * ctx = _sc_memory_context_get_impl(manager, user_addr);
  if (ctx == null_ptr)
    _sc_context_remove_user_access_levels(user_addr, levels);
  else
    sc_context_remove_context_access_levels(ctx, levels);

  sc_addr const action_arc_addr = sc_memory_arc_new(
      s_memory_default_ctx, sc_type_arc_neg_const_temp, manager->nrel_user_action_class_addr, arc_to_action_class_addr);
  _sc_context_set_access_levels_for_element(action_arc_addr, SC_CONTEXT_ACCESS_LEVEL_TO_ALL_ACCESS_LEVELS);

  return SC_RESULT_OK;
}

void _sc_memory_context_manager_initialize(sc_memory_context_manager ** manager, sc_addr myself_addr, sc_bool user_mode)
{
  sc_memory_info("Initialize context manager");

  *manager = sc_mem_new(sc_memory_context_manager, 1);
  (*manager)->context_hash_table = sc_hash_table_init(g_direct_hash, g_direct_equal, null_ptr, null_ptr);
  (*manager)->context_count = 0;
  sc_monitor_init(&(*manager)->context_monitor);
  (*manager)->user_mode = user_mode;
  (*manager)->user_access_levels = sc_hash_table_init(g_direct_hash, g_direct_equal, null_ptr, null_ptr);
  sc_monitor_init(&(*manager)->user_access_levels_monitor);
  (*manager)->basic_action_classes = sc_hash_table_init(g_direct_hash, g_direct_equal, null_ptr, null_ptr);

  _sc_context_set_access_levels_for_element(myself_addr, SC_CONTEXT_ACCESS_LEVEL_TO_ALL_ACCESS_LEVELS);

  s_memory_default_ctx = sc_memory_context_new_ext(myself_addr);
  s_memory_default_ctx->access_levels = SC_CONTEXT_ACCESS_LEVEL_FULL;
}

void _sc_memory_context_manager_register_user_events(sc_memory_context_manager * manager)
{
  sc_helper_resolve_system_identifier(
      s_memory_default_ctx, "concept_authentication_request_user", &manager->concept_authentication_request_user_addr);
  _sc_context_set_access_levels_for_element(
      manager->concept_authentication_request_user_addr, SC_CONTEXT_ACCESS_LEVEL_TO_ALL_ACCESS_LEVELS);

  sc_helper_resolve_system_identifier(
      s_memory_default_ctx, "concept_authenticated_user", &manager->concept_authenticated_user_addr);
  _sc_context_set_access_levels_for_element(
      manager->concept_authenticated_user_addr, SC_CONTEXT_ACCESS_LEVEL_TO_ALL_ACCESS_LEVELS);

  manager->on_authentication_request_user_subscription = sc_event_with_user_new(
      s_memory_default_ctx,
      manager->concept_authentication_request_user_addr,
      SC_EVENT_ADD_OUTPUT_ARC,
      manager,
      _sc_memory_context_manager_on_authentication_request_user,
      null_ptr);
  manager->on_remove_authenticated_user_subscription = sc_event_with_user_new(
      s_memory_default_ctx,
      manager->concept_authenticated_user_addr,
      SC_EVENT_REMOVE_OUTPUT_ARC,
      manager,
      _sc_memory_context_manager_on_unauthentication_request_user,
      null_ptr);

  sc_helper_resolve_system_identifier(
      s_memory_default_ctx, "nrel_user_action_class", &manager->nrel_user_action_class_addr);
  _sc_context_set_access_levels_for_element(
      manager->nrel_user_action_class_addr, SC_CONTEXT_ACCESS_LEVEL_TO_ALL_ACCESS_LEVELS);

  sc_context_manager_add_basic_action_class_access_levels("read_action_in_sc_memory", SC_CONTEXT_ACCESS_LEVEL_READ);
  sc_context_manager_add_basic_action_class_access_levels("write_action_in_sc_memory", SC_CONTEXT_ACCESS_LEVEL_WRITE);
  sc_context_manager_add_basic_action_class_access_levels("erase_action_in_sc_memory", SC_CONTEXT_ACCESS_LEVEL_ERASE);
  sc_context_manager_add_basic_action_class_access_levels(
      "read_access_levels_action_in_sc_memory", SC_CONTEXT_ACCESS_LEVEL_TO_READ_ACCESS_LEVELS);
  sc_context_manager_add_basic_action_class_access_levels(
      "write_access_levels_action_in_sc_memory", SC_CONTEXT_ACCESS_LEVEL_TO_WRITE_ACCESS_LEVELS);
  sc_context_manager_add_basic_action_class_access_levels(
      "erase_access_levels_action_in_sc_memory", SC_CONTEXT_ACCESS_LEVEL_TO_ERASE_ACCESS_LEVELS);

  manager->on_new_user_action_class = sc_event_with_user_new(
      s_memory_default_ctx,
      manager->nrel_user_action_class_addr,
      SC_EVENT_ADD_OUTPUT_ARC,
      manager,
      _sc_memory_context_manager_on_new_user_action_class,
      null_ptr);

  manager->on_remove_user_action_class = sc_event_with_user_new(
      s_memory_default_ctx,
      manager->nrel_user_action_class_addr,
      SC_EVENT_REMOVE_OUTPUT_ARC,
      manager,
      _sc_memory_context_manager_on_remove_user_action_class,
      null_ptr);
}

void _sc_memory_context_manager_unregister_user_events(sc_memory_context_manager * manager)
{
  sc_event_destroy(manager->on_authentication_request_user_subscription);
  sc_event_destroy(manager->on_remove_authenticated_user_subscription);

  sc_event_destroy(manager->on_new_user_action_class);
  sc_event_destroy(manager->on_remove_user_action_class);
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
  sc_monitor_destroy(&manager->user_access_levels_monitor);

  sc_hash_table_destroy(manager->user_access_levels);
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
  ctx->user_addr = user_addr;
  ctx->ref_count = 0;
  ctx->access_levels =
      (sc_uint64)sc_hash_table_get(manager->user_access_levels, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(ctx->user_addr)));

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

sc_bool _sc_memory_context_is_authenticated(sc_memory_context_manager * manager, sc_memory_context const * ctx)
{
  if (manager->user_mode == SC_FALSE)
    return SC_TRUE;

  sc_monitor_acquire_read((sc_monitor *)&ctx->monitor);
  sc_access_levels const access_levels = ctx->access_levels;
  sc_monitor_release_read((sc_monitor *)&ctx->monitor);

  sc_bool is_authenticated =
      sc_context_check_context_access_levels(access_levels, SC_CONTEXT_ACCESS_LEVEL_AUTHENTICATED);
  return is_authenticated;
}

sc_bool _sc_memory_context_check_action_class(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_access_levels action_class_access_levels)
{
  // If the system is not in user mode, grant access
  if (manager->user_mode == SC_FALSE)
    return SC_TRUE;

  sc_monitor_acquire_read((sc_monitor *)&ctx->monitor);
  sc_access_levels const access_levels = ctx->access_levels;
  sc_monitor_release_read((sc_monitor *)&ctx->monitor);

  // Check if the sc-memory context has access to the action class
  sc_bool result = sc_context_check_context_access_levels(access_levels, action_class_access_levels);
  return result;
}

sc_bool _sc_memory_context_access_levels_to_read_access_levels(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_element * accessed_element,
    sc_addr accessed_element_addr,
    sc_access_levels required_access_levels)
{
  sc_unused(&accessed_element_addr);

  // If the system is not in user mode, grant access
  if (manager->user_mode == SC_FALSE)
    return SC_TRUE;

  sc_monitor_acquire_read((sc_monitor *)&ctx->monitor);
  sc_access_levels const context_access_levels = ctx->access_levels;
  sc_monitor_release_read((sc_monitor *)&ctx->monitor);

  sc_access_levels const element_access_levels = accessed_element->flags.access_levels;

  // Check if the sc-memory context has read access to the element
  sc_access_levels const required_context_access_levels = context_access_levels & required_access_levels;
  sc_access_levels const required_element_access_levels = element_access_levels & required_access_levels;

  sc_bool result =
      sc_context_check_context_access_levels(required_context_access_levels, required_element_access_levels);
  return result;
}

sc_bool _sc_memory_context_acess_levels_to_handle_access_levels(
    sc_memory_context const * ctx,
    sc_addr accessed_element_addr,
    sc_access_levels required_access_levels)
{
  sc_monitor_acquire_read((sc_monitor *)&ctx->monitor);
  sc_access_levels const context_access_levels = ctx->access_levels;
  sc_monitor_release_read((sc_monitor *)&ctx->monitor);

  sc_monitor * monitor = sc_monitor_get_monitor_for_addr(&sc_storage_get()->addr_monitors_table, accessed_element_addr);
  sc_monitor_acquire_read(monitor);
  sc_element * accessed_element;
  sc_storage_get_element_by_addr(accessed_element_addr, &accessed_element);
  sc_access_levels const element_access_levels = accessed_element->flags.access_levels;
  sc_monitor_release_read(monitor);

  // Check if the sc-memory context has access to handle the operation
  sc_access_levels const required_context_access_levels = context_access_levels & required_access_levels;
  sc_access_levels const required_element_access_levels = element_access_levels & required_access_levels;

  sc_bool result =
      sc_context_check_context_access_levels(required_context_access_levels, required_element_access_levels);
  return result;
}

sc_bool _sc_memory_context_access_levels_to_write_access_levels(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_addr accessed_element_addr,
    sc_type connector_from_element_type,
    sc_access_levels required_access_levels)
{
  // If the system is not in user mode, grant access
  if (manager->user_mode == SC_FALSE)
    return SC_TRUE;

  if (sc_type_check(connector_from_element_type, sc_type_arc_pos_const_temp) == SC_FALSE)
    return SC_TRUE;

  return _sc_memory_context_acess_levels_to_handle_access_levels(ctx, accessed_element_addr, required_access_levels);
}

sc_bool _sc_memory_context_access_levels_to_erase_access_levels(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_addr accessed_element_addr,
    sc_access_levels required_access_levels)
{
  // If the system is not in user mode, grant access
  if (manager->user_mode == SC_FALSE)
    return SC_TRUE;

  return _sc_memory_context_acess_levels_to_handle_access_levels(ctx, accessed_element_addr, required_access_levels);
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
  ((sc_memory_context *)ctx)->pend_events = g_slist_append(ctx->pend_events, params);
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
