/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifdef SC_ROCKSDB_FS_STORAGE

extern "C"
{
#  include "sc_rocksdb_fs_storage.h"
#  include "../sc_link_helpers.h"
#  include "sc_file_system.h"
}

#  include "../sc-base/sc_allocator.h"
#  include "../sc-container/sc-string/sc_string.h"
#  include "../sc-base/sc_message.h"

#  include "rocksdb/db.h"
#  include "../sc_element.h"

#  include <iostream>
#  include <mutex>
#  include <vector>
#  include <string>
#  include <sstream>

rocksdb::DB * gDBInstance = nullptr;
std::string gInstancePath;
std::mutex gMutex;

using ScAddrsVector = std::vector<sc_addr>;
using MutexGuard = std::lock_guard<std::mutex>;

namespace
{
bool CreateDBInstance()
{
  rocksdb::Options options;
  options.create_if_missing = true;

  sc_fs_mkdirs(gInstancePath.c_str());
  rocksdb::Status status = rocksdb::DB::Open(options, gInstancePath, &gDBInstance);
  if (!status.ok())
    std::cerr << "Can't initialize file memory: " << status.ToString() << std::endl;

  return status.ok();
}

void DestroyDBInstance()
{
  assert(gDBInstance);
  gDBInstance->SyncWAL();
  delete gDBInstance;

  gDBInstance = nullptr;
}

std::string ChecksumToString(sc_check_sum const * check_sum)
{
  return {check_sum->data, check_sum->data + check_sum->len};
}

std::string MakeAddrsKey(sc_check_sum const * check_sum)
{
  return ChecksumToString(check_sum) + "_addrs";
}

std::string MakeContentKey(sc_check_sum const * check_sum)
{
  return ChecksumToString(check_sum) + "_content";
}

std::string DataToStringBuffer(sc_list * addrs)
{
  std::ostringstream stream;
  stream << "@addrs";

  sc_iterator * it = sc_list_iterator(addrs);
  while (sc_iterator_next(it))
  {
    auto addr_hash = (sc_addr_hash)sc_iterator_get(it);
    stream << addr_hash << ",";
  }
  sc_iterator_destroy(it);

  return stream.str();
}

void StringBufferToData(std::string const & data, sc_list * result)
{
  std::stringstream stream{data.substr(6)};

  sc_addr_hash addr_hash;
  while (stream)
  {
    if (stream >> addr_hash)
      sc_list_push_back(result, (void *)addr_hash);

    sc_char ch;
    stream >> ch;
  }
}

#  define SC_RES(expr) (expr) ? SC_RESULT_OK : SC_RESULT_ERROR;

}  // namespace

sc_bool sc_rocksdb_fs_storage_initialize(const sc_char * repo_path)
{
  MutexGuard lock(gMutex);

  sc_message("Initialize sc-rocksdb fs-storage from path: %s", repo_path);
  gInstancePath = std::string(repo_path) + "/file_memory";
  return SC_RES(CreateDBInstance());
}

sc_bool sc_rocksdb_fs_storage_shutdown()
{
  sc_message("Shutdown sc-rocksdb fs-storage");
  MutexGuard lock(gMutex);
  DestroyDBInstance();

  return SC_TRUE;
}

sc_bool sc_rocksdb_fs_storage_clear()
{
  MutexGuard lock(gMutex);

  assert(gDBInstance);
  DestroyDBInstance();

  rocksdb::Options options;
  rocksdb::DestroyDB(gInstancePath, options).ok();

  return SC_RES(CreateDBInstance());
}

sc_bool sc_rocksdb_fs_storage_save()
{
  MutexGuard lock(gMutex);

  assert(gDBInstance);
  rocksdb::FlushOptions options;
  options.wait = true;

  rocksdb::Status status = gDBInstance->Flush(options);
  return SC_RES(status.ok());
}

sc_char * sc_rocksdb_fs_storage_read_stream_new(const sc_check_sum * check_sum)
{
  MutexGuard lock(gMutex);

  assert(gDBInstance);
  std::string value;

  rocksdb::ReadOptions options;
  rocksdb::Status status = gDBInstance->Get(options, MakeContentKey(check_sum), &value);

  if (status.ok())
  {
    auto * data = sc_mem_new(sc_char, value.size() + 1);
    if (data == nullptr)
      return nullptr;

    sc_mem_cpy(data, value.c_str(), value.size());
    return data;
  }

  return nullptr;
}

