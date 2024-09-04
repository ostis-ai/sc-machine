/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

/*!
 * @file sc_memory.h
 *
 * @brief This file contains the extended C API for managing sc-memory.
 *
 * In addition to the core C API this API include methods for create, manage and delete sc-memory contexts.
 * Sc-memory contexts are used to manage access rights of user sc-processes.
 *
 * @see sc_storage.h
 */

#ifndef _sc_memory_h_
#define _sc_memory_h_

#include "sc_memory_params.h"

#include "sc-store/sc_types.h"
#include "sc-store/sc_stream.h"
#include "sc-store/sc-container/sc-list/sc_list.h"

typedef struct _sc_memory sc_memory;

/*!
 * @brief Initializes the sc-memory subsystem.
 *
 * This function initializes the sc-memory subsystem with the specified parameters.
 * It configures the logger, initializes the storage, and loads extensions.
 *
 * @param params Pointer to the structure containing initialization parameters.
 * @param context Pointer to default system context pointer.
 *
 * @return Returns a pointer to the initialized sc-memory context if successful,
 *         or a null pointer if initialization fails.
 *
 * @note The caller is responsible for handling any errors and releasing resources
 *       associated with the sc-memory context.
 */
_SC_EXTERN sc_memory_context * sc_memory_initialize(sc_memory_params const * params, sc_memory_context ** context);

/*!
 * @brief Initializes sc-memory extensions.
 *
 * This function initializes sc-memory extensions based on the provided parameters.
 * It loads extensions from the specified directory and enables the ones specified
 * in the given list. Additionally, it resolves the identifier for the initial
 * memory generated structure.
 *
 * @param ext_path Path to the directory containing sc-memory extensions.
 * @param enabled_list Array of enabled extensions.
 * @param init_memory_generated_structure Identifier for the initial memory generated structure.
 *
 * @return Returns the result of the operation. If successful, it returns SC_RESULT_OK.
 *         If an error occurs during initialization, an appropriate error code is returned.
 *
 * @note The caller is responsible for handling any errors and ensuring proper shutdown
 *       of the initialized extensions.
 */
_SC_EXTERN sc_result sc_memory_init_ext(
    sc_char const * ext_path,
    sc_char const ** enabled_list,
    sc_addr const init_memory_generated_structure);

/*!
 * @brief Shuts down the sc-memory subsystem.
 *
 * This function shuts down the sc-memory subsystem, including the storage and extensions.
 * It also releases resources associated with the sc-memory context.
 *
 * @param save_state Flag indicating whether to save the state before shutting down.
 *                  If `save_state` is true, the state will be saved; otherwise, it won't be saved.
 *
 * @return Returns SC_RESULT_OK if the shutdown is successful; otherwise, an error code is returned.
 *
 * @note The caller is responsible for handling any errors and releasing resources
 *       associated with the sc-memory context.
 */
_SC_EXTERN sc_result sc_memory_shutdown(sc_bool save_state);

/*!
 * @brief Shuts down sc-memory extensions.
 *
 * This function shuts down sc-memory extensions. It triggers the shutdown process
 * for all loaded extensions, releasing any resources they might have acquired.
 *
 * @note The caller is responsible for ensuring that this function is called
 *       before shutting down the sc-memory subsystem.
 */
_SC_EXTERN void sc_memory_shutdown_ext();

/*!
 * Generates a new sc-memory context for a specified user.
 *
 * @param user_addr sc_addr representing the user for whom the context is created.
 *
 * @returns Returns a pointer to the newly created sc-memory context. If an error occurs, returns null_ptr.
 *
 * @note This function creates a new SC-memory context for the specified user, resolving it from the context manager.
 *
 * @see sc_memory_context_free
 */
_SC_EXTERN sc_memory_context * sc_memory_context_new_ext(sc_addr user_addr);

/*!
 * Frees an sc-memory context.
 *
 * This function destroys the sc-memory context that was created using `sc_memory_context_new_ext`.
 *
 * @param ctx Pointer to the sc-memory context to be freed.
 *
 * @note This function frees an sc-memory context, removing it from the context manager and releasing associated
 * resources.
 */
_SC_EXTERN void sc_memory_context_free(sc_memory_context * ctx);

_SC_EXTERN sc_addr sc_memory_context_get_user_addr(sc_memory_context * ctx);

