/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_file_system_h_
#define _sc_file_system_h_

#include "../sc_types.h"

sc_bool sc_fs_create_file(sc_char const * path);

sc_bool sc_fs_copy_file(sc_char const * path, sc_char const * target_path);

sc_bool sc_fs_remove_file(sc_char const * path);

sc_bool sc_fs_rename_file(sc_char const * old_path, sc_char const * new_path);

sc_bool sc_fs_is_file(sc_char const * path);

sc_bool sc_fs_is_binary_file(sc_char const * file_path);

void sc_fs_get_file_content(sc_char const * file_path, sc_char ** content, sc_uint32 * content_size);

void sc_fs_concat_path(sc_char const * path, sc_char const * postfix, sc_char ** out_path);

void sc_fs_concat_path_ext(sc_char const * path, sc_char const * postfix, sc_char const * ext, sc_char ** out_path);

sc_bool sc_fs_create_directory(sc_char const * path);

sc_bool sc_fs_remove_directory_ext(sc_char const * path, sc_bool remove_root);

sc_bool sc_fs_remove_directory(sc_char const * path);

sc_bool sc_fs_is_directory(sc_char const * path);

void * sc_fs_new_tmp_write_channel(sc_char const * path, sc_char ** tmp_file_name, sc_char * prefix);

sc_char * sc_fs_execute(sc_char const * command);

#endif
