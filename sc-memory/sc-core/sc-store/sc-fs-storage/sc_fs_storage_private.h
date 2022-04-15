/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_fs_store_h_
#define _sc_fs_store_h_

void _sc_fm_remove_dir(const char * path);

sc_bool _sc_fs_mkdirs(const char * path);

sc_char * itora(sc_uint32 num);

#endif
