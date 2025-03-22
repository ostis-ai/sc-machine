#ifndef SC_ELEMENT_VERSION_H
#define SC_ELEMENT_VERSION_H

#include <stdint.h>

typedef struct element_version {
  void* data;
  uint64_t version_id;
  element_version * prev_version;
  uint64_t epoch;
  uint64_t transaction_id;
  int is_committed;
} element_version;

element_version* create_new_version(void* data, uint64_t version_id, uint64_t transaction_id, uint64_t epoch);
void commit_version(element_version* version);
void rollback_version(element_version* version);

#endif
