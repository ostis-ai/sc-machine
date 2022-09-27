/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_file_system_h_
#define _sc_file_system_h_

#include "../sc_types.h"

void sc_fs_rmdir(const sc_char * path);

sc_bool sc_fs_mkdirs(const sc_char * path);

void * sc_fs_open_tmp_file(const sc_char * path, sc_char ** tmp_file_name, sc_char * prefix);

#endif
