/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory_context_permissions.h"

#include "sc_memory_context_private.h"

#include "sc-store/sc_storage_private.h"
#include "sc-store/sc_iterator3.h"
#include "sc_helper.h"
#include "sc_keynodes.h"

typedef void (*sc_permissions_handler)(sc_memory_context_manager *, sc_addr, sc_addr);

/**
 * @brief Adds global permissions (within the knowledge base) to a given sc-memory context.
 * @param _context Pointer to the sc-memory context.
 * @param _adding_permissions Permissions to be added.
 * @return None.
 */
#define _sc_context_add_context_global_permissions(_context, _adding_permissions) \
  ({ \
    sc_monitor_acquire_write(&_context->monitor); \
    _context->global_permissions |= _adding_permissions; \
    sc_monitor_release_write(&_context->monitor); \
  })

/**
 * @brief Removes global permissions (within the knowledge base) from a given sc-memory context.
 * @param _context Pointer to the sc-memory context.
 * @param _removing_permissions Permissions to be removed.
 * @return None.
 */
#define _sc_context_remove_context_global_permissions(_context, _removing_permissions) \
  ({ \
    sc_monitor_acquire_write(&_context->monitor); \
    _context->global_permissions &= ~_removing_permissions; \
    sc_monitor_release_write(&_context->monitor); \
  })

/**
 * @brief Checks if a given subset of permissions is present in the permission.
 * @param _permissions Permissions to be checked if it has subset of permissions.
 * @param _sub_permissions Subset of permissions to be checked in permissions.
 * @return Returns true if all the specified subset of permissions are present; otherwise, false.
 */
#define sc_context_has_permissions_subset(_permissions, _permissions_subset) \
  ((_permissions) & (_permissions_subset)) == _permissions_subset

//! Gets sc-memory context global permissions.
#define _sc_monitor_get_context_global_permissions(_context) \
  ({ \
    sc_monitor_acquire_read((sc_monitor *)&_context->monitor); \
    sc_permissions const _context_permissions = _context->global_permissions; \
    sc_monitor_release_read((sc_monitor *)&_context->monitor); \
    _context_permissions; \
  })

/**
 * @brief Adds global permissions (within the knowledge base) for a specific user in the context manager.
 * @param _user_addr sc-address of the user.
 * @param _adding_permissions Permissions to be added.
 * @return None.
 */
#define _sc_context_add_user_global_permissions(_user_addr, _adding_permissions) \
  ({ \
    sc_monitor_acquire_write(&manager->user_global_permissions_monitor); \
    sc_permissions _user_permissions = (sc_uint64)sc_hash_table_get( \
        manager->user_global_permissions, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_user_addr))); \
    _user_permissions |= (_adding_permissions); \
    sc_hash_table_insert( \
        manager->user_global_permissions, \
        GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_user_addr)), \
        GINT_TO_POINTER(_user_permissions)); \
    sc_monitor_release_write(&manager->user_global_permissions_monitor); \
  })

/**
 * @brief Removes global permissions (within the knowledge base) for a specific user in the context manager.
 * @param _user_addr sc-address of the user.
 * @param _removing_permissions Permissions to be removed.
 * @return None.
 */
#define _sc_context_remove_user_global_permissions(_user_addr, _removing_permissions) \
  ({ \
    sc_monitor_acquire_write(&manager->user_global_permissions_monitor); \
    sc_permissions _user_permissions = (sc_uint64)sc_hash_table_get( \
        manager->user_global_permissions, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_user_addr))); \
    _user_permissions &= ~(_removing_permissions); \
    sc_hash_table_insert( \
        manager->user_global_permissions, \
        GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_user_addr)), \
        GINT_TO_POINTER(_user_permissions)); \
    sc_monitor_release_write(&manager->user_global_permissions_monitor); \
  })

