/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_memory_context_permissions_h_
#define _sc_memory_context_permissions_h_

#include "sc_memory.h"
#include "sc-store/sc_event_subscription.h"

#include "sc-store/sc-base/sc_monitor_table.h"

#include "sc_memory_context_manager.h"

#define SC_CONTEXT_FLAG_SYSTEM 0x10

/**
 * @brief Sets permissions for a specific sc-memory element.
 * @param _element_addr Address of the sc-memory element.
 * @param _permissions Permissions to be set.
 * @return None.
 */
#define _sc_context_set_permissions_for_element(_element_addr, _permissions) \
  ({ \
    sc_monitor * _monitor = \
        sc_monitor_table_get_monitor_for_addr(&sc_storage_get()->addr_monitors_table, _element_addr); \
    sc_monitor_acquire_write(_monitor); \
    sc_element * _element; \
    sc_storage_get_element_by_addr(_element_addr, &_element); \
    if (_element != null_ptr) \
      _element->flags.states |= _permissions; \
    sc_monitor_release_write(_monitor); \
  })

//! Gets permissions of a specific sc-memory element.
#define _sc_context_get_permissions_for_element(_element_addr) \
  ({ \
    sc_monitor * _monitor = \
        sc_monitor_table_get_monitor_for_addr(&sc_storage_get()->addr_monitors_table, _element_addr); \
    sc_monitor_acquire_read(_monitor); \
    sc_element * _element; \
    sc_storage_get_element_by_addr(_element_addr, &_element); \
    sc_permissions const _element_permissions = _element->flags.states; \
    sc_monitor_release_read(_monitor); \
    _element_permissions; \
  })

#define _sc_context_get_user_global_permissions(_user_addr) \
  ({ \
    sc_monitor_acquire_read(&manager->user_global_permissions_monitor); \
    sc_permissions const permissions = (sc_uint64)sc_hash_table_get( \
        manager->user_global_permissions, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_user_addr))); \
    sc_monitor_release_read(&manager->user_global_permissions_monitor); \
    permissions; \
  })

#define _sc_context_get_user_local_permissions(_user_addr) \
  ({ \
    sc_monitor_acquire_read(&manager->user_local_permissions_monitor); \
    sc_hash_table * permissions = \
        sc_hash_table_get(manager->user_local_permissions, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_user_addr))); \
    sc_monitor_release_read(&manager->user_local_permissions_monitor); \
    permissions; \
  })

sc_addr _sc_memory_context_manager_generate_guest_user(sc_memory_context_manager * manager);

/*! Function that handles all user permissions by iterating through relevant relations and invoking corresponding
 * handlers.
 * @param manager Pointer to the sc-memory context manager.
 * @note This function is responsible for handling all user permissions within the given memory context manager.
 * It achieves this by iterating through specific relations related to user action classes and user action classes
 * within sc-structures. For each relevant relation, the corresponding handler function is invoked to manage the
 * permissions permissions accordingly. The handlers used are:
 * - _sc_memory_context_manager_add_user_action_class: Adds a new user action class and updates permissions.
 * - _sc_memory_context_manager_add_user_action_class_within_structure: Adds a new user action class within a structure
 * and updates permissions.
 * - _sc_memory_context_manager_remove_user_action_class_within_structure: Removes a user action class within a
 * structure and adjusts permissions.
 */
void _sc_memory_context_handle_all_user_permissions(sc_memory_context_manager * manager);

/*! Function that registers event subscriptions for user authentication and unauthentication.
 * @param manager Pointer to the sc-memory context manager for which events are registered.
 * @note This function sets up event subscriptions for user authentication and unauthentication events.
 */
void _sc_memory_context_manager_register_user_events(sc_memory_context_manager * manager);

/*! Function that unregisters event subscriptions for user authentication and unauthentication.
 * @param manager Pointer to the sc-memory context manager for which events are unregistered.
 * @note This function releases resources associated with event subscriptions for user authentication and
 * unauthentication.
 */
void _sc_memory_context_manager_unregister_user_events(sc_memory_context_manager * manager);

/*! Function that checks if a memory context is authorized.
 * @param manager Pointer to the sc-memory context manager responsible for context authentication checks.
 * @param ctx Pointer to the sc-memory context to be checked for authentication.
 * @returns Returns SC_TRUE if the context is authorized, SC_FALSE otherwise.
 * @note This function checks if a memory context is authorized based on the permissions.
 */
sc_bool _sc_memory_context_is_authenticated(sc_memory_context_manager * manager, sc_memory_context const * ctx);

/*! Function that checks if an element is an permitted structure within a specific memory context.
 * @param manager Pointer to the sc-memory context manager.
 * @param ctx Pointer to the sc-memory context in which the check is performed.
 * @param action_class_permissions Permissions associated with the action class for the check.
 * @param element_addr sc-address representing the element to be checked.
 * @returns Returns SC_TRUE if the element is an permitted structure with the specified permissions, otherwise
 * SC_FALSE.
 * @note This function checks if the provided element is an permitted structure within the given memory context. It
 * compares the permissions associated with the element against the permissions of the action class. If the
 * permissions match, the function returns SC_TRUE; otherwise, it returns SC_FALSE.
 */
