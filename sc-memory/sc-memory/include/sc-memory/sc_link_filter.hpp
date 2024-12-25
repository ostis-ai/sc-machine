/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_addr.hpp"

#include <sc-core/sc_link_filter.h>

/*!
 * @class ScLinkFilter
 * @brief An abstract base class for filtering links in a graph.
 *
 * This class defines an interface for link filtering, allowing derived classes
 * to implement specific criteria for checking and requesting links.
 */
class ScLinkFilter
{
public:
  /*!
   * @enum ScLinkFilterRequest
   * @brief Enumeration for link filter requests.
   *
   * This enumeration defines the possible responses when a link is requested.
   */
  enum class ScLinkFilterRequest : sc_uint8
  {
    CONTINUE = SC_LINK_FILTER_REQUEST_CONTINUE,  ///< Continue processing links.
    STOP = SC_LINK_FILTER_REQUEST_STOP           ///< Stop processing links.
  };

  /*!
   * @brief Checks if a given link address meets the filter criteria.
   * @param linkAddr A address of the link to check.
   * @return true if the link meets the criteria; otherwise, false.
   */
  virtual bool CheckLink(ScAddr const & linkAddr) = 0;

  /*!
   * @brief Requests a decision on how to handle a given link address.
   * @param linkAddr A sc-address of the link to request action for.
   * @return A ScLinkFilterRequest indicating whether to continue or stop processing.
   */
  virtual ScLinkFilterRequest RequestLink(ScAddr const & linkAddr) = 0;
};
