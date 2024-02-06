/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_memory_context_access_levels_h_
#define _sc_memory_context_access_levels_h_

#include "sc_memory.h"
#include "sc-store/sc_event.h"

#include "sc_memory_context_manager.h"

#define SC_CONTEXT_FLAG_SYSTEM 0x2

/**
 * @brief Sets access levels for a specific sc-memory element.
 * @param _element_addr Address of the sc-memory element.
 * @param _access_levels Access levels to be set.
 * @return None.
 */
#define _sc_context_set_access_levels_for_element(_element_addr, _access_levels) \
  ({ \
    sc_monitor * _monitor = sc_monitor_get_monitor_for_addr(&sc_storage_get()->addr_monitors_table, _element_addr); \
    sc_monitor_acquire_write(_monitor); \
    sc_element * _element; \
    sc_storage_get_element_by_addr(_element_addr, &_element); \
    if (_element != null_ptr) \
      _element->flags.states |= _access_levels; \
    sc_monitor_release_write(_monitor); \
  })

//! Gets access levels of a specific sc-memory element.
#define _sc_context_get_access_levels_for_element(_element_addr) \
  ({ \
    sc_monitor * _monitor = sc_monitor_get_monitor_for_addr(&sc_storage_get()->addr_monitors_table, _element_addr); \
    sc_monitor_acquire_read(_monitor); \
    sc_element * _element; \
    sc_storage_get_element_by_addr(_element_addr, &_element); \
    sc_access_levels const _element_access_levels = _element->flags.states; \
    sc_monitor_release_read(_monitor); \
    _element_access_levels; \
  })

/*! Function that handles all user access levels by iterating through relevant relations and invoking corresponding
 * handlers.
 * @param manager Pointer to the sc-memory context manager.
 * @note This function is responsible for handling all user access levels within the given memory context manager.
 * It achieves this by iterating through specific relations related to user action classes and user action classes
 * within SC structures. For each relevant relation, the corresponding handler function is invoked to manage the access
 * levels accordingly. The handlers used are:
 * - _sc_memory_context_manager_add_user_action_class: Adds a new user action class and updates access levels.
 * - _sc_memory_context_manager_add_user_action_class_within_structure: Adds a new user action class within a structure
 * and updates access levels.
 * - _sc_memory_context_manager_remove_user_action_class_within_structure: Removes a user action class within a
 * structure and adjusts access levels.
 */
void _sc_memory_context_handle_all_user_access_levels(sc_memory_context_manager * manager);

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
 * @note This function checks if a memory context is authorized based on the access levels.
 */
sc_bool _sc_memory_context_is_authenticated(sc_memory_context_manager * manager, sc_memory_context const * ctx);

/*! Function that checks if an element is an accessed structure within a specific memory context.
 * @param manager Pointer to the sc-memory context manager.
 * @param ctx Pointer to the sc-memory context in which the check is performed.
 * @param action_class_access_levels Access levels associated with the action class for the check.
 * @param element_addr sc-address representing the element to be checked.
 * @returns Returns SC_TRUE if the element is an accessed structure with the specified access levels, otherwise
 * SC_FALSE.
 * @note This function checks if the provided element is an accessed structure within the given memory context. It
 * compares the access levels associated with the element against the access levels of the action class. If the access
 * levels match, the function returns SC_TRUE; otherwise, it returns SC_FALSE.
 */
sc_bool _sc_memory_context_check_if_is_accessed_structure(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_access_levels action_class_access_levels,
    sc_addr element_addr);

/*! Function that checks local access levels for a given element within a specific memory context.
 * @param manager Pointer to the sc-memory context manager.
 * @param ctx Pointer to the sc-memory context in which the check is performed.
 * @param action_class_access_levels Access levels associated with the action class for the check.
 * @param element_addr sc-address representing the element to be checked.
 * @returns Returns SC_RESULT_OK if the local access levels match the action class access levels, otherwise
 * SC_RESULT_NO.
 * @note This function checks the local access levels associated with the provided element within the given memory
 * context. It compares the local access levels against the access levels of the action class. If the access levels
 * match, the function returns SC_RESULT_OK; otherwise, it returns SC_RESULT_NO.
 */