/*!
 * @brief Starts events pending mode for a context.
 *
 * In this mode, all new emitted events will be pending until `sc_memory_context_pending_end` is called.
 *
 * @param ctx Pointer to the sc-memory context.
 *
 * @note Use this function to start a pending mode for events emitted by the specified context.
 * @see sc_memory_context_pending_end
 */
_SC_EXTERN void sc_memory_context_pending_begin(sc_memory_context * ctx);

/*!
 * @brief Ends events pending mode for a context.
 *
 * This function ends the pending mode for events emitted by the specified context.
 * All pending events will be emitted when this function is called.
 *
 * @param ctx Pointer to the sc-memory context.
 *
 * @note Use this function to end the pending mode for events emitted by the specified context.
 * @see sc_memory_context_pending_begin
 */
_SC_EXTERN void sc_memory_context_pending_end(sc_memory_context * ctx);

/*!
 * @brief Starts events blocking mode for a context.
 *
 * In this mode, all new emitted events will be blocking until `sc_memory_context_blocking_end` is called.
 *
 * @param ctx Pointer to the sc-memory context.
 *
 * @note Use this function to start a blocking mode for events emitted by the specified context.
 * @see sc_memory_context_blocking_end
 */
_SC_EXTERN void sc_memory_context_blocking_begin(sc_memory_context * ctx);

/*!
 * @brief Ends events blocking mode for a context.
 *
 * This function ends the blocking mode for events emitted by the specified context.
 *
 * @param ctx Pointer to the sc-memory context.
 *
 * @note Use this function to end the blocking mode for events emitted by the specified context.
 * @see sc_memory_context_blocking_begin
 */
_SC_EXTERN void sc_memory_context_blocking_end(sc_memory_context * ctx);

/*!
 * @brief Checks if sc-memory is initialized.
 *
 * @return Returns SC_TRUE if sc-memory is initialized; otherwise, returns SC_FALSE.
 *
 * @note Use this function to check whether sc-memory has been successfully initialized.
 */
_SC_EXTERN sc_bool sc_memory_is_initialized();

/*!
 * @brief Checks if the specified sc-addr represents a valid sc-element.
 *
 * This function checks if the specified sc-addr represents a valid sc-element
 * in the given sc-memory context.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param addr The sc-addr to be checked.
 *
 * @return Returns SC_TRUE if the specified sc-addr represents a valid sc-element,
 *         and SC_FALSE otherwise.
 *
 * @note This function is thread-safe.
 */
_SC_EXTERN sc_bool sc_memory_is_element(sc_memory_context const * ctx, sc_addr addr);

/*!
 * @brief Checks if the specified sc-addr represents a valid sc-element.
 *
 * This function checks if the specified sc-addr represents a valid sc-element
 * in the given sc-memory context.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param addr The sc-addr to be checked.
 * @param result Pointer to a variable that will store the result of the operation.
 *
 * @return Returns SC_TRUE if the specified sc-addr represents a valid sc-element,
 *         and SC_FALSE otherwise.
 *
 * @note This function is thread-safe.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHORIZED The specified sc-memory context is not authorized.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS The specified sc-memory context does not have read
 * permissions.
 */
_SC_EXTERN sc_bool sc_memory_is_element_ext(sc_memory_context const * ctx, sc_addr addr, sc_result * result);

/*!
 * @brief Frees the memory occupied by a sc-element and all connected elements.
 *
 * This function frees the memory occupied by a sc-element identified by the provided
 * sc-addr, along with all the connected elements (incoming/outgoing sc-connectors) related to it.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param addr The sc-addr of the sc-element to be freed.
 *
 * @return Returns SC_RESULT_OK if the operation executed successfully.
 *
 * @note The caller is responsible for handling any errors indicated by the result value.
 * @note This function is thread-safe.
 *
 * Possible values for the result:
 * @retval SC_RESULT_OK The function executed successfully.
 * @retval SC_RESULT_ERROR_ADDR_IS_NOT_VALID The specified sc-addr is not valid.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHORIZED The specified sc-memory context is not authorized.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_ERASE_PERMISSIONS The specified sc-memory context does not have
 * erase permissions.
 */
_SC_EXTERN sc_result sc_memory_element_free(sc_memory_context * ctx, sc_addr addr);

/*!
 * @brief Generates a new sc-node with the specified type.
 *
 * This function creates a new sc-node with the specified type and returns
 * its sc-addr. The result of the operation is not explicitly returned.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param type Type of the new sc-node.
 *
 * @return Returns the sc-addr of the created sc-node or an empty sc-addr if the sc-node
 *         wasn't created successfully.
 *
 * @note This function is a convenience wrapper around `sc_memory_node_new_ext`, where
 *       the result of the operation is not explicitly returned. Use this function
 *       when the result is not needed.
 * @note This function is thread-safe.
 */
