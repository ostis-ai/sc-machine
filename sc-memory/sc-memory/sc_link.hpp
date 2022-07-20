/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_memory.hpp"
#include "sc_stream.hpp"
#include "sc_keynodes.hpp"
#include "sc_template.hpp"

#include <cstring>

/* This class wraps specified sc-link and provide functionality
 * to work with it. For example: get/set content, check content type
 */
class ScLink
{
public:
  enum class Type : uint8_t
  {
    Unknown,
    String,
    Float,
    Double,
    Int8,
    Int16,
    Int32,
    Int64,
    UInt8,
    UInt16,
    UInt32,
    UInt64,
    Custom
  };

  _SC_EXTERN ScLink(ScMemoryContext & ctx, ScAddr const & addr);

  // Check if this class has reference to sc-link element
  bool IsValid() const;

  template <typename Type>
  inline ScAddr const & Type2Addr() const;
  template <typename Type>
  inline void Value2Stream(Type const & value, ScStreamPtr & stream) const
  {
    std::stringstream stringStream;
    stringStream << value;
    std::string const str = stringStream.str();

    auto * copy = (sc_char *)calloc(str.size(), sizeof(sc_char));
    memcpy(copy, str.c_str(), str.size());

    stream.reset(new ScStream(copy, str.size(), SC_STREAM_FLAG_READ | SC_STREAM_FLAG_SEEK));
  }

  template <typename Type>
  inline bool Stream2Value(ScStreamPtr const & stream, Type & outValue) const
  {
    size_t size = stream->Size();

    size_t readBytes = 0;
    std::string str;
    str.resize(size);
    stream->Read((sc_char *)str.c_str(), size, readBytes);

    if (size != readBytes)
      return false;

    std::stringstream streamString(str);
    streamString >> outValue;

    return true;
  }

  template <typename Type>
  inline bool IsType() const
  {
    return m_ctx.HelperCheckEdge(Type2Addr<Type>(), m_addr, ScType::EdgeAccessConstPosTemp);
  }

  template <typename Type>
  inline bool Set(Type const & value)
  {
    ScStreamPtr stream;
    Value2Stream(value, stream);
    if (!m_ctx.SetLinkContent(m_addr, stream))
      return false;

    ScAddr const newType = Type2Addr<Type>();
    bool needAppend = true;
    ScAddr typeEdge, typeAddr;
    if (_DetermineTypeEdgeImpl(typeEdge, typeAddr))
    {
      if (typeAddr == newType)
        needAppend = false;
      else
        m_ctx.EraseElement(typeEdge);
    }

    // append into set
    if (needAppend)
      return m_ctx.CreateEdge(ScType::EdgeAccessConstPosTemp, newType, m_addr).IsValid();

    return true;
  }

  template <typename Type>
  Type Get() const
  {
    ScStreamPtr const stream = m_ctx.GetLinkContent(m_addr);

    // Check for empty content.
    if (!stream)
      return Type();

    Type result;
    if (!Stream2Value(stream, result))
    {
      SC_THROW_EXCEPTION(utils::ExceptionCritical, "Failed to get the value of " + std::to_string(m_addr.Hash()));
    }

    return result;
  }

  _SC_EXTERN Type DetermineType() const;
  _SC_EXTERN std::string GetAsString() const;

protected:
  _SC_EXTERN bool _DetermineTypeEdgeImpl(ScAddr & outEdge, ScAddr & outType) const;

private:
  ScMemoryContext & m_ctx;
  ScAddr m_addr;
};

template <>
inline ScAddr const & ScLink::Type2Addr<std::string>() const
{
  return ScKeynodes::kBinaryString;
}
template <>
inline ScAddr const & ScLink::Type2Addr<float>() const
{
  return ScKeynodes::kBinaryFloat;
}
template <>
inline ScAddr const & ScLink::Type2Addr<double>() const
{
  return ScKeynodes::kBinaryDouble;
}
template <>
inline ScAddr const & ScLink::Type2Addr<int8_t>() const
{
  return ScKeynodes::kBinaryInt8;
}
template <>
inline ScAddr const & ScLink::Type2Addr<int16_t>() const
{
  return ScKeynodes::kBinaryInt16;
}
template <>
inline ScAddr const & ScLink::Type2Addr<int32_t>() const
{
  return ScKeynodes::kBinaryInt32;
}
template <>
inline ScAddr const & ScLink::Type2Addr<int64_t>() const
{
  return ScKeynodes::kBinaryInt64;
}
template <>
inline ScAddr const & ScLink::Type2Addr<uint8_t>() const
{
  return ScKeynodes::kBinaryUInt8;
}
template <>
inline ScAddr const & ScLink::Type2Addr<uint16_t>() const
{
  return ScKeynodes::kBinaryUInt16;
}
template <>
inline ScAddr const & ScLink::Type2Addr<uint32_t>() const
{
  return ScKeynodes::kBinaryUInt32;
}
template <>
inline ScAddr const & ScLink::Type2Addr<uint64_t>() const
{
  return ScKeynodes::kBinaryUInt64;
}
template <>
inline ScAddr const & ScLink::Type2Addr<ScStreamPtr>() const
{
  return ScKeynodes::kBinaryCustom;
}

template <>
inline void ScLink::Value2Stream<std::string>(std::string const & value, ScStreamPtr & stream) const
{
  stream.reset(new ScStream((sc_char *)value.c_str(), value.size(), SC_STREAM_FLAG_READ | SC_STREAM_FLAG_SEEK));
}

template <>
inline void ScLink::Value2Stream<ScStreamPtr>(ScStreamPtr const & value, ScStreamPtr & stream) const
{
  stream = value;
}

template <>
inline bool ScLink::Stream2Value<std::string>(ScStreamPtr const & stream, std::string & outValue) const
{
  std::vector<uint8_t> buff(stream->Size());
  size_t readBytes = 0;
  stream->Read((sc_char *)buff.data(), buff.size(), readBytes);
  if (readBytes != buff.size())
    return false;

  outValue.assign(buff.begin(), buff.end());
  return true;
}

template <>
inline bool ScLink::Stream2Value<ScStreamPtr>(ScStreamPtr const & stream, ScStreamPtr & outValue) const
{
  outValue = stream;
  return true;
}
