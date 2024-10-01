/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory.h"
#include "sc_memory_params.h"

#include "sc-store/sc_storage.h"
#include "sc-store/sc_storage_private.h"
#include "sc_memory_private.h"
#include "sc_helper.h"
#include "sc_helper_private.h"
#include "sc_keynodes.h"
#include "sc_memory_ext.h"
#include "sc_memory_context_manager.h"
#include "sc_memory_context_permissions.h"

#include "sc-store/sc_types.h"
#include "sc-store/sc-base/sc_allocator.h"
#include "sc-store/sc-container/sc-string/sc_string.h"

struct _sc_memory
{
  sc_addr myself_addr;
  sc_memory_context_manager * context_manager;
};

sc_memory * memory = null_ptr;
sc_memory_context * s_memory_default_ctx = null_ptr;

sc_memory_context * sc_memory_initialize(sc_memory_params const * params, sc_memory_context ** context)
{
  sc_memory_info("Initialize");

  sc_char * string = sc_version_string_new(&params->version);
  sc_memory_info("Version: %s", string);
  sc_version_string_free(string);

  sc_memory_info("Logger:");
  sc_message("\tLog type: %s", params->log_type);
  sc_message("\tLog file: %s", params->log_file);
  sc_message("\tLog level: %s", params->log_level);

  if (sc_storage_initialize(params) != SC_RESULT_OK)
  {
    s_memory_default_ctx = null_ptr;
    sc_memory_error("Error while initialize sc-storage");
    goto error;
  }

  memory = sc_mem_new(sc_memory, 1);

  sc_storage_start_new_process();

  _sc_memory_context_manager_initialize(&memory->context_manager, params->user_mode);

  if (sc_helper_init(s_memory_default_ctx) != SC_RESULT_OK)
  {
    sc_memory_error("Error while initialize sc-helper");
    goto error;
  }

  sc_addr init_memory_generated_structure_addr = SC_ADDR_EMPTY;
  if (params->init_memory_generated_upload)
    sc_helper_resolve_system_identifier(
        s_memory_default_ctx, params->init_memory_generated_structure, &init_memory_generated_structure_addr);

  if (sc_keynodes_initialize(s_memory_default_ctx, init_memory_generated_structure_addr) != SC_RESULT_OK)
    goto error;

  _sc_memory_context_assign_context_for_system(memory->context_manager, &memory->myself_addr);
  _sc_memory_context_manager_register_user_events(memory->context_manager);
  _sc_memory_context_handle_all_user_permissions(memory->context_manager);

  *context = s_memory_default_ctx;

  sc_memory_info("Build configuration:");
  sc_message("\tResult structure upload: %s", params->init_memory_generated_upload ? "On" : "Off");
  sc_message("\tInit memory generated structure: %s", params->init_memory_generated_structure);
  sc_message("\tExtensions path: %s", params->ext_path);

  if (sc_memory_init_ext(params->ext_path, params->enabled_exts, init_memory_generated_structure_addr) != SC_RESULT_OK)
  {
    sc_memory_error("Error while initialize extensions");
    goto error;
  }

  sc_storage_end_new_process();
  sc_memory_info("Successfully initialized");
  return s_memory_default_ctx;

error:
  sc_storage_end_new_process();
  sc_memory_info("Initialized with errors");
  return null_ptr;
}

sc_result sc_memory_init_ext(
    sc_char const * ext_path,
    sc_char const ** enabled_list,
    sc_addr const init_memory_generated_structure_addr)
{
  sc_memory_info("Initialize extensions");

  sc_result const ext_res = sc_ext_initialize(ext_path, enabled_list, init_memory_generated_structure_addr);

  switch (ext_res)
  {
  case SC_RESULT_OK:
    sc_memory_info("Extensions initialized");
    break;

  case SC_RESULT_ERROR_INVALID_PARAMS:
    sc_memory_warning("Extensions directory `%s` doesn't exist", ext_path);
    break;

  default:
    sc_memory_warning("Unknown error while extensions initializing");
    break;
  }

  return ext_res;
}