_SC_EXTERN sc_addr sc_memory_node_new(sc_memory_context const * ctx, sc_type type);

/*!
 * @brief Generates a new sc-node with the specified type.
 *
 * This function creates a new sc-node with the specified type and returns
 * its sc-addr. The result of the operation is stored in the provided pointer
 * to sc-result.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param type Type of the new sc-node.
 * @param result Pointer to a variable that will store the result of the operation.
 *
 * @return Returns the sc-addr of the created sc-node or an empty sc-addr if the sc-node
 *         wasn't created successfully.
 *
 * @note The caller is responsible for handling any errors indicated by the result value.
 * @note This function is thread-safe.
 *
 * Possible values for the result:
 * @retval SC_RESULT_OK The function executed successfully.
 * @retval SC_RESULT_ERROR_ELEMENT_IS_NOT_NODE The specified sc-type is not valid for a sc-node.
 * @retval SC_RESULT_ERROR_FULL_MEMORY Unable to allocate memory for the new sc-node.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHORIZED The specified sc-memory context is not authorized.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_WRITE_PERMISSIONS The specified sc-memory context does not have
 * write permissions.
 */
_SC_EXTERN sc_addr sc_memory_node_new_ext(sc_memory_context const * ctx, sc_type type, sc_result * result);

/*!
 * @brief Generates a new sc-link with the specified type.
 *
 * This function creates a new sc-link with the specified type and returns
 * its sc-addr. The result of the operation is not explicitly returned.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 *
 * @return Returns the sc-addr of the created sc-link or an empty sc-addr if the sc-link
 *         wasn't created successfully.
 *
 * @note This function is a convenience wrapper around `sc_memory_link_new_ext`, where
 *       the result of the operation is not explicitly returned. Use this function
 *       when the result is not needed.
 * @note This function is thread-safe.
 */
_SC_EXTERN sc_addr sc_memory_link_new(sc_memory_context const * ctx);

/*!
 * @brief Generates a new sc-link with the specified type.
 *
 * This function creates a new sc-link with the specified type and returns
 * its sc-addr. The result of the operation is not explicitly returned.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param type Type of the new sc-link.
 *
 * @return Returns the sc-addr of the created sc-link or an empty sc-addr if the sc-link
 *         wasn't created successfully.
 *
 * @note This function is a convenience wrapper around `sc_memory_link_new_ext`, where
 *       the result of the operation is not explicitly returned. Use this function
 *       when the result is not needed.
 * @note This function is thread-safe.
 */
_SC_EXTERN sc_addr sc_memory_link_new2(sc_memory_context const * ctx, sc_type type);

/*!
 * @brief Generates a new sc-link with the specified type.
 *
 * This function creates a new sc-link with the specified type and returns
 * its sc-addr. The result of the operation is stored in the provided pointer
 * to sc-result.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param type Type of the new sc-link.
 * @param result Pointer to a variable that will store the result of the operation.
 *
 * @return Returns the sc-addr of the created sc-link or an empty sc-addr if the sc-link
 *         wasn't created successfully.
 *
 * @note The caller is responsible for handling any errors indicated by the result value.
 * @note This function is thread-safe.
 *
 * Possible values for the result:
 * @retval SC_RESULT_OK The function executed successfully.
 * @retval SC_RESULT_ERROR_ELEMENT_IS_NOT_LINK The specified sc-type is not valid for a sc-link.
 * @retval SC_RESULT_ERROR_FULL_MEMORY Unable to allocate memory for the new sc-link.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHORIZED The specified sc-memory context is not authorized.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_WRITE_PERMISSIONS The specified sc-memory context does not have
 * write permissions.
 */
_SC_EXTERN sc_addr sc_memory_link_new_ext(sc_memory_context const * ctx, sc_type type, sc_result * result);

/*!
 * @brief Generates a new sc-connector between two sc-elements with the specified type.
 *
 * This function creates a new sc-connector with the specified type between the
 * specified begin and end sc-elements, and returns its sc-addr. The result
 * of the operation is not explicitly returned.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param type Type of the new sc-arc.
 * @param beg_addr The sc-addr of the begin sc-element.
 * @param end_addr The sc-addr of the end sc-element.
 *
 * @return Returns the sc-addr of the created sc-connector or an empty sc-addr if the
 *         sc-connector wasn't created successfully.
 *
 * @note This function is a convenience wrapper around ` sc_memory_arc_new_ext`,
 *       where the result of the operation is not explicitly returned. Use this
 *       function when the result is not needed.
 * @note This function is thread-safe.
 */
