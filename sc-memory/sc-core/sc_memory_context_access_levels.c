/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory_context_access_levels.h"

#include "sc_memory_context_private.h"

#include "sc-store/sc_storage_private.h"
#include "sc-store/sc_iterator3.h"
#include "sc_helper.h"
#include "sc_keynodes.h"

/**
 * @brief Adds global access levels (within the knowledge base) to a given sc-memory context.
 * @param _context Pointer to the sc-memory context.
 * @param _adding_levels Access levels to be added.
 * @return None.
 */
#define _sc_context_add_context_global_access_levels(_context, _adding_levels) \
  ({ \
    sc_monitor_acquire_write(&_context->monitor); \
    _context->global_access_levels |= _adding_levels; \
    sc_monitor_release_write(&_context->monitor); \
  })

/**
 * @brief Removes global access levels (within the knowledge base) from a given sc-memory context.
 * @param _context Pointer to the sc-memory context.
 * @param _removing_levels Access levels to be removed.
 * @return None.
 */
#define _sc_context_remove_context_global_access_levels(_context, _removing_levels) \
  ({ \
    sc_monitor_acquire_write(&_context->monitor); \
    _context->global_access_levels &= ~_removing_levels; \
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

//! Gets sc-memory context global access levels.
#define _sc_monitor_get_context_global_access_levels(_context) \
  ({ \
    sc_monitor_acquire_read((sc_monitor *)&_context->monitor); \
    sc_access_levels const _context_access_levels = _context->global_access_levels; \
    sc_monitor_release_read((sc_monitor *)&_context->monitor); \
    _context_access_levels; \
  })

/**
 * @brief Adds global access levels (within the knowledge base) for a specific user in the context manager.
 * @param _user_addr sc-address of the user.
 * @param _adding_levels Access levels to be added.
 * @return None.
 */
#define _sc_context_add_user_global_access_levels(_user_addr, _adding_levels) \
  ({ \
    sc_monitor_acquire_write(&manager->user_global_access_levels_monitor); \
    sc_access_levels _user_levels = (sc_uint64)sc_hash_table_get( \
        manager->user_global_access_levels, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_user_addr))); \
    _user_levels |= (_adding_levels); \
    sc_hash_table_insert( \
        manager->user_global_access_levels, \
        GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_user_addr)), \
        GINT_TO_POINTER(_user_levels)); \
    sc_monitor_release_write(&manager->user_global_access_levels_monitor); \
  })

/**
 * @brief Removes global access levels (within the knowledge base) for a specific user in the context manager.
 * @param _user_addr sc-address of the user.
 * @param _removing_levels Access levels to be removed.
 * @return None.
 */
#define _sc_context_remove_user_global_access_levels(_user_addr, _removing_levels) \
  ({ \
    sc_monitor_acquire_write(&manager->user_global_access_levels_monitor); \
    sc_access_levels _user_levels = (sc_uint64)sc_hash_table_get( \
        manager->user_global_access_levels, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_user_addr))); \
    _user_levels &= ~(_removing_levels); \
    sc_hash_table_insert( \
        manager->user_global_access_levels, \
        GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_user_addr)), \
        GINT_TO_POINTER(_user_levels)); \
    sc_monitor_release_write(&manager->user_global_access_levels_monitor); \
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
    _sc_context_add_user_global_access_levels(user_addr, SC_CONTEXT_ACCESS_LEVEL_AUTHENTICATED);
  else
    _sc_context_add_context_global_access_levels(ctx, SC_CONTEXT_ACCESS_LEVEL_AUTHENTICATED);

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
    _sc_context_remove_user_global_access_levels(user_addr, SC_CONTEXT_ACCESS_LEVEL_AUTHENTICATED);
  else
    _sc_context_remove_context_global_access_levels(ctx, SC_CONTEXT_ACCESS_LEVEL_AUTHENTICATED);

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
#define sc_context_manager_add_basic_action_class_access_levels(_action_class_addr, _access_levels) \
  ({ \
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
    _sc_context_add_user_global_access_levels(user_addr, levels);
  else
    _sc_context_add_context_global_access_levels(ctx, levels);

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
    _sc_context_remove_user_global_access_levels(user_addr, levels);
  else
    _sc_context_remove_context_global_access_levels(ctx, levels);

  sc_addr const action_arc_addr = sc_memory_arc_new(
      s_memory_default_ctx, sc_type_arc_neg_const_temp, manager->nrel_user_action_class_addr, arc_to_action_class_addr);
  _sc_context_set_access_levels_for_element(action_arc_addr, SC_CONTEXT_ACCESS_LEVEL_TO_ALL_ACCESS_LEVELS);

  return SC_RESULT_OK;
}

/**
 * @brief Adds local access levels (within sc-structure) for a specific user in the context manager.
 * @param _user_addr sc-address of the user.
 * @param _adding_levels Access levels to be added.
 * @return None.
 */
#define _sc_context_add_user_local_access_levels(_user_addr, _adding_levels, _structure_addr) \
  ({ \
    sc_monitor_acquire_write(&manager->user_local_access_levels_monitor); \
    sc_hash_table * structures_access_levels_table = \
        sc_hash_table_get(manager->user_local_access_levels, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_user_addr))); \
    sc_access_levels _user_levels = 0; \
    if (structures_access_levels_table == null_ptr) \
      structures_access_levels_table = sc_hash_table_init(g_direct_hash, g_direct_equal, null_ptr, null_ptr); \
    else \
      _user_levels = (sc_uint64)sc_hash_table_get( \
          structures_access_levels_table, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_structure_addr))); \
    _user_levels |= (_adding_levels); \
    sc_hash_table_insert( \
        structures_access_levels_table, \
        GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_structure_addr)), \
        GINT_TO_POINTER(_user_levels)); \
    sc_monitor_release_write(&manager->user_local_access_levels_monitor); \
  })