sc_result sc_rocksdb_fs_storage_write_stream(const sc_check_sum * check_sum, const sc_char * sc_string)
{
  MutexGuard lock(gMutex);

  assert(gDBInstance);
  rocksdb::WriteOptions options;
  rocksdb::Status status = gDBInstance->Put(options, MakeContentKey(check_sum), sc_string);

  return SC_RES(status.ok());
}

sc_result sc_rocksdb_fs_storage_addr_ref_append(sc_addr addr, const sc_check_sum * check_sum)
{
  MutexGuard lock(gMutex);

  assert(gDBInstance);

  std::string const key = MakeAddrsKey(check_sum);
  std::string value;

  rocksdb::ReadOptions readOptions;
  rocksdb::Status status = gDBInstance->Get(readOptions, key, &value);

  sc_list * addrs;
  sc_list_init(&addrs);

  if (status.ok())
    StringBufferToData(value, addrs);

  sc_addr_hash addr_hash = SC_ADDR_LOCAL_TO_INT(addr);
  sc_list_push_back(addrs, (void *)addr_hash);

  rocksdb::WriteOptions writeOptions;
  status = gDBInstance->Put(writeOptions, key, DataToStringBuffer(addrs));

  sc_list_destroy(addrs);

  return SC_RES(status.ok());
}

sc_bool sc_hashes_compare(void * hash, void * other)
{
  return (sc_addr_hash)hash == (sc_addr_hash)other;
}

sc_result sc_rocksdb_fs_storage_addr_ref_remove(const sc_addr addr, const sc_check_sum * check_sum)
{
  MutexGuard lock(gMutex);

  assert(gDBInstance);

  std::string const key = MakeAddrsKey(check_sum);
  std::string value;

  rocksdb::ReadOptions readOptions;
  rocksdb::Status status = gDBInstance->Get(readOptions, key, &value);

  if (!status.ok())
    return SC_RESULT_ERROR_IO;

  sc_list * addrs;
  sc_list_init(&addrs);
  StringBufferToData(value, addrs);

  sc_addr_hash addr_hash = SC_ADDR_LOCAL_TO_INT(addr);
  sc_list_remove_if(addrs, (void *)addr_hash, sc_hashes_compare);

  if (addrs->size == 0)
  {
    rocksdb::WriteOptions writeOptions;
    status = gDBInstance->Delete(writeOptions, key);
    status = gDBInstance->Delete(writeOptions, MakeContentKey(check_sum));
  }
  else
  {
    rocksdb::WriteOptions writeOptions;
    status = gDBInstance->Put(writeOptions, key, DataToStringBuffer(addrs));
  }

  sc_list_destroy(addrs);

  return SC_RES(status.ok());
}

sc_result sc_rocksdb_fs_storage_find(const sc_check_sum * check_sum, sc_list * result)
{
  MutexGuard lock(gMutex);

  assert(gDBInstance);

  std::string const key = MakeAddrsKey(check_sum);
  std::string value;

  rocksdb::ReadOptions readOptions;
  rocksdb::Status status = gDBInstance->Get(readOptions, key, &value);

  if (!status.ok())
    return SC_RESULT_ERROR_IO;

  StringBufferToData(value, result);

  return SC_RESULT_OK;
}

sc_bool _sc_rocksdb_fs_storage_is_app_string(
    const std::string & str,
    const sc_char * substring,
    sc_uint32 substring_size,
    sc_uint32 max_length_to_search_as_prefix)
{
  if (str.find("@addrs") != std::string::npos)
    return SC_FALSE;

  sc_bool const isPrefix =
      substring_size <= max_length_to_search_as_prefix && sc_str_cmp(str.substr(0, substring_size).c_str(), substring);
  sc_bool const isSubstr = substring_size > max_length_to_search_as_prefix && str.find(substring) != std::string::npos;

  return isPrefix || isSubstr;
}

sc_bool sc_rocksdb_fs_storage_find_sc_links_by_content_substring(
    const sc_char * sc_substring,
    const sc_uint32 sc_substring_size,
    sc_list ** result,
    sc_uint32 max_length_to_search_as_prefix)
{
  MutexGuard lock(gMutex);

  assert(gDBInstance);

  rocksdb::ReadOptions readOptions;
  auto const & it = gDBInstance->NewIterator(readOptions);
  it->SeekToFirst();

  sc_list_init(result);
  while (it->Valid())
  {
    auto const & slice = it->value();

    std::string str{slice.data()};
    str.resize(slice.size());

    if (_sc_rocksdb_fs_storage_is_app_string(str, sc_substring, sc_substring_size, max_length_to_search_as_prefix))
    {
      sc_check_sum * check_sum;
      sc_link_calculate_checksum(str.c_str(), str.size(), &check_sum);

      std::string const key = MakeAddrsKey(check_sum);
      std::string value;

      rocksdb::Status status = gDBInstance->Get(readOptions, key, &value);
      if (!status.ok())
      {
        it->Next();
        continue;
      }

      StringBufferToData(value, *result);
    }

    it->Next();
  }

  return SC_TRUE;
}

