/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

extern "C"
{
#include "sc_fm_engine.h"
#include "sc_stream_memory.h"
}

#include "rocksdb/db.h"

#include <iostream>
#include <mutex>
#include <vector>

extern "C"
{
#include "sc_fs_storage.h"
}

rocksdb::DB * gDBInstance = nullptr;
std::string gInstancePath;
std::mutex gMutex;

using AddrsVector = std::vector<sc_addr>;
using MutexGuard = std::lock_guard<std::mutex>;

namespace
{

bool CreateDBInstance()
{
  rocksdb::Options options;
  options.create_if_missing = true;
//  options.error_if_exists = true;

//  _sc_fm_remove_dir(gInstancePath.c_str());
  _sc_fs_mkdirs(gInstancePath.c_str());
  rocksdb::Status status = rocksdb::DB::Open(options, gInstancePath, &gDBInstance);
  if (!status.ok())
    std::cerr << "Can't initialize file memory: " << status.ToString() << std::endl;

  return status.ok();
}

void DestroyDBInstance()
{
  assert(gDBInstance);
  gDBInstance->SyncWAL();
//  gDBInstance->Close();
  delete gDBInstance;

  gDBInstance = nullptr;
}

std::string ChecksumToString(sc_check_sum const * check_sum)
{
  return std::string(check_sum->data, check_sum->data + check_sum->len);
}

std::string MakeAddrsKey(sc_check_sum const * check_sum)
{
  return ChecksumToString(check_sum) + "_addrs";
}

std::string MakeContentKey(sc_check_sum const * check_sum)
{
  return ChecksumToString(check_sum) + "_content";
}

std::string DataToStringBuffer(AddrsVector const & addrs)
{
  size_t const size = addrs.size();
  uint8_t const * pSize = (uint8_t const *)(&size);
  uint8_t const * pData = (uint8_t const *)(addrs.data());

  return std::string(pSize, pSize + sizeof(size)) + 
    std::string(pData, pData + sizeof(sc_addr) * size);
}

AddrsVector StringBufferToData(std::string const & data)
{
  size_t size = 0;
  assert(data.size() >= sizeof(size));

  uint8_t const * pData = (uint8_t const *)data.data();
  size = *((size_t const *)pData);

  sc_addr const * pAddrs = (sc_addr const *)(pData + sizeof(size));
  assert(data.size() == (sizeof(size) + sizeof(sc_addr) * size));

  return AddrsVector(pAddrs, pAddrs + size);
}

#define SC_RES(expr) (expr) ? SC_RESULT_OK : SC_RESULT_ERROR;

}

sc_result sc_fm_init(const sc_char * repo_path)
{
  MutexGuard lock(gMutex);
  gInstancePath = std::string (repo_path) + "/file_memory";
  return SC_RES(CreateDBInstance());
}

void sc_fm_free()
{
  MutexGuard lock(gMutex);
  DestroyDBInstance();
}

sc_stream * sc_fm_read_stream_new(const sc_check_sum * check_sum)
{
  MutexGuard lock(gMutex);

  assert(gDBInstance);
  std::string value;

  rocksdb::ReadOptions options;
  rocksdb::Status status = gDBInstance->Get(options, ChecksumToString(check_sum), &value);

  if (status.ok())
  {
    sc_char * data = (sc_char*)malloc(value.size());
    if (data == nullptr)
      return nullptr;

    memcpy(data, value.data(), value.size());
    sc_stream * result = sc_stream_memory_new(data, sc_uint(value.size()), SC_STREAM_FLAG_READ, SC_TRUE);
    return result;
  }

  return nullptr;
}

sc_result sc_fm_write_stream(const sc_check_sum * check_sum, const sc_stream * stream)
{
  MutexGuard lock(gMutex);

  // seek streem to start
  sc_stream_seek(stream, SC_STREAM_SEEK_SET, 0);

  sc_uint32 size;
  sc_result res = sc_stream_get_length(stream, &size);

  if (res != SC_RESULT_OK)
    return SC_RESULT_ERROR_IO;
  
    
  sc_char buffer[1024];
  std::string data;

  while (data.size() < size_t(size))
  {
    sc_uint32 readBytes = 0;
    if (sc_stream_read_data(stream, buffer, 1024, &readBytes) != SC_RESULT_OK)
      return SC_RESULT_ERROR_IO;

    data += std::string(buffer, buffer + readBytes);
  }

  assert(gDBInstance);
  rocksdb::WriteOptions options;
//  options.sync = true;
  rocksdb::Status status = gDBInstance->Put(options, MakeContentKey(check_sum), data);

  return SC_RES(status.ok());
}

sc_result sc_fm_addr_ref_append(sc_addr addr, const sc_check_sum *check_sum)
{
  MutexGuard lock(gMutex);

  assert(gDBInstance);

  std::string const key = MakeAddrsKey(check_sum);
  std::string value;  

  rocksdb::ReadOptions readOptions;
  rocksdb::Status status = gDBInstance->Get(readOptions, key, &value);

  AddrsVector addrs;

  if (status.ok())
    addrs = StringBufferToData(value);

  addrs.push_back(addr);

  rocksdb::WriteOptions writeOptions;
  status = gDBInstance->Put(writeOptions, key, DataToStringBuffer(addrs));

  return SC_RES(status.ok());
}

sc_result sc_fm_addr_ref_remove(sc_addr addr, const sc_check_sum *check_sum)
{
  MutexGuard lock(gMutex);

  assert(gDBInstance);

  std::string const key = MakeAddrsKey(check_sum);
  std::string value;  

  rocksdb::ReadOptions readOptions;
  rocksdb::Status status = gDBInstance->Get(readOptions, key, &value);

  if (!status.ok())
    return SC_RESULT_ERROR_IO;

  AddrsVector addrs = StringBufferToData(value);
  for (size_t i = 0; i < addrs.size(); ++i)
  {
    if (SC_ADDR_IS_EQUAL(addrs[i], addr))
    {
      addrs.erase(addrs.begin() + i);
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

sc_result sc_fm_find(const sc_check_sum *check_sum, sc_addr **result, sc_uint32 *result_count)
{
  MutexGuard lock(gMutex);

  assert(gDBInstance);
  
  std::string const key = MakeAddrsKey(check_sum);
  std::string value;  

  rocksdb::ReadOptions readOptions;
  rocksdb::Status status = gDBInstance->Get(readOptions, key, &value);

  if (!status.ok())
    return SC_RESULT_ERROR_IO;

  AddrsVector addrs = StringBufferToData(value);

  *result_count = sc_uint32(addrs.size());

  size_t const resultBytes = sizeof(sc_addr) * addrs.size();
  *result = (sc_addr*)malloc(resultBytes);
  memcpy(*result, addrs.data(), resultBytes);

  return SC_RESULT_OK;
}

sc_result sc_fm_clear()
{
  MutexGuard lock(gMutex);

  assert(gDBInstance);
  DestroyDBInstance();

  rocksdb::Options options;
  rocksdb::DestroyDB(gInstancePath, options).ok();

  return SC_RES(CreateDBInstance());
}

sc_result sc_fm_save()
{
  MutexGuard lock(gMutex);

  assert(gDBInstance);
  rocksdb::FlushOptions options;
  options.wait = true;
  
  rocksdb::Status status = gDBInstance->Flush(options);
  return SC_RES(status.ok());
}

sc_result sc_fm_clean_state()
{
  MutexGuard lock(gMutex);
  // TODO: implement me
  return SC_RESULT_OK;
}
