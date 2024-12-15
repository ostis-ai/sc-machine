/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_link_filter_h_
#define _sc_link_filter_h_

#include "sc-core/sc_types.h"

typedef enum
{
  SC_LINK_FILTER_REQUEST_CONTINUE = 0,  // Continue processing links.
  SC_LINK_FILTER_REQUEST_STOP,          // Stop processing links
} sc_link_filter_request;

/*!
 * @struct sc_link_handler
 * @brief A structure with callbacks for handling sc-links.
 *
 * This structure contains function pointers that are used as callbacks for
 * various sc-link operations, allowing users to define custom behavior when
 * sc-links are iterating, or when their contents are iterating.
 */
typedef struct
{
  /*!
   * @brief Callback function to validate a sc-link against a specified condition.
   *
   * This callback function is invoked to determine if a given sc-link meets
   * a specific condition defined by the user. Implementations of this function
   * should contain the logic to evaluate the sc-link based on the provided criteria.
   *
   * @param data Pointer to user-defined data that will be passed to the callback.
   *             This can be used to provide context or additional information needed
   *             for the validation process.
   * @param link_addr A sc-address of the sc-link that is being qualified. This address
   *                  identifies the specific sc-link to be checked against the condition.
   *
   * @return sc_bool Returns true if the sc-link satisfies the specified condition;
   *                 otherwise, it returns false. This result indicates whether the
   *                 sc-link is valid according to the implemented validation logic.
   */
  sc_bool (*check_link_callback)(void * data, sc_addr link_addr);

  //! User-defined data to be passed to the check_link_callback.
  void * check_link_callback_data;

  /*!
   * @brief Callback function to request a sc-link.
   *
   * This function is invoked when a sc-link is requested. This callback can be designed
   * to stop the iteration over sc-links if certain conditions are met, providing control
   * over the sc-link processing flow.
   *
   * @param data Pointer to user-defined data that will be passed to the callback.
   * @param link_addr A sc-address of the sc-link being requested. This sc-address
   *                  identifies which sc-link is being processed in the current
   *                  iteration.
   * @return sc_uint8 Returns a status code indicating the result of the request.
   */
  sc_link_filter_request (*request_link_callback)(void * data, sc_addr link_addr);

  //! User-defined data to be passed to the request_link_callback.
  void * request_link_callback_data;

  /*!
   * @brief Callback function to push a sc-link.
   *
   * This function is called when a sc-link needs to be pushed into user-defined container.
   *
   * @param data Pointer to user-defined data that will be passed to the callback.
   * @param link_addr A sc-address of the link being pushed.
   */
  void (*push_link_callback)(void * data, sc_addr const link_addr);

  //! User-defined data to be passed to the push_link_callback.
  void * push_link_callback_data;

  /*!
   * @brief Callback function to push a sc-link content.
   *
   * This function is invoked when a sc-link content needs to be pushed into user-defined container.
   *
   * @param data Pointer to user-defined data that will be passed to the callback.
   * @param link_addr The address of the sc-link whose content is being pushed.
   * @param link_content A pointer to the content associated with the sc-link.
   */
  void (*push_link_content_callback)(void * data, sc_addr const link_addr, sc_char const * link_content);

  //! User-defined data to be passed to the push_link_content_callback.
  void * push_link_content_callback_data;
} sc_link_handler;

#endif  // _sc_link_filter_h_
