/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

/*!
 * @file sc_storage.h
 *
 * @brief This file contains the core C API for managing the sc-memory.
 *
 * This API include core methods for managing sc-memory:
 * - methods for initialize, save and shutdown sc-memory,
 * - methods for create elements in sc-memory,
 * - methods for get basic information about sc-elements,
 * - method for delete elements from sc-memory,
 * - methods for set/get content for/from sc-links,
 * - methods for search sc-links by contents,
 */

#ifndef _sc_storage_h_
#define _sc_storage_h_

#include "sc-core/sc_memory_params.h"

#include "sc-core/sc_stream.h"
#include "sc-core/sc-container/sc_list.h"

typedef struct _sc_storage sc_storage;

/*!
 * @brief Initializes the sc-storage with the provided parameters.
 *
 * This function initializes the sc-storage based on the specified parameters.
 * It allocates memory for the storage structure, sets up necessary data structures,
 * and optionally loads the state from persistent storage.
 *
 * @param params A pointer to the sc-memory parameters used to configure the sc-storage.
 *
 * @return Returns SC_RESULT_OK if the initialization is successful, and an error code otherwise.
 *
 * @note The caller is responsible for handling any errors indicated by the result value.
 *
 * Possible values for the result:
 * @retval SC_RESULT_OK The function executed successfully.
 * @retval SC_RESULT_ERROR An error occurred during initialization.
 *
 * @see sc_storage_shutdown
 */
sc_result sc_storage_initialize(sc_memory_params const * params);

/*!
 * @brief Shuts down the sc-storage.
 *
 * This function shuts down the sc-storage. If the save_state parameter is set to SC_TRUE,
 * it saves the current state to persistent storage before shutting down. The function releases
 * all allocated resources and stops the background threads associated with the storage.
 *
 * @param save_state Specifies whether to save the current state to persistent storage before shutting down.
 *
 * @return Returns SC_RESULT_OK if the shutdown is successful, and an error code otherwise.
 *
 * @note The caller is responsible for handling any errors indicated by the result value.
 *
 * Possible values for the result:
 * @retval SC_RESULT_OK The function executed successfully.
 * @retval SC_RESULT_ERROR An error occurred during shutdown.
 *
 * @see sc_storage_initialize
 */
sc_result sc_storage_shutdown(sc_bool save_state);

//! Check if storage initialized
sc_bool sc_storage_is_initialized();

/*!
 * @brief Checks if the specified sc-addr represents a valid sc-element.
 *
 * This function checks if the specified sc-addr represents a valid sc-element
 * in the given sc-memory context.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param addr A sc-address to be checked.
 *
 * @return Returns SC_TRUE if the specified sc-addr represents a valid sc-element,
 *         and SC_FALSE otherwise.
 *
 * @note This function is thread-safe.
 */
sc_bool sc_storage_is_element(sc_memory_context const * ctx, sc_addr addr);

void sc_storage_start_new_process();

void sc_storage_end_new_process();

/*!
 * @brief Erases the memory occupied by a sc-element and all connected sc-elements.
 *
 * This function erases the memory occupied by a sc-element identified by the provided
 * sc-address, along with all the connected elements (incoming/outgoing sc-connectors) related to it.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param addr A sc-address of the sc-element to be freed.
 *
 * @return Returns SC_RESULT_OK if the operation executed successfully.
 *
 * @note The caller is responsible for handling any errors indicated by the result value.
 * @note This function is thread-safe.
 *
 * Possible values for the result:
 * @retval SC_RESULT_OK The function executed successfully.
 * @retval SC_RESULT_ERROR_ELEMENT_IS_NOT_CONNECTOR The specified sc-addr does not represent a sc-connector.
 * @retval SC_RESULT_ERROR_ADDR_IS_NOT_VALID The specified sc-addr is not valid.
 */
sc_result sc_storage_element_erase(sc_memory_context const * ctx, sc_addr addr);

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
 * @note This function is a convenience wrapper around `sc_storage_node_new_ext`, where
 *       the result of the operation is not explicitly returned. Use this function
 *       when the result is not needed.
 * @note This function is thread-safe.
 */
sc_addr sc_storage_node_new(sc_memory_context const * ctx, sc_type type);

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
 */
sc_addr sc_storage_node_new_ext(sc_memory_context const * ctx, sc_type type, sc_result * result);

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
 * @note This function is a convenience wrapper around `sc_storage_link_new_ext`, where
 *       the result of the operation is not explicitly returned. Use this function
 *       when the result is not needed.
 * @note This function is thread-safe.
 */
sc_addr sc_storage_link_new(sc_memory_context const * ctx, sc_type type);

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
 */
