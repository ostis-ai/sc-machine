/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory.hpp"

#include "sc_stream.hpp"

template <typename TContentType>
bool ScMemoryContext::SetLinkContent(
    ScAddr const & linkAddr,
    TContentType const & linkContent,
    bool isSearchableLinkContent) noexcept(false)
{
  return SetLinkContent(linkAddr, ScStreamMakeRead(linkContent), isSearchableLinkContent);
}

template <typename TContentType>
bool ScMemoryContext::GetLinkContent(ScAddr const & linkAddr, TContentType & outLinkContent) noexcept(false)
{
  std::string linkContent;
  ScStreamPtr const & linkContentStream = GetLinkContent(linkAddr);
  if (linkContentStream != nullptr && linkContentStream->IsValid()
      && ScStreamConverter::StreamToString(linkContentStream, linkContent))
  {
    std::istringstream streamString{linkContent};
    streamString >> outLinkContent;

    return true;
  }

  return true;
}

template <typename TContentType>
ScAddrSet ScMemoryContext::SearchLinksByContent(TContentType const & linkContent) noexcept(false)
{
  return SearchLinksByContent(ScStreamMakeRead(linkContent));
}

template <typename TContentType>
ScAddrVector ScMemoryContext::FindLinksByContent(TContentType const & linkContent) noexcept(false)
{
  ScAddrSet const & linkSet = SearchLinksByContent(linkContent);
  return {linkSet.cbegin(), linkSet.cend()};
}

template <typename TContentType>
ScAddrSet ScMemoryContext::SearchLinksByContentSubstring(
    TContentType const & linkContentSubstring,
    size_t maxLengthToSearchAsPrefix) noexcept(false)
{
  return SearchLinksByContentSubstring(ScStreamMakeRead(linkContentSubstring), maxLengthToSearchAsPrefix);
}

template <typename TContentType>
ScAddrVector ScMemoryContext::FindLinksByContentSubstring(
    TContentType const & linkContentSubstring,
    size_t maxLengthToSearchAsPrefix) noexcept(false)
{
  ScAddrSet const & linkSet = SearchLinksByContentSubstring(linkContentSubstring, maxLengthToSearchAsPrefix);
  return {linkSet.cbegin(), linkSet.cend()};
}

template <typename TContentType>
std::set<std::string> ScMemoryContext::SearchLinksContentsByContentSubstring(
    TContentType const & linkContentSubstring,
    size_t maxLengthToSearchAsPrefix) noexcept(false)
{
  return SearchLinksContentsByContentSubstring(ScStreamMakeRead(linkContentSubstring), maxLengthToSearchAsPrefix);
}

template <typename TContentType>
std::vector<std::string> ScMemoryContext::FindLinksContentsByContentSubstring(
    TContentType const & linkContentSubstring,
    size_t maxLengthToSearchAsPrefix) noexcept(false)
{
  std::set<std::string> const & linkContentSet =
      SearchLinksContentsByContentSubstring(ScStreamMakeRead(linkContentSubstring), maxLengthToSearchAsPrefix);
  return {linkContentSet.cbegin(), linkContentSet.cend()};
}

template <typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4, typename ParamType5>
std::shared_ptr<ScIterator5<ParamType1, ParamType2, ParamType3, ParamType4, ParamType5>> ScMemoryContext::
    CreateIterator5(
        ParamType1 const & param1,
        ParamType2 const & param2,
        ParamType3 const & param3,
        ParamType4 const & param4,
        ParamType5 const & param5)
{
  return std::shared_ptr<ScIterator5<ParamType1, ParamType2, ParamType3, ParamType4, ParamType5>>(
      new ScIterator5<ParamType1, ParamType2, ParamType3, ParamType4, ParamType5>(
          *this, param1, param2, param3, param4, param5));
}

template <typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4, typename ParamType5>
std::shared_ptr<ScIterator5<ParamType1, ParamType2, ParamType3, ParamType4, ParamType5>> ScMemoryContext::Iterator5(
    ParamType1 const & param1,
    ParamType2 const & param2,
    ParamType3 const & param3,
    ParamType4 const & param4,
    ParamType5 const & param5)
{
  return CreateIterator5(param1, param2, param3, param4, param5);
}

template <typename ParamType1, typename ParamType2, typename ParamType3>
std::shared_ptr<ScIterator3<ParamType1, ParamType2, ParamType3>> ScMemoryContext::CreateIterator3(
    ParamType1 const & param1,
    ParamType2 const & param2,
    ParamType3 const & param3)
{
  return std::shared_ptr<ScIterator3<ParamType1, ParamType2, ParamType3>>(
      new ScIterator3<ParamType1, ParamType2, ParamType3>(*this, param1, param2, param3));
}

template <typename ParamType1, typename ParamType2, typename ParamType3>
std::shared_ptr<ScIterator3<ParamType1, ParamType2, ParamType3>> ScMemoryContext::Iterator3(
    ParamType1 const & param1,
    ParamType2 const & param2,
    ParamType3 const & param3)
{
  return CreateIterator3(param1, param2, param3);
}

template <typename ParamType1, typename ParamType2, typename ParamType3, typename TripleCallback>
void ScMemoryContext::ForEach(
    ParamType1 const & param1,
    ParamType2 const & param2,
    ParamType3 const & param3,
    TripleCallback && callback)
{
  ScIterator3Ptr it = CreateIterator3(param1, param2, param3);
  while (it->Next())
    callback(it->Get(0), it->Get(1), it->Get(2));
}

template <typename ParamType1, typename ParamType2, typename ParamType3, typename TripleCallback>
void ScMemoryContext::ForEachIter3(
    ParamType1 const & param1,
    ParamType2 const & param2,
    ParamType3 const & param3,
    TripleCallback && callback)
{
  ForEach(param1, param2, param3, callback);
}

template <
    typename ParamType1,
    typename ParamType2,
    typename ParamType3,
    typename ParamType4,
    typename ParamType5,
    typename QuintupleCallback>
void ScMemoryContext::ForEach(
    ParamType1 const & param1,
    ParamType2 const & param2,
    ParamType3 const & param3,
    ParamType4 const & param4,
    ParamType5 const & param5,
    QuintupleCallback && callback)
{
  ScIterator5Ptr it = CreateIterator5(param1, param2, param3, param4, param5);
  while (it->Next())
    callback(it->Get(0), it->Get(1), it->Get(2), it->Get(3), it->Get(4));
}

template <
    typename ParamType1,
    typename ParamType2,
    typename ParamType3,
    typename ParamType4,
    typename ParamType5,
    typename QuintupleCallback>
void ScMemoryContext::ForEachIter5(
    ParamType1 const & param1,
    ParamType2 const & param2,
    ParamType3 const & param3,
    ParamType4 const & param4,
    ParamType5 const & param5,
    QuintupleCallback && callback)
{
  ForEach(param1, param2, param3, param4, param5, callback);
}