sc_result sc_memory_shutdown(sc_bool save_state)
{
  sc_memory_info("Shutdown");

  if (memory == null_ptr)
    goto error;

  sc_memory_shutdown_ext();
  sc_helper_shutdown();

  _sc_memory_context_manager_unregister_user_events(memory->context_manager);

error:
  if (sc_storage_shutdown(save_state) != SC_RESULT_OK)
    return SC_RESULT_ERROR;

  if (memory == null_ptr)
    return SC_RESULT_ERROR;

  _sc_memory_context_manager_shutdown(memory->context_manager);
  memory->context_manager = null_ptr;

  sc_mem_free(memory);
  memory = null_ptr;

  sc_memory_info("Shutdown");

  return SC_RESULT_OK;
}

void sc_memory_shutdown_ext()
{
  sc_ext_shutdown();
}

void * sc_memory_get_context_manager()
{
  return memory->context_manager;
}

sc_memory_context * sc_memory_context_new_ext(sc_addr user_addr)
{
  if (memory == null_ptr)
    return null_ptr;

  return _sc_memory_context_resolve_impl(memory->context_manager, user_addr);
}

void sc_memory_context_free(sc_memory_context * ctx)
{
  if (memory == null_ptr)
    return;

  _sc_memory_context_free_impl(memory->context_manager, ctx);
}

sc_addr sc_memory_context_get_user_addr(sc_memory_context * ctx)
{
  return _sc_memory_context_get_user_addr(ctx);
}

void sc_memory_context_pending_begin(sc_memory_context * ctx)
{
  _sc_memory_context_pending_begin(ctx);
}

void sc_memory_context_pending_end(sc_memory_context * ctx)
{
  _sc_memory_context_pending_end(ctx);
}

void sc_memory_context_blocking_begin(sc_memory_context * ctx)
{
  _sc_memory_context_blocking_begin(ctx);
}

void sc_memory_context_blocking_end(sc_memory_context * ctx)
{
  _sc_memory_context_blocking_end(ctx);
}

sc_bool sc_memory_is_initialized()
{
  return sc_storage_is_initialized();
}

sc_bool sc_memory_is_element(sc_memory_context const * ctx, sc_addr addr)
{
  sc_result result;
  return sc_memory_is_element_ext(ctx, addr, &result);
}

sc_bool sc_memory_is_element_ext(sc_memory_context const * ctx, sc_addr addr, sc_result * result)
{
  if (_sc_memory_context_is_authenticated(memory->context_manager, ctx) == SC_FALSE)
  {
    *result = SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED;
    return SC_FALSE;
  }

  if (_sc_memory_context_check_global_permissions(memory->context_manager, ctx, SC_CONTEXT_PERMISSIONS_READ)
      == SC_FALSE)
  {
    *result = SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS;
    return SC_FALSE;
  }

  *result = SC_RESULT_OK;
  return sc_storage_is_element(ctx, addr);
}

sc_uint32 sc_memory_get_element_outgoing_arcs_count(sc_memory_context const * ctx, sc_addr addr, sc_result * result)
{
  if (_sc_memory_context_is_authenticated(memory->context_manager, ctx) == SC_FALSE)
  {
    *result = SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED;
    return 0;
  }

  if (_sc_memory_context_check_local_and_global_permissions(
          memory->context_manager, ctx, SC_CONTEXT_PERMISSIONS_READ, addr)
      == SC_FALSE)
  {
    *result = SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS;
    return 0;
  }

  return sc_storage_get_element_outgoing_arcs_count(ctx, addr, result);
}

sc_uint32 sc_memory_get_element_incoming_arcs_count(sc_memory_context const * ctx, sc_addr addr, sc_result * result)
{
  if (_sc_memory_context_is_authenticated(memory->context_manager, ctx) == SC_FALSE)
  {
    *result = SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED;
    return 0;
  }

  if (_sc_memory_context_check_local_and_global_permissions(
          memory->context_manager, ctx, SC_CONTEXT_PERMISSIONS_READ, addr)
      == SC_FALSE)
  {
    *result = SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS;
    return 0;
  }

  return sc_storage_get_element_incoming_arcs_count(ctx, addr, result);
}