sc_addr sc_storage_link_new_ext(sc_memory_context const * ctx, sc_type type, sc_result * result);

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
 * @note This function is a convenience wrapper around `sc_storage_arc_new_ext`,
 *       where the result of the operation is not explicitly returned. Use this
 *       function when the result is not needed.
 * @note This function is thread-safe.
 */
sc_addr sc_storage_arc_new(sc_memory_context const * ctx, sc_type type, sc_addr beg, sc_addr end);

/*!
 * @brief Generates a new sc-connector with the specified type.
 *
 * This function creates a new sc-connector with the specified type, connecting the given
 * begin and end sc-elements. The type must be an arc type (e.g., sc_type_common_arc,
 * sc_type_const_perm_pos_arc), and the begin and end sc-elements must be valid sc-addrs.
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
 */
sc_addr sc_storage_arc_new_ext(
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
 *
 * @retval sc_uint32 The count of output connectors for the specified sc-element.
 *
 * Possible values for the `result` parameter:
 * @retval SC_RESULT_OK The function executed successfully.
 * @retval SC_RESULT_ERROR_ADDR_IS_NOT_VALID The specified sc-addr is not valid.
 */
sc_uint32 sc_storage_get_element_outgoing_arcs_count(sc_memory_context const * ctx, sc_addr addr, sc_result * result);

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
 */
sc_uint32 sc_storage_get_element_incoming_arcs_count(sc_memory_context const * ctx, sc_addr addr, sc_result * result);

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
 *
 * @retval Valid sc-type The type of the specified sc-element.
 */
sc_result sc_storage_get_element_type(sc_memory_context const * ctx, sc_addr addr, sc_type * result);

/*!
 * @brief Checks if a given sc_type can be extended to a new sc_type.
 *
 * This function evaluates whether the original type can be transformed into the new type without losing any semantic or
 * syntactic information. It considers various aspects of the types, including their syntactic subtypes, constancy, and
 * specific semantics like node links, nodes, and connectors.
 *
 * @param type The original sc_type to be evaluated for extendability.
 * @param new_type The new sc_type that is being checked against the original type.
 *
 * @return SC_TRUE if the new_type is compatible with type and can be used to extend it, SC_FALSE otherwise.
 *
 * The function performs the following checks:
 * - **Syntactic Type Check**: Compares the syntactic subtypes of both types.
 * - **Constancy Check**: Compares the constancy subtypes of both types.
 * - **Node Link Check**: If the original type is a node link, it verifies that the new type is also a node link and
 * checks their respective link subtypes.
 * - **Node Check**: If the original type is a node, it verifies that the new type is also a node and checks their
 * respective node subtypes.
 * - **Connector Check**: If the original type is a connector, it ensures that the new type is also a connector and
 * checks for compatibility in actuality, permanency, and positivity subtypes if specified connector is membership
 * sc-arc.
 */
sc_bool sc_storage_is_type_extendable_to(sc_type type, sc_type new_type);

/*!
 * @brief Changes the subtype of the specified sc-element.
 *
 * This function changes the subtype of the sc-element with the specified sc-addr
 * to the specified type. The provided type should have the same base element type
 * (e.g., sc_type_node, sc_type_membership_arc, sc_type_common_arc, sc_type_common_edge) as the current type of the
 * sc-element.
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
 */
sc_result sc_storage_change_element_subtype(sc_memory_context const * ctx, sc_addr addr, sc_type type);

/*!
 * @brief Retrieves the begin sc-addr of the specified sc-arc.
 *
 * This function retrieves the begin sc-addr of the specified sc-connector with the provided
 * sc-addr. The result is stored in the provided pointer to sc-addr.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param addr The sc-addr of the sc-connector for which to retrieve the begin sc-addr.
 * @param result_begin_addr Pointer to a variable that will store the result (begin sc-addr)
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
 *
 * @retval Valid sc-addr The begin sc-addr of the specified sc-arc.
 */
sc_result sc_storage_get_arc_begin(sc_memory_context const * ctx, sc_addr addr, sc_addr * result_begin_addr);

/*!
 * @brief Retrieves the end sc-addr of the specified sc-arc.
 *
 * This function retrieves the end sc-addr of the specified sc-connector with the provided
 * sc-addr. The result is stored in the provided pointer to sc-addr.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param addr The sc-addr of the sc-connector for which to retrieve the end sc-addr.
 * @param result_end_addr Pointer to a variable that will store the result (end sc-addr)
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
 *
 * @retval Valid sc-addr The end sc-addr of the specified sc-arc.
 */
sc_result sc_storage_get_arc_end(sc_memory_context const * ctx, sc_addr addr, sc_addr * result_end_addr);

/*!
 * @brief Retrieves the begin and end sc-addrs of the specified sc-arc.
 *
 * This function retrieves both the begin and end sc-addrs of the specified sc-arc
 * with the provided sc-addr. The results are stored in the provided pointers to sc-addr.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param addr The sc-addr of the sc-connector for which to retrieve the begin and end sc-addrs.
 * @param result_begin_addr Pointer to a variable that will store the result (begin sc-addr)
 *                          of the operation. It can be NULL if the result is not needed.
 * @param result_end_addr Pointer to a variable that will store the result (end sc-addr)
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
 *
 * @retval Valid sc-addr The begin and end sc-addrs of the specified sc-arc.
 */
sc_result sc_storage_get_arc_info(
    sc_memory_context const * ctx,
    sc_addr addr,
    sc_addr * result_begin_addr,
    sc_addr * result_end_addr);

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
 */
sc_result sc_storage_set_link_content(
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
 *
 * @retval Valid sc-stream The stream containing the content data of the specified sc-link.
 */
sc_result sc_storage_get_link_content(sc_memory_context const * ctx, sc_addr addr, sc_stream ** stream);

/*!
 * @brief Finds sc-links with content matching the specified string.
 *
 * This function searches for sc-links with content that matches the provided
 * string. The result is stored in the provided pointer to sc-list, containing
 * the hash values of the sc-links that match the search criteria.
 *
 * @param ctx A pointer to the sc-memory context that manages the operation.
 * @param stream The stream containing the search string.
 * @param result_addrs The list containing the hash values of sc-links with content
 *                      matching the specified string.
 *
 * @return Returns an sc_result indicating the success or failure of the operation.
 * Possible result values:
 * @retval SC_RESULT_OK: The operation was successful. Sc-links was found by content.
 * @retval SC_RESULT_NO_STRING: The operation was unsuccessful. Sc-links wasn't found by content.
 * @retval SC_RESULT_ERROR_STREAM_IO: An error occurred during stream I/O.
 * @retval SC_RESULT_ERROR_FILE_MEMORY_IO: An error occurred during file memory I/O.
 *
 * @note This function is thread-safe.
 */
sc_result sc_storage_find_links_with_content_string(
    sc_memory_context const * ctx,
    sc_stream const * stream,
    void * data,
    void (*callback)(void * data, sc_addr const link_addr));

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
 *
 * @return Returns an sc_result indicating the success or failure of the operation.
 * Possible result values:
 * @retval SC_RESULT_OK: The operation was successful. Sc-links was found by content substring.
 * @retval SC_RESULT_NO_STRING: The operation was unsuccessful. Sc-links wasn't found by content substring.
 * @retval SC_RESULT_ERROR_STREAM_IO: An error occurred during stream I/O.
 *
 * @note This function is thread-safe.
 */
sc_result sc_storage_find_links_by_content_substring(
    sc_memory_context const * ctx,
    sc_stream const * stream,
    sc_uint32 max_length_to_search_as_prefix,
    void * data,
    void (*callback)(void * data, sc_addr const link_addr));

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
 *
 * @return Returns an sc_result indicating the success or failure of the operation.
 * Possible result values:
 * @retval SC_RESULT_OK: The operation was successful. Sc-links contents was found by content substring.
 * @retval SC_RESULT_NO_STRING: The operation was unsuccessful. Sc-links contents wasn't found by content substring.
 * @retval SC_RESULT_ERROR_STREAM_IO: An error occurred during stream I/O.
 * @retval SC_RESULT_ERROR_FILE_MEMORY_IO: An error occurred during file memory I/O.
 *
 * @note This function is thread-safe.
 */
sc_result sc_storage_find_links_contents_by_content_substring(
    sc_memory_context const * ctx,
    sc_stream const * stream,
    sc_uint32 max_length_to_search_as_prefix,
    void * data,
    void (*callback)(void * data, sc_addr const link_addr, sc_char const * link_content));

/*!
 * @brief Retrieves statistics for sc-storage elements.
 *
 * This function retrieves statistics for SC-storage elements, including the count
 * of various types of elements (nodes, links, arcs) and their total size in bytes.
 *
 * @param stat Pointer to the `sc_stat` structure where the statistics will be stored.
 *             It should be pre-allocated by the caller.
 *
 * @return Returns the result of the operation. If successful, it returns SC_RESULT_OK.
 *         If an error occurs during the retrieval of statistics, an appropriate error code is returned.
 *
 * @note The caller is responsible for allocating and passing a valid `sc_stat` structure
 *       to store the retrieved statistics.
 * @note This function is thread-safe.
 */
sc_result sc_storage_get_elements_stat(sc_stat * stat);

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
 */
sc_result sc_storage_save(sc_memory_context const * ctx);

#endif