sc_addr _sc_memory_context_manager_create_guest_user(sc_memory_context_manager * manager)
{
  sc_addr const guest_user_addr = sc_memory_node_new(s_memory_default_ctx, sc_type_node | sc_type_const);
  sc_addr const guest_user_arc_addr =
      sc_memory_arc_new(s_memory_default_ctx, sc_type_arc_pos_const_temp, concept_guest_user_addr, guest_user_addr);
  _sc_context_set_permissions_for_element(guest_user_arc_addr, SC_CONTEXT_PERMISSIONS_TO_ALL_PERMISSIONS);
  _sc_context_add_user_global_permissions(guest_user_addr, SC_CONTEXT_PERMISSIONS_AUTHENTICATED);
  return guest_user_addr;
}

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

  // Only positive access sc-arcs can be used
  if (sc_type_has_not_subtype(connector_type, sc_type_arc_pos_const))
    return SC_RESULT_OK;

  _sc_context_set_permissions_for_element(connector_addr, SC_CONTEXT_PERMISSIONS_TO_ALL_PERMISSIONS);
  _sc_context_set_permissions_for_element(user_addr, SC_CONTEXT_PERMISSIONS_TO_ERASE_PERMISSIONS);

  sc_memory_context_manager * manager = sc_event_get_data(event);
  sc_memory_context * ctx = _sc_memory_context_get_impl(manager, user_addr);
  if (ctx == null_ptr)
    _sc_context_add_user_global_permissions(user_addr, SC_CONTEXT_PERMISSIONS_AUTHENTICATED);
  else
    _sc_context_add_context_global_permissions(ctx, SC_CONTEXT_PERMISSIONS_AUTHENTICATED);

  sc_memory_element_free(s_memory_default_ctx, connector_addr);

  // Remove all negative sc-arcs
  sc_iterator3 * it3 = sc_iterator3_f_a_f_new(
      s_memory_default_ctx, manager->concept_authenticated_user_addr, sc_type_arc_neg_const_temp, user_addr);
  while (sc_iterator3_next(it3))
    sc_memory_element_free(s_memory_default_ctx, sc_iterator3_value(it3, 1));
  sc_iterator3_free(it3);

  sc_addr const auth_arc_addr = sc_memory_arc_new(
      s_memory_default_ctx, sc_type_arc_pos_const_temp, manager->concept_authenticated_user_addr, user_addr);
  _sc_context_set_permissions_for_element(auth_arc_addr, SC_CONTEXT_PERMISSIONS_TO_ALL_PERMISSIONS);

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

  // Only positive access sc-arcs can be used
  if (sc_type_has_not_subtype(connector_type, sc_type_arc_pos_const))
    return SC_RESULT_OK;

  sc_memory_context_manager * manager = sc_event_get_data(event);
  sc_memory_context * ctx = _sc_memory_context_get_impl(manager, user_addr);
  if (ctx == null_ptr)
    _sc_context_remove_user_global_permissions(user_addr, SC_CONTEXT_PERMISSIONS_AUTHENTICATED);
  else
    _sc_context_remove_context_global_permissions(ctx, SC_CONTEXT_PERMISSIONS_AUTHENTICATED);

  sc_addr const auth_arc_addr = sc_memory_arc_new(
      s_memory_default_ctx, sc_type_arc_neg_const_temp, manager->concept_authenticated_user_addr, user_addr);
  _sc_context_set_permissions_for_element(auth_arc_addr, SC_CONTEXT_PERMISSIONS_TO_ALL_PERMISSIONS);

  return SC_RESULT_OK;
}

/**
 * @brief Adds basic action class permissions to the sc-memory context manager.
 * @param _action_class_system_idtf System identifier of the action class.
 * @param _permissions Permissions to be added for the action class.
 */
#define sc_context_manager_add_basic_action_class_permissions(_action_class_addr, _permissions) \
  ({ \
    sc_hash_table_insert( \
        manager->basic_action_classes, \
        GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_action_class_addr)), \
        GINT_TO_POINTER(_permissions)); \
    _sc_context_set_permissions_for_element(_action_class_addr, SC_CONTEXT_PERMISSIONS_TO_ALL_PERMISSIONS); \
  })

/**
 * @brief Gets basic action class permissions from the sc-memory context manager.
 * @param _action_class_addr sc-address of the action class.
 * @return Permissions associated with the action class.
 */
#define sc_context_manager_get_basic_action_class_permissions(_action_class_addr) \
  (sc_uint64) \
      sc_hash_table_get(manager->basic_action_classes, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_action_class_addr)))

void _sc_memory_context_manager_add_user_action_class(
    sc_memory_context_manager * manager,
    sc_addr connector_addr,
    sc_addr arc_to_action_class_addr)
{
  _sc_context_set_permissions_for_element(connector_addr, SC_CONTEXT_PERMISSIONS_TO_ALL_PERMISSIONS);

  sc_addr user_addr, action_class_addr;
  if (sc_memory_get_arc_info(s_memory_default_ctx, arc_to_action_class_addr, &user_addr, &action_class_addr)
      != SC_RESULT_OK)
    return;

  sc_permissions const permissions = sc_context_manager_get_basic_action_class_permissions(action_class_addr);

  sc_memory_context * ctx = _sc_memory_context_get_impl(manager, user_addr);
  if (ctx == null_ptr)
    _sc_context_add_user_global_permissions(user_addr, permissions);
  else
    _sc_context_add_context_global_permissions(ctx, permissions);
}