_SC_EXTERN sc_addr sc_memory_arc_new(sc_memory_context const * ctx, sc_type type, sc_addr beg, sc_addr end);

/*!
 * @brief Generates a new sc-connector with the specified type.
 *
 * This function creates a new sc-connector with the specified type, connecting the given
 * begin and end sc-elements. The type must be an arc type (e.g., sc_type_arc_common,
 * sc_type_arc_pos_const_perm), and the begin and end sc-elements must be valid sc-addrs.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param type Type of the new sc-connector.
 * @param beg_addr sc-addr of the begin sc-element.
 * @param end_addr sc-addr of the end sc-element.
 * @param result Pointer to a variable that will store the result of the operation.
 *               It can be NULL if the result is not needed.
 *
 * @return Returns the sc-addr of the created sc-connector if successful, or SC_ADDR_EMPTY if
 *         the sc-connector creation fails.
 *
 * @note The caller is responsible for handling any errors indicated by the result value.
 * @note This function is thread-safe.
 *
 * @retval SC_ADDR_EMPTY The sc-connector creation failed, and the returned sc-addr is empty.
 * @retval Valid sc-addr The sc-connector was successfully created, and the returned sc-addr
 *                        is the identifier of the newly created sc-connector.
 *
 * Possible values for the `result` parameter:
 * @retval SC_RESULT_OK The function executed successfully.
 * @retval SC_RESULT_ERROR_ELEMENT_IS_NOT_CONNECTOR The specified type is not a valid sc-connector type.
 * @retval SC_RESULT_ERROR_ADDR_IS_NOT_VALID Either the begin or end sc-addr is not valid.
 * @retval SC_RESULT_ERROR_FULL_MEMORY Memory allocation for the new sc-connector failed.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHORIZED The specified sc-memory context is not authorized.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_WRITE_PERMISSIONS The specified sc-memory context does not have
 * write permissions.
 */
_SC_EXTERN sc_addr sc_memory_arc_new_ext(
    sc_memory_context const * ctx,
    sc_type type,
    sc_addr beg_addr,
    sc_addr end_addr,
    sc_result * result);

/*!
 * @brief Retrieves the count of output connectors for the specified sc-element.
 *
 * This function retrieves the count of output connectors for the sc-element with the specified
 * sc-addr. The result is stored in the provided pointer to sc-uint32.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param addr The sc-addr of the sc-element for which to retrieve the output connectors count.
 * @param result Pointer to a variable that will store the result of the operation.
 *               It can be NULL if the result is not needed.
 *
 * @return Returns the count of output connectors for the sc-element. If an error occurs,
 *         the function returns 0, and the result value is set accordingly.
 *
 * @note The caller is responsible for handling any errors indicated by the result value.
 * @note This function is thread-safe.
 *
 * @retval sc_uint32 The count of output connectors for the specified sc-element.
 *
 * Possible values for the `result` parameter:
 * @retval SC_RESULT_OK The function executed successfully.
 * @retval SC_RESULT_ERROR_ADDR_IS_NOT_VALID The specified sc-addr is not valid.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHORIZED The specified sc-memory context is not authorized.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS The specified sc-memory context does not have read
 * permissions.
 */
_SC_EXTERN sc_uint32
sc_memory_get_element_outgoing_arcs_count(sc_memory_context const * ctx, sc_addr addr, sc_result * result);

/*!
 * @brief Retrieves the count of input sc-connectors for the specified sc-element.
 *
 * This function retrieves the count of input sc-connectors for the sc-element with the specified
 * sc-addr. The result is stored in the provided pointer to sc-uint32.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param addr The sc-addr of the sc-element for which to retrieve the input sc-connectors count.
 * @param result Pointer to a variable that will store the result of the operation.
 *               It can be NULL if the result is not needed.
 *
 * @return Returns the count of input sc-connectors for the sc-element. If an error occurs,
 *         the function returns 0, and the result value is set accordingly.
 *
 * @note The caller is responsible for handling any errors indicated by the result value.
 * @note This function is thread-safe.
 *
 * @retval sc_uint32 The count of input sc-connectors for the specified sc-element.
 *
 * Possible values for the `result` parameter:
 * @retval SC_RESULT_OK The function executed successfully.
 * @retval SC_RESULT_ERROR_ADDR_IS_NOT_VALID The specified sc-addr is not valid.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHORIZED The specified sc-memory context is not authorized.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS The specified sc-memory context does not have read
 * permissions.
 */
