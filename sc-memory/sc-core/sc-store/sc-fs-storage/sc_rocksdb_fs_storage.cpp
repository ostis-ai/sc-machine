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

std::string DataToStringBuffer(ScAddrsVector const & addrs)
{
  size_t const size = addrs.size();
  auto const * pSize = (uint8_t const *)(&size);
  auto const * pData = (uint8_t const *)(addrs.data());

  return std::string(pSize, pSize + sizeof(size)) + std::string(pData, pData + sizeof(sc_addr) * size);
}

ScAddrsVector StringBufferToData(std::string const & data)
{
  size_t size = 0;
  assert(data.size() >= sizeof(size));

  auto const * pData = (uint8_t const *)data.data();
  size = *((size_t const *)pData);

  auto const * pAddrs = (sc_addr const *)(pData + sizeof(size));
  assert(data.size() == (sizeof(size) + sizeof(sc_addr) * size));

  return {pAddrs, pAddrs + size};
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

  ScAddrsVector addrs;

  if (status.ok())
    addrs = StringBufferToData(value);

  addrs.push_back(addr);

  rocksdb::WriteOptions writeOptions;
  status = gDBInstance->Put(writeOptions, key, DataToStringBuffer(addrs));

  return SC_RES(status.ok());
}

sc_result sc_rocksdb_fs_storage_addr_ref_remove(sc_addr addr, const sc_check_sum * check_sum)
{
  MutexGuard lock(gMutex);

  assert(gDBInstance);

  std::string const key = MakeAddrsKey(check_sum);
  std::string value;

  rocksdb::ReadOptions readOptions;
  rocksdb::Status status = gDBInstance->Get(readOptions, key, &value);

  if (!status.ok())
    return SC_RESULT_ERROR_IO;

  ScAddrsVector addrs = StringBufferToData(value);
  for (size_t i = 0; i < addrs.size(); ++i)
  {
    if (SC_ADDR_IS_EQUAL(addrs[i], addr))
    {
      addrs.erase(addrs.begin() + (sc_int64)i);
      break;
    }
  }

  if (addrs.empty())
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

  return SC_RES(status.ok());
}

sc_result sc_rocksdb_fs_storage_find(const sc_check_sum * check_sum, sc_addr ** result, sc_uint32 * result_count)
{
  MutexGuard lock(gMutex);

  assert(gDBInstance);

  std::string const key = MakeAddrsKey(check_sum);
  std::string value;

  rocksdb::ReadOptions readOptions;
  rocksdb::Status status = gDBInstance->Get(readOptions, key, &value);

  if (!status.ok())
    return SC_RESULT_ERROR_IO;

  ScAddrsVector addrs = StringBufferToData(value);

  *result_count = sc_uint32(addrs.size());

  size_t const resultBytes = sizeof(sc_addr) * addrs.size();
  *result = (sc_addr *)malloc(resultBytes);
  sc_mem_cpy(*result, addrs.data(), resultBytes);

  return SC_RESULT_OK;
}

sc_bool sc_rocksdb_fs_storage_find_sc_links_by_content_substr(
    const sc_char * sc_substr,
    sc_addr ** result,
    sc_uint32 * result_count,
    sc_uint32 max_length_to_search_as_prefix)
{
  MutexGuard lock(gMutex);

  assert(gDBInstance);

  rocksdb::ReadOptions readOptions;
  auto const & it = gDBInstance->NewIterator(readOptions);
  it->SeekToFirst();

  ScAddrsVector foundAddrs;
  while (it->Valid())
  {
    auto const & slice = it->value();

    std::string str{slice.data()};
    str.resize(slice.size());

    sc_bool const isPrefix = strlen(sc_substr) <= max_length_to_search_as_prefix &&
                             strcmp(str.substr(0, strlen(sc_substr)).c_str(), sc_substr) == 0;
    sc_bool const isSubstr =
        strlen(sc_substr) > max_length_to_search_as_prefix && str.find(sc_substr) != std::string::npos;
    if (isPrefix || isSubstr)
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

      ScAddrsVector addrs = StringBufferToData(value);

      foundAddrs.insert(foundAddrs.cend(), addrs.cbegin(), addrs.cend());
    }

    it->Next();
  }

  *result_count = sc_uint32(foundAddrs.size());

  size_t const resultBytes = sizeof(sc_addr) * foundAddrs.size();
  *result = (sc_addr *)malloc(resultBytes);
  sc_mem_cpy(*result, foundAddrs.data(), resultBytes);

  return SC_TRUE;
}

