#ifndef _sc_link_helpers_h_
#define _sc_link_helpers_h_

#include "sc_types.h"


/*! Caclulates checksum for data in memory
 * @param data Data pointer to calculate checksum
 * @param data_len Length of data in bytes
 * @param check_sum Pointer to stucture, that contains calculated checksum
 *
 * @return If checksum calculated, then return SC_TRUE; otherwise return SC_FALSE
 */
sc_bool sc_link_calculate_checksum_from_memory(const sc_uint8 *data, const sc_uint32 data_len, sc_check_sum *check_sum);



#endif
