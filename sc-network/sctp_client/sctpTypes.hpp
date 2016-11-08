/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

extern "C"
{
#include "sc-memory/sc_memory_headers.h"
#include "sc-memory/sc_helper.h"
}

#include <stdint.h>

#define POINTER_TO_UINT(p)	((sc_uint) (sc_ulong) (p))
#define UINT_TO_POINTER(u)	((sc_pointer) (sc_ulong) (u))

typedef enum
{
    SCTP_CMD_UNKNOWN            = 0x00, // unkown command
    SCTP_CMD_CHECK_ELEMENT      = 0x01, // check if specified sc-element exist
    SCTP_CMD_GET_ELEMENT_TYPE   = 0x02, // return sc-element type
    SCTP_CMD_ERASE_ELEMENT      = 0x03, // erase specified sc-element
    SCTP_CMD_CREATE_NODE        = 0x04, // create new sc-node
    SCTP_CMD_CREATE_LINK        = 0x05, // create new sc-link
    SCTP_CMD_CREATE_ARC         = 0x06, // create new sc-arc
    SCTP_CMD_GET_ARC            = 0x07, // return begin element of sc-arc

    SCTP_CMD_GET_LINK_CONTENT   = 0x09, // return content of sc-link
    SCTP_CMD_FIND_LINKS         = 0x0a, // return sc-links with specified content
    SCTP_CMD_SET_LINK_CONTENT   = 0x0b, // setup new content for the link
    SCTP_CMD_ITERATE_ELEMENTS   = 0x0c, // return base template iteration results
    SCTP_CMD_ITERATE_CONSTRUCTION = 0x0d, // return advanced template iteration results
    SCTP_CMD_EVENT_CREATE       = 0x0e, // create subscription to specified event
    SCTP_CMD_EVENT_DESTROY      = 0x0f, // destroys specified event subscription
    SCTP_CMD_EVENT_EMIT         = 0x10, // emits events to client
	SCTP_CMD_GENERATE_CONSTRUCTION = 0x11, // generate cunstrution by template

    SCTP_CMD_FIND_ELEMENT_BY_SYSITDF = 0xa0, // return sc-element by it system identifier
    SCTP_CMD_SET_SYSIDTF        = 0xa1,   // setup new system identifier for sc-element
    SCTP_CMD_STATISTICS         = 0xa2, // return usage statistics from server
    SCTP_CMD_VERSION            = 0xa3  // return version of used sctp protocol

} eSctpCommandCode;

typedef enum
{

    SCTP_ITERATOR_3F_A_A = 0,
    SCTP_ITERATOR_3A_A_F = 1,
    SCTP_ITERATOR_3F_A_F = 2,
    SCTP_ITERATOR_5F_A_A_A_F = 3,
    SCTP_ITERATOR_5A_A_F_A_F = 4,
    SCTP_ITERATOR_5F_A_F_A_F = 5,
    SCTP_ITERATOR_5F_A_F_A_A = 6,
    SCTP_ITERATOR_5F_A_A_A_A = 7,
    SCTP_ITERATOR_5A_A_F_A_A = 8,

    SCTP_ITERATOR_COUNT

} eSctpIteratorType;

typedef enum
{
    SCTP_RESULT_OK              = 0x00, //
    SCTP_RESULT_FAIL            = 0x01, //
    SCTP_RESULT_ERROR_NO_ELEMENT= 0x02, // sc-element wasn't found
    SCTP_RESULT_NORIGHTS        = 0x03

} eSctpResultCode;

//! Command processing result codes
typedef enum
{
    SCTP_NO_ERROR = 0,                      // No any errors, command processed
    SCTP_ERROR,                             // There are any errors while processing command
    SCTP_ERROR_UNKNOWN_CMD,                 // Unknown command code
    SCTP_ERROR_CMD_READ_PARAMS,             // Error while read command parameters
    SCTP_ERROR_CMD_HEADER_READ_TIMEOUT,     // Timeout while waiting command header
    SCTP_ERROR_CMD_PARAM_READ_TIMEOUT       // Timeout while waiting command params

} eSctpErrorCode;


typedef sc_uint32 tEventId;