sc_bool sc_rocksdb_fs_storage_find_sc_links_contents_by_content_substring(
    const sc_char * sc_substring,
    const sc_uint32 sc_substring_size,
    sc_list ** result,
    sc_uint32 max_length_to_search_as_prefix)
{
  MutexGuard lock(gMutex);

  assert(gDBInstance);

  rocksdb::ReadOptions readOptions;
  auto const & it = gDBInstance->NewIterator(readOptions);
  it->SeekToFirst();

  sc_list_init(result);
  while (it->Valid())
  {
    auto const & slice = it->value();

    std::string str{slice.data()};
    str.resize(slice.size());

    if (_sc_rocksdb_fs_storage_is_app_string(str, sc_substring, sc_substring_size, max_length_to_search_as_prefix))
    {
      sc_char * copy;
      sc_str_cpy(copy, str.c_str(), str.size());
      sc_list_push_back(*result, copy);
    }

    it->Next();
  }

  return SC_TRUE;
}

sc_bool sc_rocksdb_fs_storage_append_sc_link_content_string(
    const sc_element * element,
    const sc_addr addr,
    const sc_char * sc_string,
    const sc_uint32 size)
{
  sc_char * current_string;
  sc_uint32 current_size = 0;
  sc_rocksdb_fs_storage_get_sc_link_content_string_ext(element, addr, &current_string, &current_size);

  if (current_size != 0 && current_string != null_ptr)
  {
    sc_check_sum * check_sum;
    sc_link_calculate_checksum(current_string, current_size, &check_sum);
    sc_rocksdb_fs_storage_addr_ref_remove(addr, check_sum);
    sc_mem_free(check_sum);
  }
  sc_mem_free(current_string);

  sc_check_sum * check_sum;
  sc_link_calculate_checksum(sc_string, size, &check_sum);
  sc_rocksdb_fs_storage_addr_ref_append(addr, check_sum);

  sc_mem_cpy(((sc_element *)element)->content.data, check_sum->data, check_sum->len);
  sc_rocksdb_fs_storage_write_stream(check_sum, sc_string);
  sc_mem_free(check_sum);

  return SC_TRUE;
}

sc_bool sc_rocksdb_fs_storage_get_sc_links_by_content_string(
    const sc_char * sc_string,
    const sc_uint32 sc_string_size,
    sc_list ** links)
{
  sc_check_sum * check_sum;
  sc_link_calculate_checksum(sc_string, sc_string_size, &check_sum);

  sc_list_init(links);
  sc_result result = sc_rocksdb_fs_storage_find(check_sum, *links);
  sc_mem_free(check_sum);

  return result;
}

void sc_rocksdb_fs_storage_get_sc_link_content_string_ext(
    const sc_element * element,
    const sc_addr addr,
    sc_char ** sc_string,
    sc_uint32 * size)
{
  sc_check_sum check_sum;
  sc_mem_cpy(check_sum.data, element->content.data, SC_CHECKSUM_LEN);
  check_sum.len = SC_CHECKSUM_LEN;

  sc_char * data = sc_rocksdb_fs_storage_read_stream_new(&check_sum);

  if (data != null_ptr)
  {
    *sc_string = data;
    *size = sc_str_len(data);
  }
  else
  {
    *sc_string = (sc_char *)null_ptr;
    *size = 0;
  }
}

sc_bool sc_rocksdb_fs_storage_remove_sc_link_content_string(const sc_element * element, const sc_addr addr)
{
  sc_char * sc_string;
  sc_uint32 sc_string_size = 0;
  sc_rocksdb_fs_storage_get_sc_link_content_string_ext(element, addr, &sc_string, &sc_string_size);

  if (sc_string == null_ptr)
    sc_string_empty(sc_string);

  sc_check_sum * check_sum;
  sc_link_calculate_checksum(sc_string, sc_string_size, &check_sum);
  sc_mem_free(sc_string);

  sc_result result = sc_rocksdb_fs_storage_addr_ref_remove(addr, check_sum);
  sc_mem_free(check_sum);

  return result == SC_RESULT_OK;
}

#endif
