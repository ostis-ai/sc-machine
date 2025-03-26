#ifndef SC_TRANSACTION_BUFFER_H
#define SC_TRANSACTION_BUFFER_H

#include <sc-store/sc-transaction/sc_element_version.h>

#include <sc-core/sc-container/sc_list.h>
#include <sc-core/sc_stream.h>

typedef struct sc_transaction_buffer
{
  sc_list * new_elements;
  sc_list * modified_elements;
  sc_list * deleted_elements;
} sc_transaction_buffer;

sc_bool sc_transaction_buffer_initialize(sc_transaction_buffer * transaction_buffer);

sc_bool sc_transaction_buffer_created_add(sc_transaction_buffer * buffer, sc_addr addr);
sc_bool sc_transaction_buffer_modified_add(
    sc_transaction_buffer * buf,
    sc_addr addr,
    sc_element_version * version,
    SC_ELEMENT_MODIFIED_FLAGS flags);
sc_bool sc_transaction_buffer_removed_add(sc_transaction_buffer * buffer, sc_addr addr);
sc_bool sc_transaction_buffer_content_change_add(sc_transaction_buffer * buffer, sc_addr addr, sc_stream * content);

sc_bool transaction_buffer_contains_created(sc_transaction_buffer const * buffer, sc_addr addr);

#endif
