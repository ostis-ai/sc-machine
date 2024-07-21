/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_action.hpp"

template <std::size_t N>
_SC_EXTERN auto ScAction::GetArguments()
{
  return GetArgumentsImpl(std::make_index_sequence<N>{});
}

template <std::size_t... Is>
_SC_EXTERN auto ScAction::GetArgumentsImpl(std::index_sequence<Is...>)
{
  return std::make_tuple(GetArgument(Is + 1)...);
}

template <class... TScAddr>
_SC_EXTERN ScAction & ScAction::SetArguments(TScAddr const &... arguments)
{
  static_assert(
      (std::is_base_of<ScAddr, TScAddr>::value && ...), "Each element of parameter pack must have ScAddr type.");

  size_t i = 0;
  for (ScAddr const & argumentAddr : ScAddrVector{arguments...})
    SetArgument(++i, argumentAddr);

  return *this;
}

template <class... TScAddr>
ScAction & ScAction::FormAnswer(TScAddr const &... addrs)
{
  static_assert(
      (std::is_base_of<ScAddr, TScAddr>::value && ...), "Each element of parameter pack must have ScAddr type.");

  for (ScAddr const & addr : ScAddrVector(addrs...))
    m_answer << addr;

  return *this;
}

template <class... TScAddr>
_SC_EXTERN ScAction & ScAction::UpdateAnswer(TScAddr const &... addrs)
{
  return FormAnswer(addrs...);
}
