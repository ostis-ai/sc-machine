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
    sc_addr user_addr);

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
    sc_addr user_addr);

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
    sc_addr arc_to_action_class_addr);

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
    sc_addr arc_to_action_class_addr);

/*! Function that registers event subscriptions for user authentication and unauthentication.
 * @param manager Pointer to the memory context manager for which events are registered.
 * @note This function sets up event subscriptions for user authentication and unauthentication events.
 */
void _sc_memory_context_manager_register_user_events(sc_memory_context_manager * manager);

/*! Function that unregisters event subscriptions for user authentication and unauthentication.
 * @param manager Pointer to the memory context manager for which events are unregistered.
 * @note This function releases resources associated with event subscriptions for user authentication and
 * unauthentication.
 */
void _sc_memory_context_manager_unregister_user_events(sc_memory_context_manager * manager);

/*! Function that checks if a memory context is authorized.
 * @param manager Pointer to the memory context manager responsible for context authentication checks.
 * @param ctx Pointer to the memory context to be checked for authentication.
 * @returns Returns SC_TRUE if the context is authorized, SC_FALSE otherwise.
 * @note This function checks if a memory context is authorized based on the access levels.
 */
sc_bool _sc_memory_context_is_authenticated(sc_memory_context_manager * manager, sc_memory_context const * ctx);

/**
 * @brief Checks if the sc-memory context has access to a given action class.
 *
 * This function checks if the sc-memory context has access to a specified action class based on
 * the provided access levels.
 *
 * @param manager Pointer to the sc-memory context manager.
 * @param ctx Pointer to the sc-memory context to be checked.
 * @param action_class_access_levels Access levels required for the action class.
 * @return Returns SC_TRUE if the sc-memory context has access; otherwise, returns SC_FALSE.
 */
sc_bool _sc_memory_context_check_global_access_levels(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_access_levels action_class_access_levels);

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
sc_bool _sc_memory_context_global_access_levels_to_erase_access_levels(
    sc_memory_context_manager * manager,
    sc_memory_context const * ctx,
    sc_addr accessed_element_addr,
    sc_access_levels required_access_levels);

#endif
