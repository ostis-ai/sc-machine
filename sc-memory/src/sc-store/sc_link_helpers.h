#ifndef _sc_link_helpers_h_
#define _sc_link_helpers_h_

#include "sc_types.h"
#include "sc_stream.h"


/*! Caclulates checksum for data in stream
 * @param stream Pointer to data stream for checksum calculation
 * @param check_sum Pointer to stucture, that contains calculated checksum
 *
 * @return If checksum calculated, then return SC_TRUE; otherwise return SC_FALSE
 */
sc_bool sc_link_calculate_checksum(const sc_stream *stream, sc_check_sum *check_sum);


#endif