/*! Function that appends permissions for a memory context by adding relation between user and action class.
 * @param event Pointer to the sc-event triggering the addition of the action class.
 * @param initiator_addr sc-address representing user that initiated this request.
 * @param connector_addr sc-address representing created sc-connector to sc-pair with user and action class which this
 * use can complete.
 * @param connector_type sc-type of created sc-connector to sc-pair with user and action class.
 * @param arc_to_action_class_addr sc-address representing sc-pair with user and action class.
 * @returns Returns a result code indicating the success or failure of the operation (SC_RESULT_OK on success).
 * @note The function is called in response to a sc-event and is responsible for appending permissions for users and
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
 * this callback add permissions for user sc-memory context and assigns permissions to this sc-arc.
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

  // Only positive access sc-arcs can be used
  if (sc_type_has_not_subtype(connector_type, sc_type_arc_pos_const))
    return SC_RESULT_OK;

  _sc_context_set_permissions_for_element(connector_addr, SC_CONTEXT_PERMISSIONS_TO_ALL_PERMISSIONS);

  sc_memory_context_manager * manager = sc_event_get_data(event);
  _sc_memory_context_manager_add_user_action_class(manager, connector_addr, arc_to_action_class_addr);

  return SC_RESULT_OK;
}

/**
 * @brief Handles the removal of an action class from a user in the sc-memory context manager.
 *
 * This function is triggered by an sc-event when an action class is removed from a user. It updates the
 * permissions of the user or its sc-memory context accordingly.
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

  // Only positive access sc-arcs can be used
  if (sc_type_has_not_subtype(connector_type, sc_type_arc_pos_const))
    return SC_RESULT_OK;

  sc_addr user_addr, action_class_addr;
  if (sc_memory_get_arc_info(s_memory_default_ctx, arc_to_action_class_addr, &user_addr, &action_class_addr)
      != SC_RESULT_OK)
    return SC_RESULT_OK;

  sc_memory_context_manager * manager = sc_event_get_data(event);
  sc_permissions const permissions = sc_context_manager_get_basic_action_class_permissions(action_class_addr);

  sc_memory_context * ctx = _sc_memory_context_get_impl(manager, user_addr);
  if (ctx == null_ptr)
    _sc_context_remove_user_global_permissions(user_addr, permissions);
  else
    _sc_context_remove_context_global_permissions(ctx, permissions);

  sc_addr const action_arc_addr = sc_memory_arc_new(
      s_memory_default_ctx, sc_type_arc_neg_const_temp, manager->nrel_user_action_class_addr, arc_to_action_class_addr);
  _sc_context_set_permissions_for_element(action_arc_addr, SC_CONTEXT_PERMISSIONS_TO_ALL_PERMISSIONS);

  return SC_RESULT_OK;
}

/**
 * @brief Adds local permissions (within sc-structure) for a specific user in the context manager.
 * @param _user_addr sc-address of the user.
 * @param _adding_permissions Permissions to be added.
 * @return None.
 */
#define _sc_context_add_user_local_permissions(_user_addr, _adding_permissions, _structure_addr) \
  ({ \
    sc_monitor_acquire_write(&manager->user_local_permissions_monitor); \
    sc_hash_table * structures_permissions_table = \
        sc_hash_table_get(manager->user_local_permissions, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_user_addr))); \
    sc_permissions _user_permissions = 0; \
    if (structures_permissions_table == null_ptr) \
    { \
      structures_permissions_table = sc_hash_table_init(g_direct_hash, g_direct_equal, null_ptr, null_ptr); \
      sc_hash_table_insert( \
          manager->user_local_permissions, \
          GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_user_addr)), \
          structures_permissions_table); \
    } \
    else \
      _user_permissions = (sc_uint64)sc_hash_table_get( \
          structures_permissions_table, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_structure_addr))); \
    _user_permissions |= (_adding_permissions); \
    sc_hash_table_insert( \
        structures_permissions_table, \
        GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_structure_addr)), \
        GINT_TO_POINTER(_user_permissions)); \
    sc_monitor_release_write(&manager->user_local_permissions_monitor); \
  })

/**
 * @brief Removes local permissions (within sc-structure) for a specific user in the context manager.
 * @param _user_addr sc-address of the user.
 * @param _removing_permissions Permissions to be removed.
 * @return None.
 */
#define _sc_context_remove_user_local_permissions(_user_addr, _removing_permissions, _structure_addr) \
  ({ \
    sc_monitor_acquire_write(&manager->user_local_permissions_monitor); \
    sc_hash_table * structures_permissions_table = \
        sc_hash_table_get(manager->user_local_permissions, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_user_addr))); \
    sc_permissions _user_permissions = 0; \
    if (structures_permissions_table != null_ptr) \
    { \
      _user_permissions = (sc_uint64)sc_hash_table_get( \
          structures_permissions_table, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_structure_addr))); \
      _user_permissions &= ~(_removing_permissions); \
      sc_hash_table_insert( \
          structures_permissions_table, \
          GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_structure_addr)), \
          GINT_TO_POINTER(_user_permissions)); \
    } \
    sc_monitor_release_write(&manager->user_local_permissions_monitor); \
  })