_SC_EXTERN sc_uint32
sc_memory_get_element_incoming_arcs_count(sc_memory_context const * ctx, sc_addr addr, sc_result * result);

/*!
 * @brief Retrieves the type of the specified sc-element.
 *
 * This function retrieves the type of the sc-element with the specified sc-addr.
 * The result is stored in the provided pointer to sc-type.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param addr The sc-addr of the sc-element for which to retrieve the type.
 * @param result Pointer to a variable that will store the result of the operation.
 *             It can be NULL if the result is not needed.
 *
 * @return Returns the result of the operation. If successful, the function returns
 *         SC_RESULT_OK, and the type value is set accordingly. If an error occurs,
 *         the function returns an error code, and the type value is not valid.
 *
 * @note The caller is responsible for handling any errors indicated by the result value.
 * @note This function is thread-safe.
 *
 * Possible values for the `result` parameter:
 * @retval SC_RESULT_OK The function executed successfully.
 * @retval SC_RESULT_ERROR_ADDR_IS_NOT_VALID The specified sc-addr is not valid.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHORIZED The specified sc-memory context is not authorized.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS The specified sc-memory context does not have read
 * permissions.
 */
_SC_EXTERN sc_result sc_memory_get_element_type(sc_memory_context const * ctx, sc_addr addr, sc_type * result);

/*!
 * @brief Changes the subtype of the specified sc-element.
 *
 * This function changes the subtype of the sc-element with the specified sc-addr
 * to the specified type. The provided type should have the same base element type
 * (e.g., sc_type_node, sc_type_link) as the current type of the sc-element.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param addr The sc-addr of the sc-element for which to change the subtype.
 * @param type The new subtype to assign to the sc-element.
 *
 * @return Returns the result of the operation. If successful, the function returns
 *         SC_RESULT_OK. If an error occurs, the function returns an error code.
 *
 * @note The caller is responsible for handling any errors indicated by the result value.
 * @note This function is thread-safe.
 *
 * Possible values for the result:
 * @retval SC_RESULT_OK The function executed successfully.
 * @retval SC_RESULT_ERROR_ADDR_IS_NOT_VALID The specified sc-addr is not valid.
 * @retval SC_RESULT_ERROR_INVALID_PARAMS The provided type is not a valid subtype for the sc-element.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHORIZED The specified sc-memory context is not authorized.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_WRITE_PERMISSIONS The specified sc-memory context does not have
 * write permissions.
 */
_SC_EXTERN sc_result sc_memory_change_element_subtype(sc_memory_context const * ctx, sc_addr addr, sc_type type);

/*!
 * @brief Retrieves the begin sc-addr of the specified sc-arc.
 *
 * This function retrieves the begin sc-addr of the specified sc-connector with the provided
 * sc-addr. The result is stored in the provided pointer to sc-addr.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param addr The sc-addr of the sc-connector for which to retrieve the begin sc-addr.
 * @param begin_addr Pointer to a variable that will store the result (begin sc-addr)
 *                          of the operation. It can be NULL if the result is not needed.
 *
 * @return Returns the result of the operation. If successful, the function returns
 *         SC_RESULT_OK, and the begin sc-addr value is set accordingly. If an error occurs,
 *         the function returns an error code, and the begin sc-addr value is not valid.
 *
 * @note The caller is responsible for handling any errors indicated by the result value.
 * @note This function is thread-safe.
 *
 * Possible values for the result:
 * @retval SC_RESULT_OK The function executed successfully.
 * @retval SC_RESULT_ERROR_ADDR_IS_NOT_VALID The specified sc-addr is not valid.
 * @retval SC_RESULT_ERROR_ELEMENT_IS_NOT_CONNECTOR The specified sc-addr does not represent a valid sc-arc.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHORIZED The specified sc-memory context is not authorized.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS The specified sc-memory context does not have read
 * permissions.
 */
_SC_EXTERN sc_result sc_memory_get_arc_begin(sc_memory_context const * ctx, sc_addr addr, sc_addr * begin_addr);

