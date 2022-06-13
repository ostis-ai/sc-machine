/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_link.hpp"

ScLink::ScLink(ScMemoryContext & ctx, ScAddr const & addr)
  : m_ctx(ctx)
  , m_addr(addr)
{
}

bool ScLink::IsValid() const
{
  return m_ctx.GetElementType(m_addr).IsLink();
}

ScLink::Type ScLink::DetermineType() const
{
  ScAddr typeEdge, typeAddr;
  _DetermineTypeEdgeImpl(typeEdge, typeAddr);

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

bool ScLink::_DetermineTypeEdgeImpl(ScAddr & outEdge, ScAddr & outType) const
{
  // set type
  ScTemplate templ;
  templ.Triple(ScKeynodes::kBinaryType, ScType::EdgeAccessVarPosPerm, ScType::NodeVarClass >> "_type");

  templ.Triple("_type", ScType::EdgeAccessVarPosTemp >> "_edge", m_addr);

  ScTemplateSearchResult res;
  if (m_ctx.HelperSearchTemplate(templ, res))
  {
    SC_ASSERT(res.Size() == 1, ("Invalid state of knowledge base"));
    outType = res[0]["_type"];
    outEdge = res[0]["_edge"];
    return true;
  }

  return false;
}