sc_bool _sc_memory_context_check_if_has_permitted_structure(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_permissions action_class_permissions,
    sc_addr element_addr);

/*! Function that checks local permissions for a given element within a specific memory context.
 * @param manager Pointer to the sc-memory context manager.
 * @param ctx Pointer to the sc-memory context in which the check is performed.
 * @param action_class_permissions Permissions associated with the action class for the check.
 * @param element_addr sc-address representing the element to be checked.
 * @returns Returns SC_RESULT_OK if the local permissions match the action class permissions, otherwise
 * SC_RESULT_NO.
 * @note This function checks the local permissions associated with the provided element within the given memory
 * context. It compares the local permissions against the permissions of the action class. If the permissions
 * match, the function returns SC_RESULT_OK; otherwise, it returns SC_RESULT_NO.
 */
sc_result _sc_memory_context_check_local_permissions(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_permissions action_class_permissions,
    sc_addr element_addr);

/*! Function that checks global permissions for a given memory context.
 * @param manager Pointer to the sc-memory context manager.
 * @param ctx Pointer to the sc-memory context for which global permissions are checked.
 * @param action_class_permissions Permissions associated with the action class for the check.
 * @returns Returns SC_TRUE if the global permissions match the action class permissions, otherwise SC_FALSE.
 * @note This function checks the global permissions associated with the provided memory context against the
 * permissions of the action class. If the permissions match, the function returns SC_TRUE; otherwise, it returns
 * SC_FALSE.
 */
sc_bool _sc_memory_context_check_global_permissions(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_permissions action_class_permissions);

/*! Function that checks both local and global permissions for a given element within a specific memory context.
 * @param manager Pointer to the sc-memory context manager.
 * @param ctx Pointer to the sc-memory context in which the check is performed.
 * @param action_class_permissions Permissions associated with the action class for the check.
 * @param element_addr sc-address representing the element to be checked.
 * @returns Returns SC_TRUE if either local or global permissions match the action class permissions, otherwise
 * SC_FALSE.
 * @note This function checks both local and global permissions associated with the provided element within the given
 * memory context. If either the local or global permissions match the permissions of the action class, the function
 * returns SC_TRUE; otherwise, it returns SC_FALSE.
 */
sc_bool _sc_memory_context_check_local_and_global_permissions(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_permissions action_class_permissions,
    sc_addr element_addr);

/**
 * @brief Checks if the sc-memory context has read permissions to a specified element.
 *
 * This function checks if the sc-memory context has read permissions to a specified element based on
 * the required permissions.
 *
 * @param manager Pointer to the sc-memory context manager.
 * @param ctx Pointer to the sc-memory context to be checked.
 * @param permitted_element Pointer to the sc-element being permitted.
 * @param permitted_element_addr sc-address representing the permitted sc-element.
 * @param required_permissions Permissions required for the read operation.
 * @return Returns SC_TRUE if the sc-memory context has read permissions; otherwise, returns SC_FALSE.
 */
sc_bool _sc_memory_context_check_global_permissions_to_read_permissions(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_element * permitted_element,
    sc_addr permitted_element_addr,
    sc_permissions required_permissions);

/**
 * @brief Checks if the sc-memory context has write permissions to a specified element.
 *
 * This function checks if the sc-memory context has write permissions to a specified element based on
 * the required permissions.
 *
 * @param manager Pointer to the sc-memory context manager.
 * @param ctx Pointer to the sc-memory context to be checked.
 * @param permitted_element_addr sc-address representing the permitted sc-element.
 * @param connector_from_element_type sc-type of the arc connecting from the element (unused).
 * @param required_permissions Permissions required for the write operation.
 * @return Returns SC_TRUE if the sc-memory context has write permissions; otherwise, returns SC_FALSE.
 */
sc_bool _sc_memory_context_check_global_permissions_to_write_permissions(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_addr permitted_element_addr,
    sc_type connector_from_element_type,
    sc_permissions required_permissions);

/**
 * @brief Checks if the sc-memory context has permissions to erase a specified element.
 *
 * This function checks if the sc-memory context has permissions to erase a specified element based on
 * the required permissions.
 *
 * @param manager Pointer to the sc-memory context manager.
 * @param ctx Pointer to the sc-memory context to be checked.
 * @param permitted_element_addr sc-address representing the permitted sc-element.
 * @param required_permissions Permissions required for the erase operation.
 * @return Returns SC_TRUE if the sc-memory context has permissions; otherwise, returns SC_FALSE.
 */
sc_bool _sc_memory_context_check_global_permissions_to_erase_permissions(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_addr permitted_element_addr,
    sc_permissions required_permissions);

#endif