/*!
 * @brief Retrieves the end sc-addr of the specified sc-arc.
 *
 * This function retrieves the end sc-addr of the specified sc-connector with the provided
 * sc-addr. The result is stored in the provided pointer to sc-addr.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param addr The sc-addr of the sc-connector for which to retrieve the end sc-addr.
 * @param end_addr Pointer to a variable that will store the result (end sc-addr)
 *                        of the operation. It can be NULL if the result is not needed.
 *
 * @return Returns the result of the operation. If successful, the function returns
 *         SC_RESULT_OK, and the end sc-addr value is set accordingly. If an error occurs,
 *         the function returns an error code, and the end sc-addr value is not valid.
 *
 * @note The caller is responsible for handling any errors indicated by the result value.
 * @note This function is thread-safe.
 *
 * Possible values for the result:
 * @retval SC_RESULT_OK The function executed successfully.
 * @retval SC_RESULT_ERROR_ADDR_IS_NOT_VALID The specified sc-addr is not valid.
 * @retval SC_RESULT_ERROR_ELEMENT_IS_NOT_CONNECTOR The specified sc-addr does not represent a valid sc-arc.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHORIZED The specified sc-memory context is not authorized.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS The specified sc-memory context does not have read
 * permissions.
 */
_SC_EXTERN sc_result sc_memory_get_arc_end(sc_memory_context const * ctx, sc_addr addr, sc_addr * end_addr);

/*!
 * @brief Retrieves the begin and end sc-addrs of the specified sc-arc.
 *
 * This function retrieves both the begin and end sc-addrs of the specified sc-arc
 * with the provided sc-addr. The results are stored in the provided pointers to sc-addr.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param addr The sc-addr of the sc-connector for which to retrieve the begin and end sc-addrs.
 * @param begin_addr Pointer to a variable that will store the result (begin sc-addr)
 *                          of the operation. It can be NULL if the result is not needed.
 * @param end_addr Pointer to a variable that will store the result (end sc-addr)
 *                        of the operation. It can be NULL if the result is not needed.
 *
 * @return Returns the result of the operation. If successful, the function returns
 *         SC_RESULT_OK, and the begin and end sc-addrs are set accordingly.
 *         If an error occurs, the function returns an error code, and the sc-addrs are not valid.
 *
 * @note The caller is responsible for handling any errors indicated by the result value.
 * @note This function is thread-safe.
 *
 * Possible values for the result:
 * @retval SC_RESULT_OK The function executed successfully.
 * @retval SC_RESULT_ERROR_ADDR_IS_NOT_VALID The specified sc-addr is not valid.
 * @retval SC_RESULT_ERROR_ELEMENT_IS_NOT_CONNECTOR The specified sc-addr does not represent a valid sc-arc.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHORIZED The specified sc-memory context is not authorized.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS The specified sc-memory context does not have read
 * permissions.
 */
_SC_EXTERN sc_result
sc_memory_get_arc_info(sc_memory_context const * ctx, sc_addr addr, sc_addr * begin_addr, sc_addr * end_addr);

/*!
 * @brief Sets the content of the specified sc-link.
 *
 * This function sets the content of the sc-link with the specified sc-addr using the
 * data from the provided stream. The stream is expected to contain the content data
 * to be associated with the sc-link.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param addr The sc-addr of the sc-link for which to set the content.
 * @param stream The stream containing the content data to be associated with the sc-link.
 *
 * @return Returns the result of the operation. If successful, the function returns
 *         SC_RESULT_OK. If an error occurs, the function returns an error code.
 *
 * @note The caller is responsible for handling any errors indicated by the result value.
 * @note This function is thread-safe.
 *
 * Possible values for the result:
 * @retval SC_RESULT_OK The function executed successfully.
 * @retval SC_RESULT_ERROR_ADDR_IS_NOT_VALID The specified sc-addr is not valid.
 * @retval SC_RESULT_ERROR_ELEMENT_IS_NOT_LINK The specified sc-addr does not represent a valid sc-link.
 * @retval SC_RESULT_ERROR_STREAM_IO Error occurred while processing the stream.
 * @retval SC_RESULT_ERROR_FILE_MEMORY_IO Error occurred during file/memory operations.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHORIZED The specified sc-memory context is not authorized.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_WRITE_PERMISSIONS The specified sc-memory context does not have
 * write permissions.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_ERASE_PERMISSIONS The specified sc-memory context does not have
 * erase permissions.
 */
_SC_EXTERN sc_result sc_memory_set_link_content(sc_memory_context const * ctx, sc_addr addr, sc_stream const * stream);