/**
 * @brief Adds local permissions (within sc-structure) to a given sc-memory context.
 * @param _context Pointer to the sc-memory context.
 * @param _adding_permissions Permissions to be added.
 * @return None.
 */
#define _sc_context_add_context_local_permissions(_context, _adding_permissions, _structure_addr) \
  ({ \
    _sc_context_add_user_local_permissions(_context->user_addr, _adding_permissions, _structure_addr); \
    if (_context->local_permissions == null_ptr) \
    { \
      sc_monitor_acquire_write(&manager->user_local_permissions_monitor); \
      _context->local_permissions = sc_hash_table_get( \
          manager->user_local_permissions, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_context->user_addr))); \
      sc_monitor_release_write(&manager->user_local_permissions_monitor); \
    } \
  })

/**
 * @brief Removes local permissions (within sc-structure) from a given sc-memory context.
 * @param _context Pointer to the sc-memory context.
 * @param _removing_permissions Permissions to be removed.
 * @return None.
 */
#define _sc_context_remove_context_local_permissions(_context, _removing_permissions, _structure_addr) \
  _sc_context_remove_user_local_permissions(_context->user_addr, _removing_permissions, _structure_addr)

/*! Function that adds a new user action class within a structure, updating permissions accordingly.
 * @param manager Pointer to the sc-memory context manager.
 * @param connector_addr sc-address representing the created sc-connector connecting user and action class.
 * @param arc_to_arc_between_action_class_and_structure_addr sc-address representing sc-pair between action class and
 * structure.
 * @note This function is called when a new user action class is added within a structure. It updates permissions
 * for the user's sc-memory context and assigns permissions to the sc-arc connecting user and action class.
 */
void _sc_memory_context_manager_add_user_action_class_within_structure(
    sc_memory_context_manager * manager,
    sc_addr connector_addr,
    sc_addr arc_to_arc_between_action_class_and_structure_addr)
{
  _sc_context_set_permissions_for_element(connector_addr, SC_CONTEXT_PERMISSIONS_TO_ALL_PERMISSIONS);

  sc_addr user_addr, arc_between_action_class_and_structure;
  if (sc_memory_get_arc_info(
          s_memory_default_ctx,
          arc_to_arc_between_action_class_and_structure_addr,
          &user_addr,
          &arc_between_action_class_and_structure)
      != SC_RESULT_OK)
    return;

  sc_addr action_class_addr, structure_addr;
  if (sc_memory_get_arc_info(
          s_memory_default_ctx, arc_between_action_class_and_structure, &action_class_addr, &structure_addr)
      != SC_RESULT_OK)
    return;

  _sc_context_set_permissions_for_element(structure_addr, SC_CONTEXT_PERMITTED_STRUCTURE);

  sc_permissions const permissions = sc_context_manager_get_basic_action_class_permissions(action_class_addr);

  sc_memory_context * ctx = _sc_memory_context_get_impl(manager, user_addr);
  if (ctx == null_ptr)
    _sc_context_add_user_local_permissions(user_addr, permissions, structure_addr);
  else
    _sc_context_add_context_local_permissions(ctx, permissions, structure_addr);
}

/*! Function that removes a user action class within a structure, adjusting permissions accordingly.
 * @param manager Pointer to the sc-memory context manager.
 * @param connector_addr sc-address representing the sc-connector connecting user and action class.
 * @param arc_to_arc_between_action_class_and_structure_addr sc-address representing sc-pair between action class and
 * structure.
 * @note This function is called when a user action class is removed within a structure. It adjusts permissions
 * for the user's sc-memory context and removes permissions assigned to the sc-arc connecting user and action class.
 */
void _sc_memory_context_manager_remove_user_action_class_within_structure(
    sc_memory_context_manager * manager,
    sc_addr connector_addr,
    sc_addr arc_to_arc_between_action_class_and_structure_addr)
{
  sc_unused(&connector_addr);

  sc_addr user_addr, arc_between_action_class_and_structure;
  if (sc_memory_get_arc_info(
          s_memory_default_ctx,
          arc_to_arc_between_action_class_and_structure_addr,
          &user_addr,
          &arc_between_action_class_and_structure)
      != SC_RESULT_OK)
    return;

  sc_addr action_class_addr, structure_addr;
  if (sc_memory_get_arc_info(
          s_memory_default_ctx, arc_between_action_class_and_structure, &action_class_addr, &structure_addr)
      != SC_RESULT_OK)
    return;

  sc_permissions const permissions = sc_context_manager_get_basic_action_class_permissions(action_class_addr);

  sc_memory_context * ctx = _sc_memory_context_get_impl(manager, user_addr);
  if (ctx == null_ptr)
    _sc_context_remove_user_local_permissions(user_addr, permissions, structure_addr);
  else
    _sc_context_remove_context_local_permissions(ctx, permissions, structure_addr);
}

