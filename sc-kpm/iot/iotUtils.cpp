/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "iotUtils.hpp"
#include "iotKeynodes.hpp"

#include "sc-memory/cpp/sc_memory.hpp"
#include "sc-memory/cpp/sc_stream.hpp"

namespace iot
{
namespace utils
{

bool addToSet(ScMemoryContext & ctx, ScAddr const & setAddr, ScAddr const & elAddr)
{
  if (ctx.HelperCheckEdge(setAddr, elAddr, ScType::EdgeAccessConstPosPerm))
    return false;

  ScAddr const edgeAddr = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, setAddr, elAddr);
  SC_ASSERT(edgeAddr.IsValid(), ());
  return true;
}

bool removeFromSet(ScMemoryContext & ctx, ScAddr const & setAddr, ScAddr const & elAddr)
{
  ScIterator3Ptr it = ctx.Iterator3(setAddr, ScType::EdgeAccessConstPosPerm, elAddr);
  bool result = false;
  while (it->Next())
    result = result || ctx.EraseElement(it->Get(1));

  return result;
}

void setMass(ScMemoryContext & ctx, ScAddr const & objAddr, ScAddr const & valueAddr)
{
  ScAddr massAddr;

  ScIterator5Ptr itMass = ctx.Iterator5(
        objAddr,
        ScType::EdgeDCommonConst,
        ScType::NodeConstAbstract,
        ScType::EdgeAccessConstPosPerm,
        Keynodes::nrel_mass);

  /// TODO: check if there is a more than one result
  if (itMass->Next())
  {
    massAddr = itMass->Get(2);
  }
  else
  {
    massAddr = ctx.CreateNode(ScType::NodeConstAbstract);
    SC_ASSERT(massAddr.IsValid(), ());
    ScAddr const edgeCommon = ctx.CreateEdge(ScType::EdgeDCommonConst, objAddr, massAddr);
    SC_ASSERT(edgeCommon.IsValid(), ());
    ScAddr const edge = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, Keynodes::nrel_mass, edgeCommon);
    SC_ASSERT(edge.IsValid(), ());
  }

  ScIterator5Ptr itValue = ctx.Iterator5(
        massAddr,
        ScType::EdgeAccessConstPosPerm,
        ScType::Link,
        ScType::EdgeAccessConstPosPerm,
        Keynodes::rrel_gram);

  if (itValue->Next())
  {
    ScAddr const linkAddr = itValue->Get(2);
    ScStream stream;
    bool res = ctx.GetLinkContent(valueAddr, stream);
    SC_ASSERT(res, ());
    res = ctx.SetLinkContent(linkAddr, stream);
    SC_ASSERT(res, ());
  }
  else
  {
    ScAddr edge = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, massAddr, valueAddr);
    SC_ASSERT(edge.IsValid(), ());
    edge = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, Keynodes::rrel_gram, edge);
    SC_ASSERT(edge.IsValid(), ());
  }

}


ScAddr findMainIdtf(ScMemoryContext & ctx, ScAddr const & elAddr, ScAddr const langAddr)
{
  SC_ASSERT(elAddr.IsValid(), ());
  SC_ASSERT(langAddr.IsValid(), ());

  ScAddr result;
  ScIterator5Ptr it5 = ctx.Iterator5(
        elAddr,
        ScType::EdgeDCommonConst,
        ScType::Link,
        ScType::EdgeAccessConstPosPerm,
        Keynodes::nrel_main_idtf);

  while (it5->Next())
  {
    if (ctx.HelperCheckEdge(langAddr, it5->Get(2), ScType::EdgeAccessConstPosPerm))
    {
      result = it5->Get(2);
      break;
    }
  }

  return result;
}

} // namespace utils
} // namespace iot