sc_bool sc_rocksdb_fs_storage_find_sc_links_contents_by_content_substr(
    const sc_char * sc_substr,
    sc_char *** result,
    sc_uint32 * result_count,
    sc_uint32 max_length_to_search_as_prefix)
{
  MutexGuard lock(gMutex);

  assert(gDBInstance);

  rocksdb::ReadOptions readOptions;
  auto const & it = gDBInstance->NewIterator(readOptions);
  it->SeekToFirst();

  std::vector<std::string> foundStrings;
  while (it->Valid())
  {
    auto const & slice = it->value();

    std::string str{slice.data()};
    str.resize(slice.size());

    sc_bool const isPrefix = strlen(sc_substr) <= max_length_to_search_as_prefix &&
                             strcmp(str.substr(0, strlen(sc_substr)).c_str(), sc_substr) == 0;
    sc_bool const isSubstr =
        strlen(sc_substr) > max_length_to_search_as_prefix && str.find(sc_substr) != std::string::npos;
    if (isPrefix || isSubstr)
    {
      foundStrings.emplace_back(str);
    }

    it->Next();
  }

  *result_count = sc_uint32(foundStrings.size());

  size_t const resultBytes = sizeof(sc_char *) * foundStrings.size();
  *result = (sc_char **)malloc(resultBytes);
  for (size_t i = 0; i < foundStrings.size(); ++i)
  {
    std::string str = foundStrings[i];
    sc_str_cpy((*result)[i], str.c_str(), str.size());
  }

  return SC_TRUE;
}

sc_bool sc_rocksdb_fs_storage_append_sc_link_content(
    sc_element * element,
    sc_addr addr,
    sc_char * sc_string,
    sc_uint32 size)
{
  sc_char * current_string;
  sc_uint32 current_size = 0;
  sc_rocksdb_fs_storage_get_sc_link_content_ext(element, addr, &current_string, &current_size);

  if (current_size != 0 && current_string != null_ptr)
  {
    sc_check_sum * check_sum;
    sc_link_calculate_checksum(current_string, current_size, &check_sum);
    sc_rocksdb_fs_storage_addr_ref_remove(addr, check_sum);
  }

  sc_check_sum * check_sum;
  sc_link_calculate_checksum(sc_string, size, &check_sum);
  sc_rocksdb_fs_storage_addr_ref_append(addr, check_sum);

  sc_mem_cpy(element->content.data, check_sum->data, check_sum->len);
  sc_rocksdb_fs_storage_write_stream(check_sum, sc_string);

  return SC_TRUE;
}

sc_bool sc_rocksdb_fs_storage_get_sc_links_by_content(const sc_char * sc_string, sc_addr ** links, sc_uint32 * size)
{
  sc_check_sum * check_sum;
  sc_link_calculate_checksum(sc_string, strlen(sc_string), &check_sum);

  sc_result result = sc_rocksdb_fs_storage_find(check_sum, links, size);
  sc_mem_free(check_sum);

  return result;
}

void sc_rocksdb_fs_storage_get_sc_link_content_ext(
    sc_element * element,
    sc_addr addr,
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
    *size = strlen(data);
  }
  else
  {
    *sc_string = (sc_char *)null_ptr;
    *size = 0;
  }
}

sc_bool sc_rocksdb_fs_storage_remove_sc_link_content(sc_element * element, sc_addr addr)
{
  sc_char * string;
  sc_uint32 size = 0;
  sc_rocksdb_fs_storage_get_sc_link_content_ext(element, addr, &string, &size);

  if (size != 0 && string != null_ptr)
  {
    sc_check_sum * check_sum;
    sc_link_calculate_checksum(string, size, &check_sum);
    sc_rocksdb_fs_storage_addr_ref_remove(addr, check_sum);

    return SC_TRUE;
  }

  return SC_FALSE;
}

#endif