sc_result _sc_memory_context_check_local_access_levels(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_access_levels action_class_access_levels,
    sc_addr element_addr);

/*! Function that checks global access levels for a given memory context.
 * @param manager Pointer to the sc-memory context manager.
 * @param ctx Pointer to the sc-memory context for which global access levels are checked.
 * @param action_class_access_levels Access levels associated with the action class for the check.
 * @returns Returns SC_TRUE if the global access levels match the action class access levels, otherwise SC_FALSE.
 * @note This function checks the global access levels associated with the provided memory context against the access
 * levels of the action class. If the access levels match, the function returns SC_TRUE; otherwise, it returns SC_FALSE.
 */
sc_bool _sc_memory_context_check_global_access_levels(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_access_levels action_class_access_levels);

/*! Function that checks both local and global access levels for a given element within a specific memory context.
 * @param manager Pointer to the sc-memory context manager.
 * @param ctx Pointer to the sc-memory context in which the check is performed.
 * @param action_class_access_levels Access levels associated with the action class for the check.
 * @param element_addr sc-address representing the element to be checked.
 * @returns Returns SC_TRUE if either local or global access levels match the action class access levels, otherwise
 * SC_FALSE.
 * @note This function checks both local and global access levels associated with the provided element within the given
 * memory context. If either the local or global access levels match the access levels of the action class, the function
 * returns SC_TRUE; otherwise, it returns SC_FALSE.
 */
sc_bool _sc_memory_context_check_local_and_global_access_levels(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_access_levels action_class_access_levels,
    sc_addr element_addr);

/**
 * @brief Checks if the sc-memory context has read access to a specified element.
 *
 * This function checks if the sc-memory context has read access to a specified element based on
 * the required access levels.
 *
 * @param manager Pointer to the sc-memory context manager.
 * @param ctx Pointer to the sc-memory context to be checked.
 * @param accessed_element Pointer to the sc-element being accessed.
 * @param accessed_element_addr sc-address representing the accessed sc-element.
 * @param required_access_levels Access levels required for the read operation.
 * @return Returns SC_TRUE if the sc-memory context has read access; otherwise, returns SC_FALSE.
 */
sc_bool _sc_memory_context_check_global_access_levels_to_read_access_levels(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_element * accessed_element,
    sc_addr accessed_element_addr,
    sc_access_levels required_access_levels);

/**
 * @brief Checks if the sc-memory context has write access to a specified element.
 *
 * This function checks if the sc-memory context has write access to a specified element based on
 * the required access levels.
 *
 * @param manager Pointer to the sc-memory context manager.
 * @param ctx Pointer to the sc-memory context to be checked.
 * @param accessed_element_addr sc-address representing the accessed sc-element.
 * @param connector_from_element_type sc-type of the arc connecting from the element (unused).
 * @param required_access_levels Access levels required for the write operation.
 * @return Returns SC_TRUE if the sc-memory context has write access; otherwise, returns SC_FALSE.
 */
sc_bool _sc_memory_context_check_global_access_levels_to_write_access_levels(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_addr accessed_element_addr,
    sc_type connector_from_element_type,
    sc_access_levels required_access_levels);

/**
 * @brief Checks if the sc-memory context has access to erase a specified element.
 *
 * This function checks if the sc-memory context has access to erase a specified element based on
 * the required access levels.
 *
 * @param manager Pointer to the sc-memory context manager.
 * @param ctx Pointer to the sc-memory context to be checked.
 * @param accessed_element_addr sc-address representing the accessed sc-element.
 * @param required_access_levels Access levels required for the erase operation.
 * @return Returns SC_TRUE if the sc-memory context has access; otherwise, returns SC_FALSE.
 */
sc_bool _sc_memory_context_check_global_access_levels_to_erase_access_levels(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_addr accessed_element_addr,
    sc_access_levels required_access_levels);

#endif
