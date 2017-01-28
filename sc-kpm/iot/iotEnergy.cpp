/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "iotKeynodes.hpp"

#include "sc-memory/cpp/sc_stream.hpp"

#include "iotEnergy.hpp"

namespace iot
{
struct UsedPowerSum
{
  UsedPowerSum(ScAddr const & groupAddr, ScMemoryContext & ctx)
    : mGroupAddr(groupAddr)
    , mMemoryCtx(ctx)
  {
  }

  float operator() ()
  {
    float result = 0.f;
    // calculate for child groups
    ScIterator3Ptr itGroups = mMemoryCtx.Iterator3(
          mGroupAddr,
          ScType::EdgeAccessConstPosPerm,
          ScType::NodeConstClass);

    while (itGroups->Next())
    {
      UsedPowerSum sum(itGroups->Get(2), mMemoryCtx);
      result += sum();
    }

    // calculate sum of used power for devices in this group (without childs)
    // search template:
    //		mGroupAddr _-> _device;;
    //		_device _=> nrel_real_energy_usage: _link;;

    /// TODO: use transitive relation to device set
    ScTemplate templDevice;
    templDevice
        .Triple(
          mGroupAddr,
          ScType::EdgeAccessConstPosPerm,
          ScType::NodeConstMaterial >> "device")
        .Triple(
          "device",
          ScType::EdgeDCommonConst >> "edge",
          ScType::Link >> "link")
        .Triple(
          Keynodes::device_real_energy_usage,
          ScType::EdgeAccessConstPosPerm,
          "edge");

    // Devices
    ScTemplateSearchResult searchResult;
    if (mMemoryCtx.HelperSearchTemplate(templDevice, searchResult))
    {
      size_t const resultNum = searchResult.Size();
      for (size_t i = 0; i < resultNum; ++i)
      {
        ScTemplateSearchResultItem const res = searchResult[i];

        /// TODO: implement support of different types (int, float, ...)
        ScAddr const & link = res["link"];
        ScStream stream;
        if (mMemoryCtx.GetLinkContent(link, stream) && (stream.Size() == sizeof(float)))
        {
          float value = 0.f;
          if (stream.ReadType(value))
            result += value;
        }
      }
    }

    // update value for a current group

    ScTemplate templPowerUsage;
    templPowerUsage
        .Triple(
          mGroupAddr,
          ScType::EdgeDCommonConst >> "edge",
          ScType::Link >> "link")
        .Triple(
          Keynodes::device_real_energy_usage,
          ScType::EdgeAccessConstPosPerm,
          "edge")
        .Triple(
          Keynodes::binary_float,
          ScType::EdgeAccessConstPosPerm,
          "link");

    ScAddr linkAddr;
    if (mMemoryCtx.HelperSearchTemplate(templPowerUsage, searchResult))
    {
      linkAddr = searchResult[0]["link"];
    }
    else
    {
      ScTemplateGenResult genResult;
      if (mMemoryCtx.HelperGenTemplate(templPowerUsage, genResult))
      {
        linkAddr = genResult["link"];
      }
    }

    if (linkAddr.IsValid())
    {
      ScStream stream((sc_char*)&result, sizeof(result), SC_STREAM_FLAG_READ);
      mMemoryCtx.SetLinkContent(linkAddr, stream);
    }

    return result;
  }

protected:
  ScAddr const & mGroupAddr;
  ScMemoryContext & mMemoryCtx;
};


SC_AGENT_ACTION_IMPLEMENTATION(AUpdateUsedPowerSum)
{
  // update all device groups, that in params
  ScIterator3Ptr itGroups = m_memoryCtx.Iterator3(
        requestAddr,
        ScType::EdgeAccessConstPosPerm,
        ScType::NodeConstClass);

  while (itGroups->Next())
  {
    UsedPowerSum sum(itGroups->Get(2), m_memoryCtx);
    sum();
  }

  return SC_RESULT_ERROR;
}

} // namespace iot