/*!
 * @brief Sets the content of the specified sc-link.
 *
 * This function sets the content of the sc-link with the specified sc-addr using the
 * data from the provided stream. The stream is expected to contain the content data
 * to be associated with the sc-link.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param addr The sc-addr of the sc-link for which to set the content.
 * @param stream The stream containing the content data to be associated with the sc-link.
 * @param is_searchable_string A boolean indicating whether the content should be treated
 *                             as a searchable string. If SC_TRUE, the content will be processed
 *                             as a searchable string; otherwise, it will be treated as raw data.
 *
 * @return Returns the result of the operation. If successful, the function returns
 *         SC_RESULT_OK. If an error occurs, the function returns an error code.
 *
 * @note The caller is responsible for handling any errors indicated by the result value.
 * @note This function is thread-safe.
 *
 * Possible values for the result:
 * @retval SC_RESULT_OK The function executed successfully.
 * @retval SC_RESULT_ERROR_ADDR_IS_NOT_VALID The specified sc-addr is not valid.
 * @retval SC_RESULT_ERROR_ELEMENT_IS_NOT_LINK The specified sc-addr does not represent a valid sc-link.
 * @retval SC_RESULT_ERROR_STREAM_IO Error occurred while processing the stream.
 * @retval SC_RESULT_ERROR_FILE_MEMORY_IO Error occurred during file/memory operations.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHORIZED The specified sc-memory context is not authorized.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_WRITE_PERMISSIONS The specified sc-memory context does not have
 * write permissions.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_ERASE_PERMISSIONS The specified sc-memory context does not have
 * erase permissions.
 */
_SC_EXTERN sc_result sc_memory_set_link_content_ext(
    sc_memory_context const * ctx,
    sc_addr addr,
    sc_stream const * stream,
    sc_bool is_searchable_string);

/*!
 * @brief Retrieves the content of the specified sc-link as a stream.
 *
 * This function retrieves the content of the sc-link with the specified sc-addr
 * as a stream. The stream contains the content data associated with the sc-link.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param addr The sc-addr of the sc-link for which to retrieve the content.
 * @param stream Pointer to a variable that will store the result (stream)
 *               of the operation. It can be NULL if the result is not needed.
 *
 * @return Returns the result of the operation. If successful, the function returns
 *         SC_RESULT_OK, and the stream value is set accordingly. If an error occurs,
 *         the function returns an error code, and the stream value is not valid.
 *
 * @note The caller is responsible for handling any errors indicated by the result value.
 * @note This function is thread-safe.
 *
 * Possible values for the result:
 * @retval SC_RESULT_OK The function executed successfully.
 * @retval SC_RESULT_ERROR_ADDR_IS_NOT_VALID The specified sc-addr is not valid.
 * @retval SC_RESULT_ERROR_ELEMENT_IS_NOT_LINK The specified sc-addr does not represent a valid sc-link.
 * @retval SC_RESULT_ERROR_STREAM_IO Error occurred while processing the stream.
 * @retval SC_RESULT_ERROR_FILE_MEMORY_IO Error occurred during file/memory operations.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHORIZED The specified sc-memory context is not authorized.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS The specified sc-memory context does not have read
 * permissions.
 */
_SC_EXTERN sc_result sc_memory_get_link_content(sc_memory_context const * ctx, sc_addr addr, sc_stream ** stream);

/*!
 * @brief Finds sc-links with content matching the specified string.
 *
 * This function searches for sc-links with content that matches the provided
 * string. The result is stored in the provided pointer to sc-list, containing
 * the hash values of the sc-links that match the search criteria.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param stream The stream containing the search string.
 * @param result_hashes The list containing the hash values of sc-links with content
 *                      matching the specified string.
 *
 * @note The caller is responsible for handling any errors indicated by the result value.
 * @note This function is thread-safe.
 *
 * @return Returns an sc_result indicating the success or failure of the operation.
 * Possible result values:
 * @retval SC_RESULT_OK: The operation was successful. Sc-links was found by content.
 * @retval SC_RESULT_NO_STRING: The operation was unsuccessful. Sc-links wasn't found by content.
 * @retval SC_RESULT_ERROR_STREAM_IO: An error occurred during stream I/O.
 * @retval SC_RESULT_ERROR_FILE_MEMORY_IO: An error occurred during file memory I/O.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHORIZED The specified sc-memory context is not authorized.
 */
_SC_EXTERN sc_result sc_memory_find_links_with_content_string(
    sc_memory_context const * ctx,
    sc_stream const * stream,
    sc_list ** result_hashes);