/*! Callback function triggered when a new user action class is added within a structure.
 * @param event Pointer to the sc-event triggering the addition of the action class within a structure.
 * @param initiator_addr sc-address representing the user that initiated this request.
 * @param connector_addr sc-address representing created sc-connector connecting user and action class within a
 * structure.
 * @param connector_type sc-type of the created sc-connector.
 * @param arc_to_arc_between_action_class_and_structure_addr sc-address representing sc-pair between action class and
 * structure.
 * @returns Returns a result code indicating the success or failure of the operation (SC_RESULT_OK on success).
 * @note This function is called in response to a sc-event and is responsible for handling the addition of a user
 * action class within a structure.
 */
sc_result _sc_memory_context_manager_on_new_user_action_class_within_structure(
    sc_event const * event,
    sc_addr initiator_addr,
    sc_addr connector_addr,
    sc_type connector_type,
    sc_addr arc_to_arc_between_action_class_and_structure_addr)
{
  sc_unused(&initiator_addr);

  sc_memory_context_manager * manager = sc_event_get_data(event);

  // Only positive access sc-arcs can be used
  if (sc_type_has_subtype_in_mask(connector_type, sc_type_arc_pos_const))
  {
    _sc_memory_context_manager_add_user_action_class_within_structure(
        manager, connector_addr, arc_to_arc_between_action_class_and_structure_addr);
    return SC_RESULT_OK;
  }

  if (sc_type_has_subtype_in_mask(connector_type, sc_type_arc_neg_const))
  {
    _sc_memory_context_manager_remove_user_action_class_within_structure(
        manager, connector_addr, arc_to_arc_between_action_class_and_structure_addr);
    return SC_RESULT_OK;
  }

  return SC_RESULT_OK;
}

/*! Callback function triggered when a user action class is removed within a structure.
 * @param event Pointer to the sc-event triggering the removal of the action class within a structure.
 * @param initiator_addr sc-address representing the user that initiated this request.
 * @param connector_addr sc-address representing sc-connector connecting user and action class within a structure.
 * @param connector_type sc-type of the sc-connector.
 * @param arc_to_arc_between_action_class_and_structure_addr sc-address representing sc-pair between action class and
 * structure.
 * @returns Returns a result code indicating the success or failure of the operation (SC_RESULT_OK on success).
 * @note This function is called in response to a sc-event and is responsible for handling the removal of a user
 * action class within a structure.
 */
sc_result _sc_memory_context_manager_on_remove_user_action_class_within_structure(
    sc_event const * event,
    sc_addr initiator_addr,
    sc_addr connector_addr,
    sc_type connector_type,
    sc_addr arc_to_arc_between_action_class_and_structure_addr)
{
  sc_unused(&initiator_addr);
  sc_unused(&connector_addr);

  // Only positive access sc-arcs can be used
  if (sc_type_has_not_subtype(connector_type, sc_type_arc_pos_const))
    return SC_RESULT_OK;

  sc_memory_context_manager * manager = sc_event_get_data(event);
  _sc_memory_context_manager_remove_user_action_class_within_structure(
      manager, connector_addr, arc_to_arc_between_action_class_and_structure_addr);

  sc_addr const action_arc_addr = sc_memory_arc_new(
      s_memory_default_ctx,
      sc_type_arc_neg_const_temp,
      manager->nrel_user_action_class_addr,
      arc_to_arc_between_action_class_and_structure_addr);
  _sc_context_set_permissions_for_element(action_arc_addr, SC_CONTEXT_PERMISSIONS_TO_ALL_PERMISSIONS);

  return SC_RESULT_OK;
}

/*! Function that iterates through all output arcs from a specified permitted relation with a given arc mask,
 * invoking a provided handler function for each matching arc.
 * @param manager Pointer to the sc-memory context manager.
 * @param permitted_relation sc-address representing the permitted relation for which output arcs are iterated.
 * @param arc_mask sc-type mask used to filter the output arcs.
 * @param handler Function pointer to the handler function that is invoked for each matching arc.
 *                The handler function must have the signature: void handler(sc_memory_context_manager * manager,
 *                sc_addr arc_addr, sc_addr target_arc_addr).
 * @note This function iterates through all output arcs from the specified permitted relation that match the provided
 * arc mask. For each matching arc, the handler function is called, providing the sc-memory context manager, arc
 * address, and target arc address.
 */
