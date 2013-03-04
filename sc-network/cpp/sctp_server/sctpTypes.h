#ifndef SCTPTYPES_H
#define SCTPTYPES_H

typedef enum
{
    SCTP_CMD_UNKNOWN            = 0x00, // unkown command
    SCTP_CMD_CHECK_ELEMENT      = 0x01, // check if specified sc-element exist
    SCTP_CMD_GET_ELEMENT_TYPE   = 0x02, // return sc-element type
    SCTP_CMD_ERASE_ELEMENT      = 0x03, // erase specified sc-element
    SCTP_CMD_CREATE_NODE        = 0x04, // create new sc-node
    SCTP_CMD_CREAET_LINK        = 0x05, // create new sc-link
    SCTP_CMD_CREATE_ARC         = 0x06, // create new sc-arc
    SCTP_CMD_GET_ARC_BEGIN      = 0x07, // return begin element of sc-arc
    SCTP_CMD_GET_ARC_END        = 0x08, // return end element of sc-arc
    SCTP_CMD_GET_LINK_CONTENT   = 0x09, // return content of sc-link
    SCTP_CMD_FIND_LINKS         = 0x0a, // return sc-links with specified content
    SCTP_CMD_ITERATE_ELEMENTS   = 0x0b, // return base template iteration result

    SCTP_CMD_FIND_ELEMENT_BY_SYSITDF = 0xa0, // return sc-element by it system identifier
    SCTP_CMD_STATISTICS         = 0xa1, // return usage statistics from server

    SCTP_CMD_SHUTDOWN           = 0xfe // disconnect client from server

} sctpCommandCode;

typedef enum
{

    SCTP_ITERATOR_3F_A_A = 0,
    SCTP_ITERATOR_3A_A_F,
    SCTP_ITERATOR_3F_A_F,
    SCTP_ITERATOR_5F_A_A_A_F,
    SCTP_ITERATOR_5_A_A_F_A_F,
    SCTP_ITERATOR_5_F_A_F_A_F,
    SCTP_ITERATOR_5_F_A_F_A_A,
    SCTP_ITERATOR_5_F_A_A_A_A,
    SCTP_ITERATOR_5_A_A_F_A_A,

    SCTP_ITERATOR_COUNT

} sctpIteratorType;

typedef enum
{
    SCTP_RESULT_OK              = 0x00, //
    SCTP_RESULT_FAIL            = 0x01, //
    SCTP_RESULT_ERROR_NO_ELEMENT= 0x02  // sc-element wasn't found

} sctpResultCode;

//! Command processing result codes
typedef enum
{
    SCTP_ERROR_NO = 0,                      // No any errors, command processed
    SCTP_ERROR,                             // There are any errors while processing command
    SCTP_ERROR_UNKNOWN_CMD,                 // Unknown command code
    SCTP_ERROR_CMD_READ_PARAMS,             // Error while read command parameters
    SCTP_ERROR_CMD_HEADER_READ_TIMEOUT,     // Timeout while waiting command header
    SCTP_ERROR_CMD_PARAM_READ_TIMEOUT       // Timeout while waiting command params

} sctpErrorCode;

#endif // SCTPTYPES_H