/**
 * @brief Removes local access levels (within sc-structure) for a specific user in the context manager.
 * @param _user_addr sc-address of the user.
 * @param _removing_levels Access levels to be removed.
 * @return None.
 */
#define _sc_context_remove_user_local_access_levels(_user_addr, _removing_levels, _structure_addr) \
  ({ \
    sc_monitor_acquire_write(&manager->user_local_access_levels_monitor); \
    sc_hash_table * structures_access_levels_table = \
        sc_hash_table_get(manager->user_local_access_levels, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_user_addr))); \
    sc_access_levels _user_levels = 0; \
    if (structures_access_levels_table != null_ptr) \
    { \
      _user_levels = (sc_uint64)sc_hash_table_get( \
          structures_access_levels_table, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_structure_addr))); \
      _user_levels &= ~(_removing_levels); \
      sc_hash_table_insert( \
          structures_access_levels_table, \
          GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_structure_addr)), \
          GINT_TO_POINTER(_user_levels)); \
      sc_monitor_release_write(&manager->user_local_access_levels_monitor); \
    } \
  })

/**
 * @brief Adds local access levels (within sc-structure) to a given sc-memory context.
 * @param _context Pointer to the sc-memory context.
 * @param _adding_levels Access levels to be added.
 * @return None.
 */
#define _sc_context_add_context_local_access_levels(_context, _adding_levels, _structure_addr) \
  _sc_context_add_user_local_access_levels(_context->user_addr, _adding_levels, _structure_addr)

/**
 * @brief Removes local access levels (within sc-structure) from a given sc-memory context.
 * @param _context Pointer to the sc-memory context.
 * @param _removing_levels Access levels to be removed.
 * @return None.
 */
#define _sc_context_remove_context_local_access_levels(_context, _removing_levels, _structure_addr) \
  _sc_context_remove_user_local_access_levels(_context->user_addr, _removing_levels, _structure_addr)

sc_result _sc_memory_context_manager_on_new_user_action_class_within_structure(
    sc_event const * event,
    sc_addr initiator_addr,
    sc_addr connector_addr,
    sc_type connector_type,
    sc_addr arc_to_arc_between_action_class_and_structure_addr)
{
  sc_unused(&initiator_addr);
  sc_unused(&connector_addr);

  // Only temporal sc-arcs can be used
  if (sc_type_check(connector_type, sc_type_arc_pos_const_temp) == SC_FALSE)
    return SC_RESULT_OK;

  _sc_context_set_access_levels_for_element(connector_addr, SC_CONTEXT_ACCESS_LEVEL_TO_ALL_ACCESS_LEVELS);

  sc_addr user_addr, arc_between_action_class_and_structure;
  sc_memory_get_arc_info(
      s_memory_default_ctx,
      arc_to_arc_between_action_class_and_structure_addr,
      &user_addr,
      &arc_between_action_class_and_structure);

  sc_addr action_class_addr, structure_addr;
  sc_memory_get_arc_info(
      s_memory_default_ctx, arc_between_action_class_and_structure, &action_class_addr, &structure_addr);

  sc_memory_context_manager * manager = sc_event_get_data(event);
  sc_access_levels const levels = sc_context_manager_get_basic_action_class_access_levels(action_class_addr);

  sc_memory_context * ctx = _sc_memory_context_get_impl(manager, user_addr);
  if (ctx == null_ptr)
    _sc_context_add_user_local_access_levels(user_addr, levels, structure_addr);
  else
    _sc_context_add_context_local_access_levels(ctx, levels, structure_addr);

  return SC_RESULT_OK;
}

