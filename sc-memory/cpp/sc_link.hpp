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
    UInt64
  };

  _SC_EXTERN ScLink(ScMemoryContext & ctx, ScAddr const & addr);

  // Check if this class has reference to sc-link element
  bool IsValid() const;

  template <typename Type> ScAddr const & Type2Addr() const;
  template <typename Type> void Value2Stream(Type const & value, ScStreamPtr & stream) const
  {
    stream = new ScStream((sc_char*)&value, sizeof(value), SC_STREAM_FLAG_READ | SC_STREAM_FLAG_SEEK);
  }

  template <typename Type> bool Stream2Value(ScStream & stream, Type & outValue) const
  {
    if (stream.Size() != sizeof(Type))
      return false;

    size_t readBytes = 0;
    stream.Read((sc_char*)&outValue, sizeof(Type), readBytes);
    if (sizeof(Type) != readBytes)
      return false;

    return true;
  }

  template <typename Type>
  bool IsType() const
  {
    return m_ctx.HelperCheckEdge(Type2Addr<Type>(), m_addr, ScType::EdgeAccessConstPosTemp);
  }

  template <typename Type>
  bool Set(Type const & value)
  {
    ScStreamPtr stream;
    Value2Stream(value, stream);
    if (!m_ctx.SetLinkContent(m_addr, *stream))
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
    if (!IsType<Type>())
    {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidParams,
                         "You use incorrect type. Use IsType<>() to check it");
    }
    ScStream stream;
    Type result;
    if (!m_ctx.GetLinkContent(m_addr, stream) || !Stream2Value(stream, result))
    {
      SC_THROW_EXCEPTION(utils::ExceptionCritical,
                         "Error to get value of " + std::to_string(m_addr.Hash()));
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


template <> ScAddr const & ScLink::Type2Addr<std::string>() const { return ScKeynodes::kBinaryString; }
template <> ScAddr const & ScLink::Type2Addr<float>() const { return ScKeynodes::kBinaryFloat; }
template <> ScAddr const & ScLink::Type2Addr<double>() const { return ScKeynodes::kBinaryDouble; }
template <> ScAddr const & ScLink::Type2Addr<int8_t>() const { return ScKeynodes::kBinaryInt8; }
template <> ScAddr const & ScLink::Type2Addr<int16_t>() const { return ScKeynodes::kBinaryInt16; }
template <> ScAddr const & ScLink::Type2Addr<int32_t>() const { return ScKeynodes::kBinaryInt32; }
template <> ScAddr const & ScLink::Type2Addr<int64_t>() const { return ScKeynodes::kBinaryInt64; }
template <> ScAddr const & ScLink::Type2Addr<uint8_t>() const { return ScKeynodes::kBinaryUInt8; }
template <> ScAddr const & ScLink::Type2Addr<uint16_t>() const { return ScKeynodes::kBinaryUInt16; }
template <> ScAddr const & ScLink::Type2Addr<uint32_t>() const { return ScKeynodes::kBinaryUInt32; }
template <> ScAddr const & ScLink::Type2Addr<uint64_t>() const { return ScKeynodes::kBinaryUInt64; }

template <> void ScLink::Value2Stream<std::string>(std::string const & value, ScStreamPtr & stream) const
{
  stream = new ScStream((sc_char*)value.c_str(), value.size(), SC_STREAM_FLAG_READ | SC_STREAM_FLAG_SEEK);
}

template <> bool ScLink::Stream2Value<std::string>(ScStream & stream, std::string & outValue) const
{
  std::vector<uint8_t> buff(stream.Size());
  size_t readBytes = 0;
  stream.Read((sc_char*)buff.data(), buff.size(), readBytes);
  if (readBytes != buff.size())
    return false;

  outValue.assign(buff.begin(), buff.end());
  return true;
}