void _sc_memory_context_manager_iterate_by_all_output_arcs_from_permitted_relation(
    sc_memory_context_manager * manager,
    sc_addr const permitted_relation,
    sc_type const arc_mask,
    sc_permissions_handler handler)
{
  sc_iterator3 * it3 = sc_iterator3_f_a_a_new(s_memory_default_ctx, permitted_relation, 0, sc_type_arc_common);
  while (sc_iterator3_next(it3))
  {
    sc_addr const arc_addr = sc_iterator3_value(it3, 1);
    sc_type arc_type;
    if (sc_memory_get_element_type(s_memory_default_ctx, arc_addr, &arc_type) != SC_RESULT_OK
        || sc_type_has_not_subtype(arc_type, arc_mask))
      continue;

    sc_addr const target_arc_addr = sc_iterator3_value(it3, 2);
    handler(manager, arc_addr, target_arc_addr);
  }
  sc_iterator3_free(it3);
}

void _sc_memory_context_handle_all_user_permissions(sc_memory_context_manager * manager)
{
  if (manager->user_mode == SC_FALSE)
    return;

  _sc_memory_context_manager_iterate_by_all_output_arcs_from_permitted_relation(
      manager,
      manager->nrel_user_action_class_addr,
      sc_type_arc_pos_const,
      _sc_memory_context_manager_add_user_action_class);

  _sc_memory_context_manager_iterate_by_all_output_arcs_from_permitted_relation(
      manager,
      manager->nrel_user_action_class_within_sc_structure_addr,
      sc_type_arc_pos_const,
      _sc_memory_context_manager_add_user_action_class_within_structure);

  _sc_memory_context_manager_iterate_by_all_output_arcs_from_permitted_relation(
      manager,
      manager->nrel_user_action_class_within_sc_structure_addr,
      sc_type_arc_neg_const,
      _sc_memory_context_manager_remove_user_action_class_within_structure);
}

#define sc_context_manager_register_user_event(...) manager->user_mode ? sc_event_with_user_new(__VA_ARGS__) : null_ptr

#define sc_context_manager_unregister_user_event(...) sc_event_destroy(__VA_ARGS__)

void _sc_memory_context_manager_register_user_events(sc_memory_context_manager * manager)
{
  manager->concept_guest_user_addr = concept_guest_user_addr;
  _sc_context_set_permissions_for_element(manager->concept_guest_user_addr, SC_CONTEXT_PERMISSIONS_TO_ALL_PERMISSIONS);

  manager->concept_authentication_request_user_addr = concept_authentication_request_user_addr;
  _sc_context_set_permissions_for_element(
      manager->concept_authentication_request_user_addr, SC_CONTEXT_PERMISSIONS_TO_ALL_PERMISSIONS);

  manager->concept_authenticated_user_addr = concept_authenticated_user_addr;
  _sc_context_set_permissions_for_element(
      manager->concept_authenticated_user_addr, SC_CONTEXT_PERMISSIONS_TO_ALL_PERMISSIONS);

  manager->on_authentication_request_user_subscription = sc_context_manager_register_user_event(
      s_memory_default_ctx,
      manager->concept_authentication_request_user_addr,
      SC_EVENT_ADD_OUTPUT_ARC,
      manager,
      _sc_memory_context_manager_on_authentication_request_user,
      null_ptr);
  manager->on_remove_authenticated_user_subscription = sc_context_manager_register_user_event(
      s_memory_default_ctx,
      manager->concept_authenticated_user_addr,
      SC_EVENT_REMOVE_OUTPUT_ARC,
      manager,
      _sc_memory_context_manager_on_unauthentication_request_user,
      null_ptr);

  manager->nrel_user_action_class_addr = nrel_user_action_class_addr;
  _sc_context_set_permissions_for_element(
      manager->nrel_user_action_class_addr, SC_CONTEXT_PERMISSIONS_TO_ALL_PERMISSIONS);

  sc_context_manager_add_basic_action_class_permissions(action_read_from_sc_memory_addr, SC_CONTEXT_PERMISSIONS_READ);
  sc_context_manager_add_basic_action_class_permissions(
      action_generate_in_sc_memory_addr, SC_CONTEXT_PERMISSIONS_WRITE);
  sc_context_manager_add_basic_action_class_permissions(action_erase_in_sc_memory_addr, SC_CONTEXT_PERMISSIONS_ERASE);
  sc_context_manager_add_basic_action_class_permissions(
      action_read_permissions_from_sc_memory_addr, SC_CONTEXT_PERMISSIONS_TO_READ_PERMISSIONS);
  sc_context_manager_add_basic_action_class_permissions(
      action_generate_permissions_in_sc_memory_addr, SC_CONTEXT_PERMISSIONS_TO_WRITE_PERMISSIONS);
  sc_context_manager_add_basic_action_class_permissions(
      action_erase_permissions_from_sc_memory_addr, SC_CONTEXT_PERMISSIONS_TO_ERASE_PERMISSIONS);

  manager->on_new_user_action_class = sc_context_manager_register_user_event(
      s_memory_default_ctx,
      manager->nrel_user_action_class_addr,
      SC_EVENT_ADD_OUTPUT_ARC,
      manager,
      _sc_memory_context_manager_on_new_user_action_class,
      null_ptr);

  manager->on_remove_user_action_class = sc_context_manager_register_user_event(
      s_memory_default_ctx,
      manager->nrel_user_action_class_addr,
      SC_EVENT_REMOVE_OUTPUT_ARC,
      manager,
      _sc_memory_context_manager_on_remove_user_action_class,
      null_ptr);

  manager->nrel_user_action_class_within_sc_structure_addr = nrel_user_action_class_within_sc_structure_addr;
  _sc_context_set_permissions_for_element(
      manager->nrel_user_action_class_within_sc_structure_addr, SC_CONTEXT_PERMISSIONS_TO_ALL_PERMISSIONS);

  manager->on_new_user_action_class_within_sc_structure = sc_context_manager_register_user_event(
      s_memory_default_ctx,
      manager->nrel_user_action_class_within_sc_structure_addr,
      SC_EVENT_ADD_OUTPUT_ARC,
      manager,
      _sc_memory_context_manager_on_new_user_action_class_within_structure,
      null_ptr);

  manager->on_remove_user_action_class_within_sc_structure = sc_context_manager_register_user_event(
      s_memory_default_ctx,
      manager->nrel_user_action_class_within_sc_structure_addr,
      SC_EVENT_REMOVE_OUTPUT_ARC,
      manager,
      _sc_memory_context_manager_on_remove_user_action_class_within_structure,
      null_ptr);
}