sc_result _sc_memory_context_manager_on_remove_user_action_class_within_structure(
    sc_event const * event,
    sc_addr initiator_addr,
    sc_addr connector_addr,
    sc_type connector_type,
    sc_addr arc_to_arc_between_action_class_and_structure_addr)
{
  sc_unused(&initiator_addr);
  sc_unused(&connector_addr);

  // Only temporal sc-arcs can be used
  if (sc_type_check(connector_type, sc_type_arc_pos_const_temp) == SC_FALSE)
    return SC_RESULT_OK;

  _sc_context_set_access_levels_for_element(connector_addr, SC_CONTEXT_ACCESS_LEVEL_TO_ALL_ACCESS_LEVELS);

  sc_addr user_addr, arc_between_action_class_and_structure;
  sc_memory_get_arc_info(
      s_memory_default_ctx,
      arc_to_arc_between_action_class_and_structure_addr,
      &user_addr,
      &arc_between_action_class_and_structure);

  sc_addr action_class_addr, structure_addr;
  sc_memory_get_arc_info(
      s_memory_default_ctx, arc_between_action_class_and_structure, &action_class_addr, &structure_addr);

  sc_memory_context_manager * manager = sc_event_get_data(event);
  sc_access_levels const levels = sc_context_manager_get_basic_action_class_access_levels(action_class_addr);

  sc_memory_context * ctx = _sc_memory_context_get_impl(manager, user_addr);
  if (ctx == null_ptr)
    _sc_context_remove_user_local_access_levels(user_addr, levels, structure_addr);
  else
    _sc_context_remove_context_local_access_levels(ctx, levels, structure_addr);

  sc_addr const action_arc_addr = sc_memory_arc_new(
      s_memory_default_ctx,
      sc_type_arc_neg_const_temp,
      manager->nrel_user_action_class_addr,
      arc_to_arc_between_action_class_and_structure_addr);
  _sc_context_set_access_levels_for_element(action_arc_addr, SC_CONTEXT_ACCESS_LEVEL_TO_ALL_ACCESS_LEVELS);

  return SC_RESULT_OK;
}

void _sc_memory_context_manager_register_user_events(sc_memory_context_manager * manager)
{
  manager->concept_authentication_request_user_addr = concept_authentication_request_user_addr;
  _sc_context_set_access_levels_for_element(
      manager->concept_authentication_request_user_addr, SC_CONTEXT_ACCESS_LEVEL_TO_ALL_ACCESS_LEVELS);

  manager->concept_authenticated_user_addr = concept_authenticated_user_addr;
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

  manager->nrel_user_action_class_addr = nrel_user_action_class_addr;
  _sc_context_set_access_levels_for_element(
      manager->nrel_user_action_class_addr, SC_CONTEXT_ACCESS_LEVEL_TO_ALL_ACCESS_LEVELS);

  sc_context_manager_add_basic_action_class_access_levels(
      action_read_from_sc_memory_addr, SC_CONTEXT_ACCESS_LEVEL_READ);
  sc_context_manager_add_basic_action_class_access_levels(
      action_generate_in_sc_memory_addr, SC_CONTEXT_ACCESS_LEVEL_WRITE);
  sc_context_manager_add_basic_action_class_access_levels(
      action_erase_in_sc_memory_addr, SC_CONTEXT_ACCESS_LEVEL_ERASE);
  sc_context_manager_add_basic_action_class_access_levels(
      action_read_access_levels_from_sc_memory_addr, SC_CONTEXT_ACCESS_LEVEL_TO_READ_ACCESS_LEVELS);
  sc_context_manager_add_basic_action_class_access_levels(
      action_generate_access_levels_in_sc_memory_addr, SC_CONTEXT_ACCESS_LEVEL_TO_WRITE_ACCESS_LEVELS);
  sc_context_manager_add_basic_action_class_access_levels(
      action_erase_access_levels_from_sc_memory_addr, SC_CONTEXT_ACCESS_LEVEL_TO_ERASE_ACCESS_LEVELS);

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

  manager->nrel_user_action_class_within_sc_structure_addr = nrel_user_action_class_within_sc_structure_addr;
  _sc_context_set_access_levels_for_element(
      manager->nrel_user_action_class_within_sc_structure_addr, SC_CONTEXT_ACCESS_LEVEL_TO_ALL_ACCESS_LEVELS);

  manager->on_new_user_action_class_within_sc_structure = sc_event_with_user_new(
      s_memory_default_ctx,
      manager->nrel_user_action_class_within_sc_structure_addr,
      SC_EVENT_ADD_OUTPUT_ARC,
      manager,
      _sc_memory_context_manager_on_new_user_action_class_within_structure,
      null_ptr);

  manager->on_remove_user_action_class_within_sc_structure = sc_event_with_user_new(
      s_memory_default_ctx,
      manager->nrel_user_action_class_within_sc_structure_addr,
      SC_EVENT_REMOVE_OUTPUT_ARC,
      manager,
      _sc_memory_context_manager_on_remove_user_action_class_within_structure,
      null_ptr);
}

