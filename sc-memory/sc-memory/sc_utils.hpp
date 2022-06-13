/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_debug.hpp"

#include <cstdint>
#include <exception>
#include <memory>
#include <string>

#include <cassert>
#include <memory.h>

// Got it there: https://github.com/mapsme/omim/blob/136f12af3adde05623008f71d07bb996fe5801a5/base/macros.hpp
#define ARRAY_SIZE(X) sizeof(::my::impl::ArraySize(X))

#define SC_DISALLOW_COPY(className) \
  className(className const &) = delete; \
  className & operator=(className const &) = delete

#define SC_DISALLOW_MOVE(className) \
  className(className &&) = delete; \
  className & operator=(className &&) = delete

#define SC_DISALLOW_COPY_AND_MOVE(className) \
  SC_DISALLOW_COPY(className); \
  SC_DISALLOW_MOVE(className)

// ---------------- Reference counter -----------
class RefCount
{
public:
  RefCount()
    : m_refCount(0)
  {
  }

  inline void Ref()
  {
    ++m_refCount;
  }

  inline uint32_t Unref()
  {
    SC_ASSERT(m_refCount > 0, ());
    --m_refCount;

    return m_refCount;
  }

private:
  uint32_t m_refCount;
};

#define SHARED_PTR_TYPE(__type) using __type##Ptr = std::shared_ptr<__type>;

class MemoryBuffer
{
public:
  MemoryBuffer(char * buff, unsigned int size)
    : m_data(buff)
    , m_size(size)
  {
  }

  inline bool IsValid() const
  {
    return m_data != nullptr;
  }

  void * Data()
  {
    return static_cast<void *>(m_data);
  }
  void const * CData() const
  {
    return static_cast<void const *>(m_data);
  }
  size_t Size() const
  {
    return m_size;
  }

  size_t Read(void * buff, size_t size) const
  {
    size_t const read = std::min(size, m_size);
    memcpy(buff, m_data, read);
    return read;
  }

protected:
  MemoryBuffer()
    : m_data(nullptr)
    , m_size(0)
  {
  }

  char * m_data;
  size_t m_size;
};

class MemoryBufferSafe : public MemoryBuffer
{
public:
  MemoryBufferSafe()
    : MemoryBuffer()
  {
  }

  MemoryBufferSafe(char const * buff, size_t size)
  {
    Reinit(buff, size);
  }

  ~MemoryBufferSafe()
  {
    Clear();
  }

  void Reinit(size_t size)
  {
    Clear();
    m_data = new char[size];
    m_size = size;
  }

  void Reinit(char const * buff, size_t size)
  {
    m_data = new char[size];
    m_size = size;
    memcpy(m_data, buff, size);
  }

  void Clear()
  {
    delete[] m_data;
    m_data = nullptr;
    m_size = 0;
  }
};

SHARED_PTR_TYPE(MemoryBuffer)
SHARED_PTR_TYPE(MemoryBufferSafe)

namespace utils
{
namespace impl
{
template <typename T>
inline T toNumber(std::string const & value);

template <>
inline int8_t toNumber<int8_t>(std::string const & value)
{
  return int8_t(std::stol(value));
}
template <>
inline int16_t toNumber<int16_t>(std::string const & value)
{
  return int16_t(std::stol(value));
}
template <>
inline int32_t toNumber<int32_t>(std::string const & value)
{
  return int32_t(std::stol(value));
}
template <>
inline int64_t toNumber<int64_t>(std::string const & value)
{
  return int64_t(std::stoll(value));
}
template <>
inline uint8_t toNumber<uint8_t>(std::string const & value)
{
  return uint8_t(std::stoul(value));
}
template <>
inline uint16_t toNumber<uint16_t>(std::string const & value)
{
  return uint16_t(std::stoul(value));
}
template <>
inline uint32_t toNumber<uint32_t>(std::string const & value)
{
  return uint32_t(std::stoul(value));
}
template <>
inline uint64_t toNumber<uint64_t>(std::string const & value)
{
  return uint64_t(std::stoull(value));
}
template <>
inline float toNumber<float>(std::string const & value)
{
  return std::stof(value);
}
template <>
inline double toNumber<double>(std::string const & value)
{
  return std::stod(value);
}

}  // namespace impl

class StringUtils
{
public:
  _SC_EXTERN static void ToLowerCase(std::string & str);
  _SC_EXTERN static void ToUpperCase(std::string & str);

  _SC_EXTERN static bool StartsWith(std::string const & str, std::string const & pattern, bool lowerCase = true);
  _SC_EXTERN static bool EndsWith(std::string const & str, std::string const & pattern, bool lowerCase = true);

  _SC_EXTERN static void SplitFilename(
      std::string const & qualifiedName,
      std::string & outBasename,
      std::string & outPath);
  _SC_EXTERN static void SplitString(std::string const & str, char delim, std::vector<std::string> & outList);

  _SC_EXTERN static void TrimLeft(std::string & str);
  _SC_EXTERN static void TrimRight(std::string & str);
  _SC_EXTERN static void Trim(std::string & str);

  _SC_EXTERN static std::string GetFileExtension(std::string const & filename);
  _SC_EXTERN static std::string NormalizeFilePath(std::string const & init, bool makeLowerCase);

  _SC_EXTERN static std::string ReplaceAll(
      std::string const & source,
      std::string const & replaceWhat,
      std::string const & replaceWithWhat);
  template <typename T>
  static bool ParseNumber(std::string const & value, T & outValue)
  {
    try
    {
      outValue = impl::toNumber<T>(value);
      return true;
    }
    catch (std::exception const &)
    {
    }

    return false;
  }
};

class Random
{
public:
  _SC_EXTERN static int Int();
};

}  // namespace utils