void _sc_memory_context_manager_unregister_user_events(sc_memory_context_manager * manager)
{
  sc_context_manager_unregister_user_event(manager->on_authentication_request_user_subscription);
  sc_context_manager_unregister_user_event(manager->on_remove_authenticated_user_subscription);

  sc_context_manager_unregister_user_event(manager->on_new_user_action_class);
  sc_context_manager_unregister_user_event(manager->on_remove_user_action_class);

  sc_context_manager_unregister_user_event(manager->on_new_user_action_class_within_sc_structure);
  sc_context_manager_unregister_user_event(manager->on_remove_user_action_class_within_sc_structure);
}

// If the system is not in user mode, grant permissions
#define _sc_memory_context_check_system(_manager, _context) \
  (manager->user_mode == SC_FALSE || (ctx->flags & SC_CONTEXT_FLAG_SYSTEM) == SC_CONTEXT_FLAG_SYSTEM)

sc_bool _sc_memory_context_is_authenticated(sc_memory_context_manager * manager, sc_memory_context const * ctx)
{
  if (_sc_memory_context_check_system(manager, ctx))
    return SC_TRUE;

  sc_permissions const context_permissions = _sc_monitor_get_context_global_permissions(ctx);

  sc_bool const is_authenticated =
      sc_context_has_permissions_subset(context_permissions, SC_CONTEXT_PERMISSIONS_AUTHENTICATED);
  return is_authenticated;
}

sc_bool _sc_memory_context_check_if_has_permitted_structure(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_permissions action_class_permissions,
    sc_addr element_addr)
{
  if (_sc_memory_context_check_system(manager, ctx))
    return SC_TRUE;

  sc_bool result = SC_FALSE;

  sc_monitor_acquire_read((sc_monitor *)&ctx->monitor);
  sc_hash_table * permissions_table = ctx->local_permissions;

  // If element is permitted structure
  if (permissions_table == null_ptr)
    goto result;

  sc_permissions permissions =
      (sc_uint64)sc_hash_table_get(permissions_table, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(element_addr)));
  result = sc_context_has_permissions_subset(permissions, action_class_permissions);

result:
  sc_monitor_release_read((sc_monitor *)&ctx->monitor);

  return result;
}

#define _sc_memory_check_if_is_permitted_structure(_structure_addr) \
  ({ \
    sc_permissions const _structure_permissions = _sc_context_get_permissions_for_element(_structure_addr); \
    sc_bool const _result = sc_context_has_permissions_subset(_structure_permissions, SC_CONTEXT_PERMITTED_STRUCTURE); \
    _result; \
  })

