#pragma once

#include "../sc_utils.hpp"

#include <memory>

// Got it there: https://github.com/mapsme/omim/blob/master/base/cache.hpp

namespace utils
{
// Simple cache that stores list of values.
template <typename KeyT, typename ValueT>
class Cache
{
  SC_DISALLOW_COPY(Cache);

public:
  Cache() = default;
  Cache(Cache && r) = default;

  explicit Cache(uint32_t logCacheSize)
  {
    Init(logCacheSize);
  }

  /// @param[in] logCacheSize is pow of two for number of elements in cache.
  void Init(uint32_t logCacheSize)
  {
    SC_ASSERT(logCacheSize > 0 && logCacheSize < 32, (logCacheSize));
    static_assert(
        (std::is_same<KeyT, uint32_t>::value || std::is_same<KeyT, uint64_t>::value ||
         std::is_same<KeyT, std::string>::value),
        "");

    m_cache.reset(new Data[uint64_t(1) << logCacheSize]);
    m_hashMask = uint32_t(1 << logCacheSize) - 1;

    Reset();
  }

  uint32_t GetCacheSize() const
  {
    return m_hashMask + 1;
  }

  // Find value by @key. If @key is found, returns reference to its value.
  // If @key is not found, some other key is removed and it's value is reused without
  // re-initialization. It's up to caller to re-initialize the new value if @found == false.
  // TODO: Return pair<ValueT *, bool> instead?
  ValueT & Find(KeyT const & key, bool & found)
  {
    Data & data = m_cache[Index(key)];
    if (data.m_Key == key)
    {
      found = true;
    }
    else
    {
      found = false;
      data.m_Key = key;
    }
    return data.m_Value;
  }

  inline void ResetValue(uint32_t i, uint32_t & value)
  {
    for (value = 0; Index(value) == i; ++value)
      ;
  }

  inline void ResetValue(uint32_t i, uint64_t & value)
  {
    for (value = 0; Index(value) == i; ++value)
      ;
  }

  inline void ResetValue(uint32_t i, std::string & value)
  {
    for (value = ""; Index(value) == i; value += " ")
      ;
  }

  template <typename F>
  void ForEachValue(F && f)
  {
    for (uint32_t i = 0; i <= m_hashMask; ++i)
      f(m_cache[i].m_Value);
  }

  void Reset()
  {
    // Initialize m_Cache such, that Index(m_Cache[i].m_Key) != i.
    for (uint32_t i = 0; i <= m_hashMask; ++i)
    {
      KeyT & key = m_cache[i].m_Key;
      ResetValue(i, key);
    }
  }

private:
  inline size_t Index(KeyT const & key) const
  {
    return static_cast<size_t>(Hash(key) & m_hashMask);
  }

  inline static uint32_t Hash(uint32_t x)
  {
    x = (x ^ 61) ^ (x >> 16);
    x = x + (x << 3);
    x = x ^ (x >> 4);
    x = x * 0x27d4eb2d;
    x = x ^ (x >> 15);
    return x;
  }

  inline static uint32_t Hash(uint64_t x)
  {
    return Hash(uint32_t(x) ^ uint32_t(x >> 32));
  }

  inline static uint32_t Hash(std::string const & x)
  {
    return uint32_t(std::hash<std::string>()(x));
  }

  // TODO: Consider using separate arrays for keys and values, to save on padding.
  struct Data
  {
    Data()
      : m_Key()
      , m_Value()
    {
    }
    KeyT m_Key;
    ValueT m_Value;
  };

  std::unique_ptr<Data[]> m_cache;
  uint32_t m_hashMask;
};

}  // namespace utils