/*!
 * @brief Finds sc-links with content containing the specified substring.
 *
 * This function searches for sc-links with content that contains the specified
 * substring. The result is stored in the provided pointer to sc-list, containing
 * the hash values of the sc-links that match the search criteria.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param stream The stream containing the search substring.
 * @param result_addrs The list containing the hash values of sc-links with content
 *                      containing the specified substring.
 * @param max_length_to_search_as_prefix The maximum length of the substring to search
 *                                       as a prefix. Set to 0 for a standard substring search.
 * @note The caller is responsible for handling any errors indicated by the result value.
 * @note This function is thread-safe.
 *
 * @return Returns an sc_result indicating the success or failure of the operation.
 * Possible result values:
 * @retval SC_RESULT_OK: The operation was successful. Sc-links was found by content substring.
 * @retval SC_RESULT_NO_STRING: The operation was unsuccessful. Sc-links wasn't found by content substring.
 * @retval SC_RESULT_ERROR_STREAM_IO: An error occurred during stream I/O.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHORIZED The specified sc-memory context is not authorized.
 */
_SC_EXTERN sc_result sc_memory_find_links_by_content_substring(
    sc_memory_context const * ctx,
    sc_stream const * stream,
    sc_uint32 max_length_to_search_as_prefix,
    sc_list ** result_hashes);

/*!
 * @brief Finds sc-link contents containing the specified substring.
 *
 * This function searches for sc-link contents that contain the specified
 * substring. The result is stored in the provided pointer to sc-list,
 * containing the content strings of the sc-links that match the search criteria.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param stream The stream containing the search substring.
 * @param result_strings The list containing the content strings of sc-links with content
 *                      containing the specified substring.
 * @param max_length_to_search_as_prefix The maximum length of the substring to search
 *                                       as a prefix. Set to 0 for a standard substring search.
 * @note The caller is responsible for handling any errors indicated by the result value.
 * @note This function is thread-safe.
 *
 * @return Returns an sc_result indicating the success or failure of the operation.
 * Possible result values:
 * @retval SC_RESULT_OK: The operation was successful. Sc-links contents was found by content substring.
 * @retval SC_RESULT_NO_STRING: The operation was unsuccessful. Sc-links contents wasn't found by content substring.
 * @retval SC_RESULT_ERROR_STREAM_IO: An error occurred during stream I/O.
 * @retval SC_RESULT_ERROR_FILE_MEMORY_IO: An error occurred during file memory I/O.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHORIZED The specified sc-memory context is not authorized.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS The specified sc-memory context does not have read
 * permissions.
 */
_SC_EXTERN sc_result sc_memory_find_links_contents_by_content_substring(
    sc_memory_context const * ctx,
    sc_stream const * stream,
    sc_uint32 max_length_to_search_as_prefix,
    sc_list ** result_strings);

/*!
 * @brief Retrieves statistics for sc-storage elements.
 *
 * This function retrieves statistics for SC-storage elements, including the count
 * of various types of elements (nodes, links, arcs) and their total size in bytes.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param stat Pointer to the `sc_stat` structure where the statistics will be stored.
 *             It should be pre-allocated by the caller.
 *
 * @return Returns the result of the operation. If successful, it returns SC_RESULT_OK.
 *         If an error occurs during the retrieval of statistics, an appropriate error code is returned.
 *
 *
 * @note The caller is responsible for allocating and passing a valid `sc_stat` structure
 *       to store the retrieved statistics.
 * @note This function is thread-safe.
 *
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHORIZED The specified sc-memory context is not authorized.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS The specified sc-memory context does not have read
 * permissions.
 */
_SC_EXTERN sc_result sc_memory_stat(sc_memory_context const * ctx, sc_stat * stat);

/*!
 * @brief Saves the current state of the sc-storage to persistent storage.
 *
 * This function triggers the saving of the current state of the sc-storage
 * to persistent storage. It utilizes the file system memory to store the data.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 *
 * @return Returns the result of the operation. If successful, it returns SC_RESULT_OK.
 *         If an error occurs during the saving process, an appropriate error code is returned.
 *
 * @note The caller is responsible for handling any errors indicated by the result value.
 * @note This function is thread-safe.
 *
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHORIZED The specified sc-memory context is not authorized.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_WRITE_PERMISSIONS The specified sc-memory context does not have
 * write permissions.
 */
_SC_EXTERN sc_result sc_memory_save(sc_memory_context const * ctx);

#endif
