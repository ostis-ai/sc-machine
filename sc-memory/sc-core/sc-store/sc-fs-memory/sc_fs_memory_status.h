/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_fs_memory_status_h_
#define _sc_fs_memory_status_h_

typedef enum _sc_fs_memory_status
{
  SC_FS_MEMORY_OK = 0,

  SC_FS_MEMORY_NO,  // no memory, it is null_ptr or not correct
  SC_FS_MEMORY_NO_STRING,
  SC_FS_MEMORY_WRONG_PATH,
  SC_FS_MEMORY_WRITE_ERROR,
  SC_FS_MEMORY_READ_ERROR
} sc_fs_memory_status;

#endif