sc_result _sc_memory_context_check_local_permissions(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_permissions action_class_permissions,
    sc_addr element_addr)
{
  if (_sc_memory_context_check_system(manager, ctx))
    return SC_RESULT_OK;

  sc_result result = SC_RESULT_UNKNOWN;

  sc_monitor_acquire_read((sc_monitor *)&ctx->monitor);
  sc_hash_table * permissions_table = ctx->local_permissions;

  if (permissions_table == null_ptr)
    goto result;

  sc_iterator3 * it3 = sc_iterator3_a_a_f_new(
      s_memory_default_ctx, sc_type_node | sc_type_const | sc_type_node_struct, sc_type_arc_pos_const, element_addr);
  while (result != SC_RESULT_OK && sc_iterator3_next(it3))
  {
    sc_addr const structure_addr = sc_iterator3_value(it3, 0);
    if (_sc_memory_check_if_is_permitted_structure(structure_addr) == SC_FALSE)
      continue;

    sc_permissions const permissions =
        (sc_uint64)sc_hash_table_get(permissions_table, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(structure_addr)));
    result = sc_context_has_permissions_subset(permissions, action_class_permissions) ? SC_RESULT_OK : SC_RESULT_NO;
  }
  sc_iterator3_free(it3);

result:
  sc_monitor_release_read((sc_monitor *)&ctx->monitor);

  return result;
}

sc_bool _sc_memory_context_check_global_permissions(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_permissions action_class_permissions)
{
  if (_sc_memory_context_check_system(manager, ctx))
    return SC_TRUE;

  sc_permissions const context_permissions = _sc_monitor_get_context_global_permissions(ctx);

  // Check if the sc-memory context has permissions to the action class
  sc_bool const result = sc_context_has_permissions_subset(context_permissions, action_class_permissions);
  return result;
}

sc_bool _sc_memory_context_check_local_and_global_permissions(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_permissions action_class_permissions,
    sc_addr element_addr)
{
  sc_result const result =
      _sc_memory_context_check_local_permissions(manager, ctx, action_class_permissions, element_addr);
  return result == SC_RESULT_OK
         || (result == SC_RESULT_UNKNOWN
             && _sc_memory_context_check_global_permissions(manager, ctx, action_class_permissions))
                == SC_TRUE;
}

sc_bool _sc_memory_context_check_global_permissions_to_read_permissions(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_element * permitted_element,
    sc_addr permitted_element_addr,
    sc_permissions required_permissions)
{
  sc_unused(&permitted_element_addr);

  if (_sc_memory_context_check_system(manager, ctx))
    return SC_TRUE;

  sc_permissions const context_permissions = _sc_monitor_get_context_global_permissions(ctx);

  sc_permissions const element_permissions = permitted_element->flags.states;

  // Check if the sc-memory context has read access to the element
  sc_permissions const required_context_permissions = context_permissions & required_permissions;
  sc_permissions const required_element_permissions = element_permissions & required_permissions;

  sc_bool result = sc_context_has_permissions_subset(required_context_permissions, required_element_permissions);
  return result;
}

sc_bool _sc_memory_context_check_global_permissions_to_handle_permissions(
    sc_memory_context const * ctx,
    sc_addr permitted_element_addr,
    sc_permissions required_permissions)
{
  sc_permissions const context_permissions = _sc_monitor_get_context_global_permissions(ctx);
  sc_permissions const element_permissions = _sc_context_get_permissions_for_element(permitted_element_addr);

  // Check if the sc-memory context has access to handle the operation
  sc_permissions const required_context_permissions = context_permissions & required_permissions;
  sc_permissions const required_element_permissions = element_permissions & required_permissions;

  sc_bool const result = sc_context_has_permissions_subset(required_context_permissions, required_element_permissions);
  return result;
}

sc_bool _sc_memory_context_check_global_permissions_to_write_permissions(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_addr permitted_element_addr,
    sc_type connector_from_element_type,
    sc_permissions required_permissions)
{
  if (_sc_memory_context_check_system(manager, ctx))
    return SC_TRUE;

  if (sc_type_has_not_subtype(connector_from_element_type, sc_type_arc_pos_const_temp))
    return SC_TRUE;

  return _sc_memory_context_check_global_permissions_to_handle_permissions(
      ctx, permitted_element_addr, required_permissions);
}

sc_bool _sc_memory_context_check_global_permissions_to_erase_permissions(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_addr permitted_element_addr,
    sc_permissions required_permissions)
{
  if (_sc_memory_context_check_system(manager, ctx))
    return SC_TRUE;

  return _sc_memory_context_check_global_permissions_to_handle_permissions(
      ctx, permitted_element_addr, required_permissions);
}
