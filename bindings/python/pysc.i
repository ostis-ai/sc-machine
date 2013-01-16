%module pysc
%include "typemaps.i"
%include "pysc_types.i"

%{
#include "sc_memory_headers.h"
#include "sc_helper.h"
%}

/*%include "sc_memory_headers.h"
%include "sc_helper.h"*/

%apply (char *STRING, int LENGTH) { (char *data, int size) };

/* -------- Memory -------- */
sc_bool sc_memory_initialize(const sc_char *repo_path, const sc_char *config_file);
void sc_memory_shutdown();
sc_bool sc_memory_is_initialized();
sc_bool sc_memory_is_element(sc_addr addr);
sc_result sc_memory_element_free(sc_addr addr);
sc_addr sc_memory_node_new(sc_type type);
sc_addr sc_memory_link_new();
sc_addr sc_memory_arc_new(sc_type type, sc_addr beg, sc_addr end);
sc_result sc_memory_get_element_type(sc_addr addr, sc_type *result);
sc_result sc_memory_get_arc_begin(sc_addr addr, sc_addr *result);
sc_result sc_memory_get_arc_end(sc_addr addr, sc_addr *result);
sc_result sc_memory_set_link_content(sc_addr addr, const sc_stream *stream);
sc_result sc_memory_find_links_with_content(const sc_stream *stream, sc_addr **result, sc_uint32 *result_count);

/* -------- Helper -------- */
sc_result sc_helper_init();
void sc_helper_shutdown();
sc_result sc_helper_find_element_by_system_identifier(sc_char* data, sc_uint32 len, sc_addr *result_addr);
sc_result sc_helper_set_system_identifier(sc_addr addr, sc_char* data, sc_uint32 len);
sc_result sc_helper_get_keynode(sc_keynode keynode, sc_addr *keynode_addr);

/* -------- Streams ------- */
#include "sc-store/sc_stream.h"
#include "sc-store/sc_stream_file.h"
#include "sc-store/sc_stream_memory.h"

%inline %{

      
%}

