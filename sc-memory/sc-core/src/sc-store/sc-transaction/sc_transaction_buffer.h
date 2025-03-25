#ifndef SC_TRANSACTION_BUFFER_H
#define SC_TRANSACTION_BUFFER_H

#include <sc-core/sc-container/sc_list.h>

typedef struct sc_transaction_buffer{
  sc_list* new_elements;
  sc_list* modified_elements;
  sc_list* deleted_elements;
} sc_transaction_buffer;

#endif