void _sc_memory_context_manager_unregister_user_events(sc_memory_context_manager * manager)
{
  sc_event_destroy(manager->on_authentication_request_user_subscription);
  sc_event_destroy(manager->on_remove_authenticated_user_subscription);

  sc_event_destroy(manager->on_new_user_action_class);
  sc_event_destroy(manager->on_remove_user_action_class);
}

sc_bool _sc_memory_context_is_authenticated(sc_memory_context_manager * manager, sc_memory_context const * ctx)
{
  if (manager->user_mode == SC_FALSE)
    return SC_TRUE;

  sc_access_levels const context_access_levels = _sc_monitor_get_context_global_access_levels(ctx);

  sc_bool is_authenticated =
      sc_context_check_context_access_levels(context_access_levels, SC_CONTEXT_ACCESS_LEVEL_AUTHENTICATED);
  return is_authenticated;
}

sc_bool _sc_memory_context_check_global_access_levels(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_access_levels action_class_access_levels)
{
  // If the system is not in user mode, grant access
  if (manager->user_mode == SC_FALSE)
    return SC_TRUE;

  sc_access_levels const context_access_levels = _sc_monitor_get_context_global_access_levels(ctx);

  // Check if the sc-memory context has access to the action class
  sc_bool result = sc_context_check_context_access_levels(context_access_levels, action_class_access_levels);
  return result;
}

sc_bool _sc_memory_context_check_global_access_levels_to_read_access_levels(
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

  sc_access_levels const context_access_levels = _sc_monitor_get_context_global_access_levels(ctx);

  sc_access_levels const element_access_levels = accessed_element->flags.states;

  // Check if the sc-memory context has read access to the element
  sc_access_levels const required_context_access_levels = context_access_levels & required_access_levels;
  sc_access_levels const required_element_access_levels = element_access_levels & required_access_levels;

  sc_bool result =
      sc_context_check_context_access_levels(required_context_access_levels, required_element_access_levels);
  return result;
}

sc_bool _sc_memory_context_check_global_access_levels_to_handle_access_levels(
    sc_memory_context const * ctx,
    sc_addr accessed_element_addr,
    sc_access_levels required_access_levels)
{
  sc_access_levels const context_access_levels = _sc_monitor_get_context_global_access_levels(ctx);
  sc_access_levels const element_access_levels = _sc_context_get_access_levels_for_element(accessed_element_addr);

  // Check if the sc-memory context has access to handle the operation
  sc_access_levels const required_context_access_levels = context_access_levels & required_access_levels;
  sc_access_levels const required_element_access_levels = element_access_levels & required_access_levels;

  sc_bool result =
      sc_context_check_context_access_levels(required_context_access_levels, required_element_access_levels);
  return result;
}

sc_bool _sc_memory_context_check_global_access_levels_to_write_access_levels(
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

  return _sc_memory_context_check_global_access_levels_to_handle_access_levels(
      ctx, accessed_element_addr, required_access_levels);
}

sc_bool _sc_memory_context_check_global_access_levels_to_erase_access_levels(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_addr accessed_element_addr,
    sc_access_levels required_access_levels)
{
  // If the system is not in user mode, grant access
  if (manager->user_mode == SC_FALSE)
    return SC_TRUE;

  return _sc_memory_context_check_global_access_levels_to_handle_access_levels(
      ctx, accessed_element_addr, required_access_levels);
}
