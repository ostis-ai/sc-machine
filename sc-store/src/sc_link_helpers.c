#include "sc_link_helpers.h"

#include <stdlib.h>
#include <memory.h>
#include <glib.h>

#define SC_DEFAULT_CHECKSUM G_CHECKSUM_SHA256

sc_bool sc_link_calculate_checksum_from_memory(const sc_uint8 *data, const sc_uint32 data_len, sc_check_sum *check_sum)
{
    gchar *result = g_compute_checksum_for_data(SC_DEFAULT_CHECKSUM, data, data_len);
    gssize result_len = g_checksum_type_get_length(SC_DEFAULT_CHECKSUM);

    g_assert(result_len <= SC_MAX_CHECKSUM_LEN);
    g_assert(check_sum != 0);

    check_sum->len = result_len;
    memcpy(&(check_sum->data[0]), result, result_len);
    g_free(result);

    return SC_TRUE;
}



