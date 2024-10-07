/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/sc_link.hpp"

ScLink::ScLink(ScMemoryContext & context, ScAddr const & linkAddr)
  : ScAddr(linkAddr)
  , m_context(&context)
{
}

bool ScLink::IsValid() const
{
  return m_context->GetElementType(*this).IsLink();
}

ScLink::Type ScLink::DetermineType() const
{
  ScAddr typeArcAddr, typeAddr;
  DetermineTypeArcImpl(typeArcAddr, typeAddr);

  if (typeAddr == Type2Addr<std::string>())
    return Type::String;

  if (typeAddr == Type2Addr<float>())
    return Type::Float;

  if (typeAddr == Type2Addr<double>())
    return Type::Double;

  if (typeAddr == Type2Addr<int8_t>())
    return Type::Int8;

  if (typeAddr == Type2Addr<int16_t>())
    return Type::Int16;

  if (typeAddr == Type2Addr<int32_t>())
    return Type::Int32;

  if (typeAddr == Type2Addr<int64_t>())
    return Type::Int64;

  if (typeAddr == Type2Addr<uint8_t>())
    return Type::UInt8;

  if (typeAddr == Type2Addr<uint16_t>())
    return Type::UInt16;

  if (typeAddr == Type2Addr<uint32_t>())
    return Type::UInt32;

  if (typeAddr == Type2Addr<uint64_t>())
    return Type::UInt64;

  if (typeAddr == Type2Addr<ScStreamPtr>())
    return Type::Custom;

  return Type::Unknown;
}

std::string ScLink::GetAsString() const
{
  Type const type = DetermineType();
  switch (type)
  {
  case Type::String:
    return Get<std::string>();

  case Type::Float:
    return std::to_string(Get<float>());

  case Type::Double:
    return std::to_string(Get<double>());

  case Type::Int8:
    return std::to_string((int)Get<int8_t>());

  case Type::Int16:
    return std::to_string(Get<int16_t>());

  case Type::Int32:
    return std::to_string(Get<int32_t>());

  case Type::Int64:
    return std::to_string(Get<int64_t>());

  case Type::UInt8:
    return std::to_string((int)Get<uint8_t>());

  case Type::UInt16:
    return std::to_string(Get<uint16_t>());

  case Type::UInt32:
    return std::to_string(Get<uint32_t>());

  case Type::UInt64:
    return std::to_string(Get<uint64_t>());

  case Type::Custom:
  {
    std::string value;
    ScStreamConverter::StreamToString(Get<ScStreamPtr>(), value);
    return value;
  }

  default:
    return "";
  }
}

bool ScLink::DetermineTypeArcImpl(ScAddr & outArcAddr, ScAddr & outTypeAddr) const
{
  // set type
  ScTemplate templ;
  templ.Triple(ScKeynodes::binary_type, ScType::VarPermPosArc, ScType::VarNodeClass >> "_type");

  templ.Triple("_type", ScType::VarTempPosArc >> "_arc", *this);

  ScTemplateSearchResult res;
  if (m_context->SearchByTemplate(templ, res))
  {
    outTypeAddr = res[0]["_type"];
    outArcAddr = res[0]["_arc"];
    return true;
  }

  return false;
}