sc_result sc_memory_element_free(sc_memory_context * ctx, sc_addr addr)
{
  if (_sc_memory_context_is_authenticated(memory->context_manager, ctx) == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED;

  if (_sc_memory_context_check_local_and_global_permissions(
          memory->context_manager, ctx, SC_CONTEXT_PERMISSIONS_ERASE, addr)
      == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_ERASE_PERMISSIONS;

  if (_sc_memory_context_check_global_permissions_to_erase_permissions(
          memory->context_manager, ctx, addr, SC_CONTEXT_PERMISSIONS_TO_ERASE_PERMISSIONS)
      == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_PERMISSIONS_TO_ERASE_PERMISSIONS;

  return sc_storage_element_erase(ctx, addr);
}

sc_addr sc_memory_node_new(sc_memory_context const * ctx, sc_type type)
{
  sc_result result;
  return sc_memory_node_new_ext(ctx, type, &result);
}

sc_addr sc_memory_node_new_ext(sc_memory_context const * ctx, sc_type type, sc_result * result)
{
  if (_sc_memory_context_is_authenticated(memory->context_manager, ctx) == SC_FALSE)
  {
    *result = SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED;
    return SC_ADDR_EMPTY;
  }

  return sc_storage_node_new_ext(ctx, type, result);
}

sc_addr sc_memory_link_new(sc_memory_context const * ctx)
{
  return sc_memory_link_new2(ctx, sc_type_const_node_link);
}

sc_addr sc_memory_link_new2(sc_memory_context const * ctx, sc_type type)
{
  sc_result result;
  return sc_memory_link_new_ext(ctx, type, &result);
}

sc_addr sc_memory_link_new_ext(sc_memory_context const * ctx, sc_type type, sc_result * result)
{
  if (_sc_memory_context_is_authenticated(memory->context_manager, ctx) == SC_FALSE)
  {
    *result = SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED;
    return SC_ADDR_EMPTY;
  }

  return sc_storage_link_new_ext(ctx, type, result);
}

sc_addr sc_memory_arc_new(sc_memory_context const * ctx, sc_type type, sc_addr beg, sc_addr end)
{
  sc_result result;
  return sc_memory_arc_new_ext(ctx, type, beg, end, &result);
}

sc_addr sc_memory_arc_new_ext(sc_memory_context const * ctx, sc_type type, sc_addr beg, sc_addr end, sc_result * result)
{
  if (_sc_memory_context_is_authenticated(memory->context_manager, ctx) == SC_FALSE)
  {
    *result = SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED;
    return SC_ADDR_EMPTY;
  }

  if (_sc_memory_context_check_if_has_permitted_structure(
          memory->context_manager, ctx, SC_CONTEXT_PERMISSIONS_WRITE, beg)
          == SC_FALSE
      || sc_type_has_not_subtype_in_mask(type, sc_type_const_pos_arc))
  {
    if (_sc_memory_context_check_local_and_global_permissions(
            memory->context_manager, ctx, SC_CONTEXT_PERMISSIONS_WRITE, beg)
        == SC_FALSE)
    {
      *result = SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_WRITE_PERMISSIONS;
      return SC_ADDR_EMPTY;
    }
    if (_sc_memory_context_check_local_and_global_permissions(
            memory->context_manager, ctx, SC_CONTEXT_PERMISSIONS_WRITE, end)
        == SC_FALSE)
    {
      *result = SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_WRITE_PERMISSIONS;
      return SC_ADDR_EMPTY;
    }
  }

  if (_sc_memory_context_check_global_permissions_to_write_permissions(
          memory->context_manager, ctx, beg, type, SC_CONTEXT_PERMISSIONS_TO_WRITE_PERMISSIONS)
      == SC_FALSE)
  {
    *result = SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_PERMISSIONS_TO_WRITE_PERMISSIONS;
    return SC_ADDR_EMPTY;
  }

  return sc_storage_arc_new_ext(ctx, type, beg, end, result);
}

sc_result sc_memory_get_element_type(sc_memory_context const * ctx, sc_addr addr, sc_type * result)
{
  if (_sc_memory_context_is_authenticated(memory->context_manager, ctx) == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED;

  if (_sc_memory_context_check_local_and_global_permissions(
          memory->context_manager, ctx, SC_CONTEXT_PERMISSIONS_READ, addr)
      == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS;

  return sc_storage_get_element_type(ctx, addr, result);
}

sc_bool sc_memory_is_type_expendable_to(sc_type type, sc_type new_type)
{
  return sc_storage_is_type_expendable_to(type, new_type);
}

sc_result sc_memory_change_element_subtype(sc_memory_context const * ctx, sc_addr addr, sc_type type)
{
  if (_sc_memory_context_is_authenticated(memory->context_manager, ctx) == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED;

  if (_sc_memory_context_check_local_and_global_permissions(
          memory->context_manager, ctx, SC_CONTEXT_PERMISSIONS_WRITE, addr)
      == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_WRITE_PERMISSIONS;

  return sc_storage_change_element_subtype(ctx, addr, type);
}

sc_result sc_memory_get_arc_begin(sc_memory_context const * ctx, sc_addr addr, sc_addr * begin_addr)
{
  if (_sc_memory_context_is_authenticated(memory->context_manager, ctx) == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED;

  if (_sc_memory_context_check_local_and_global_permissions(
          memory->context_manager, ctx, SC_CONTEXT_PERMISSIONS_READ, addr)
      == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS;

  sc_result const result = sc_storage_get_arc_begin(ctx, addr, begin_addr);

  if (result == SC_RESULT_OK
      && _sc_memory_context_check_local_and_global_permissions(
             memory->context_manager, ctx, SC_CONTEXT_PERMISSIONS_READ, *begin_addr)
             == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS;

  return result;
}

sc_result sc_memory_get_arc_end(sc_memory_context const * ctx, sc_addr addr, sc_addr * end_addr)
{
  if (_sc_memory_context_is_authenticated(memory->context_manager, ctx) == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED;

  if (_sc_memory_context_check_local_and_global_permissions(
          memory->context_manager, ctx, SC_CONTEXT_PERMISSIONS_READ, addr)
      == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS;

  sc_result const result = sc_storage_get_arc_end(ctx, addr, end_addr);

  if (result == SC_RESULT_OK
      && _sc_memory_context_check_local_and_global_permissions(
             memory->context_manager, ctx, SC_CONTEXT_PERMISSIONS_READ, *end_addr)
             == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS;

  return result;
}

sc_result sc_memory_get_arc_info(sc_memory_context const * ctx, sc_addr addr, sc_addr * begin_addr, sc_addr * end_addr)
{
  if (_sc_memory_context_is_authenticated(memory->context_manager, ctx) == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED;

  if (_sc_memory_context_check_local_and_global_permissions(
          memory->context_manager, ctx, SC_CONTEXT_PERMISSIONS_READ, addr)
      == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS;

  sc_result const result = sc_storage_get_arc_info(ctx, addr, begin_addr, end_addr);

  if (result == SC_RESULT_OK
      && _sc_memory_context_check_local_and_global_permissions(
             memory->context_manager, ctx, SC_CONTEXT_PERMISSIONS_READ, *begin_addr)
             == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS;

  if (result == SC_RESULT_OK
      && _sc_memory_context_check_local_and_global_permissions(
             memory->context_manager, ctx, SC_CONTEXT_PERMISSIONS_READ, *end_addr)
             == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS;

  return result;
}

sc_result sc_memory_set_link_content(sc_memory_context const * ctx, sc_addr addr, sc_stream const * stream)
{
  return sc_memory_set_link_content_ext(ctx, addr, stream, SC_TRUE);
}

sc_result sc_memory_set_link_content_ext(
    sc_memory_context const * ctx,
    sc_addr addr,
    sc_stream const * stream,
    sc_bool is_searchable_string)
{
  if (_sc_memory_context_is_authenticated(memory->context_manager, ctx) == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED;

  if (_sc_memory_context_check_local_and_global_permissions(
          memory->context_manager, ctx, SC_CONTEXT_PERMISSIONS_ERASE, addr)
      == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_ERASE_PERMISSIONS;

  if (_sc_memory_context_check_local_and_global_permissions(
          memory->context_manager, ctx, SC_CONTEXT_PERMISSIONS_WRITE, addr)
      == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_WRITE_PERMISSIONS;

  return sc_storage_set_link_content(ctx, addr, stream, is_searchable_string);
}

sc_result sc_memory_get_link_content(sc_memory_context const * ctx, sc_addr addr, sc_stream ** stream)
{
  if (_sc_memory_context_is_authenticated(memory->context_manager, ctx) == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED;

  if (_sc_memory_context_check_local_and_global_permissions(
          memory->context_manager, ctx, SC_CONTEXT_PERMISSIONS_READ, addr)
      == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS;

  return sc_storage_get_link_content(ctx, addr, stream);
}

void _push_link_hash(void * data, sc_addr const link_addr)
{
  sc_list_push_back((sc_list *)data, (sc_addr_hash_to_sc_pointer)SC_ADDR_LOCAL_TO_INT(link_addr));
}

sc_result sc_memory_find_links_with_content_string(
    sc_memory_context const * ctx,
    sc_stream const * stream,
    sc_list ** result_hashes)
{
  sc_list_init(result_hashes);
  return sc_memory_find_links_with_content_string_ext(ctx, stream, *result_hashes, _push_link_hash);
}

sc_result sc_memory_find_links_with_content_string_ext(
    sc_memory_context const * ctx,
    sc_stream const * stream,
    void * data,
    void (*callback)(void * data, sc_addr const link_addr))
{
  if (_sc_memory_context_is_authenticated(memory->context_manager, ctx) == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED;

  return sc_storage_find_links_with_content_string(ctx, stream, data, callback);
}

sc_result sc_memory_find_links_by_content_substring(
    sc_memory_context const * ctx,
    sc_stream const * stream,
    sc_uint32 max_length_to_search_as_prefix,
    sc_list ** result_hashes)
{
  sc_list_init(&*result_hashes);
  return sc_memory_find_links_by_content_substring_ext(
      ctx, stream, max_length_to_search_as_prefix, *result_hashes, _push_link_hash);
}

sc_result sc_memory_find_links_by_content_substring_ext(
    sc_memory_context const * ctx,
    sc_stream const * stream,
    sc_uint32 max_length_to_search_as_prefix,
    void * data,
    void (*callback)(void * data, sc_addr const link_addr))
{
  if (_sc_memory_context_is_authenticated(memory->context_manager, ctx) == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED;

  return sc_storage_find_links_by_content_substring(ctx, stream, max_length_to_search_as_prefix, data, callback);
}

void _test_push_link_content(void * data, sc_addr const link_addr, sc_char const * link_content)
{
  sc_unused(link_addr);

  sc_uint32 const size = sc_str_len(link_content);
  sc_char * copied_string;
  sc_str_cpy(copied_string, link_content, size);

  sc_list_push_back((sc_list *)data, (sc_pointer)copied_string);
}

sc_result sc_memory_find_links_contents_by_content_substring(
    sc_memory_context const * ctx,
    sc_stream const * stream,
    sc_uint32 max_length_to_search_as_prefix,
    sc_list ** strings)
{
  sc_list_init(&*strings);
  return sc_memory_find_links_contents_by_content_substring_ext(
      ctx, stream, max_length_to_search_as_prefix, *strings, _test_push_link_content);
}

sc_result sc_memory_find_links_contents_by_content_substring_ext(
    sc_memory_context const * ctx,
    sc_stream const * stream,
    sc_uint32 max_length_to_search_as_prefix,
    void * data,
    void (*callback)(void * data, sc_addr const link_addr, sc_char const * link_content))
{
  if (_sc_memory_context_is_authenticated(memory->context_manager, ctx) == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED;

  if (_sc_memory_context_check_global_permissions(memory->context_manager, ctx, SC_CONTEXT_PERMISSIONS_READ)
      == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS;

  return sc_storage_find_links_contents_by_content_substring(
      ctx, stream, max_length_to_search_as_prefix, data, callback);
}

sc_result sc_memory_stat(sc_memory_context const * ctx, sc_stat * stat)
{
  if (_sc_memory_context_is_authenticated(memory->context_manager, ctx) == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED;

  if (_sc_memory_context_check_global_permissions(memory->context_manager, ctx, SC_CONTEXT_PERMISSIONS_READ)
      == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS;

  return sc_storage_get_elements_stat(stat);
}

sc_result sc_memory_save(sc_memory_context const * ctx)
{
  if (_sc_memory_context_is_authenticated(memory->context_manager, ctx) == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED;

  if (_sc_memory_context_check_global_permissions(memory->context_manager, ctx, SC_CONTEXT_PERMISSIONS_WRITE)
      == SC_FALSE)
    return SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_WRITE_PERMISSIONS;

  return sc_storage_save(ctx);
}
