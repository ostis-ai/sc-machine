/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_memory_private_h_
#define _sc_memory_private_h_

#include "sc-store/sc-base/sc_message.h"

#define SC_MEMORY_PREFIX "[sc-memory] "
#define sc_memory_info(...) sc_message(SC_MEMORY_PREFIX __VA_ARGS__)
#define sc_memory_warning(...) sc_warning(SC_MEMORY_PREFIX __VA_ARGS__)
#define sc_memory_error(...) sc_critical(SC_MEMORY_PREFIX __VA_ARGS__)

/*! Finds sc-links in the sc-memory that have content matching a string from the provided stream.
 * @param ctx Pointer to the sc-memory context.
 * @param stream Pointer to the stream containing the string to search for.
 * @param link_handler Pointer to object with callbacks for handling sc-links.
 * @return Returns SC_RESULT_OK if the operation was successful; otherwise, returns an error code.
 */
_SC_EXTERN sc_result sc_memory_find_links_with_content_string_ext(
    sc_memory_context const * ctx,
    sc_stream const * stream,
    sc_link_handler * link_handler);

/*! Finds sc-links in the sc-memory that have content containing a substring from the provided stream.
 * @param ctx Pointer to the sc-memory context.
 * @param stream Pointer to the stream containing the substring to search for.
 * @param max_length_to_search_as_prefix Maximum length to consider the search as a prefix search.
 * @param link_handler Pointer to object with callbacks for handling sc-links.
 * @return Returns SC_RESULT_OK if the operation was successful; otherwise, returns an error code.
 */
_SC_EXTERN sc_result sc_memory_find_links_by_content_substring_ext(
    sc_memory_context const * ctx,
    sc_stream const * stream,
    sc_uint32 max_length_to_search_as_prefix,
    sc_link_handler * link_handler);

/*! Finds sc-links in the sc-memory that have content containing a substring from the provided stream.
 * @param ctx Pointer to the sc-memory context.
 * @param stream Pointer to the stream containing the substring to search for.
 * @param max_length_to_search_as_prefix Maximum length to consider the search as a prefix search.
 * @param link_handler Pointer to object with callbacks for handling sc-links.
 * @return Returns SC_RESULT_OK if the operation was successful; otherwise, returns an error code.
 */
_SC_EXTERN sc_result sc_memory_find_links_contents_by_content_substring_ext(
    sc_memory_context const * ctx,
    sc_stream const * stream,
    sc_uint32 max_length_to_search_as_prefix,
    sc_link_handler * link_handler);

#endif
