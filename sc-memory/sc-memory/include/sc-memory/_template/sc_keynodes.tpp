/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/sc_keynodes.hpp"

namespace
{

template <typename T>
struct IsValidTemplateItem
  : std::disjunction<
        std::is_base_of<ScKeynode, T>,
        std::is_same<T, ScTemplateItem>,
        std::is_same<T, ScType>,
        std::is_convertible<T, std::string_view>>
{
};

}  // namespace

template <typename T>
auto ScTemplateKeynode::HandleParam(T const & param) noexcept
{
  if constexpr (std::is_same<T, ScKeynode>::value)
  {
    return [&param]() -> ScKeynode
    {
      return param;
    };
  }
  else
  {
    return [param]() -> ScTemplateItem
    {
      return param;
    };
  }
}

template <typename T1, typename T2, typename T3>
ScTemplateKeynode ScTemplateKeynode::Triple(T1 const & param1, T2 const & param2, T3 const & param3) noexcept
{
  static_assert(IsValidTemplateItem<T1>::value, "Parameter 1 must be either ScKeynode or ScTemplateItem");
  static_assert(IsValidTemplateItem<T2>::value, "Parameter 2 must be either ScKeynode or ScTemplateItem");
  static_assert(IsValidTemplateItem<T3>::value, "Parameter 3 must be either ScKeynode or ScTemplateItem");

  m_constructionInitializers.push_back(
      [p1 = HandleParam(param1), p2 = HandleParam(param2), p3 = HandleParam(param3)](ScTemplate & templ)
      {
        templ.Triple(p1(), p2(), p3());
      });
  return std::move(*this);
}

template <typename T1, typename T2, typename T3, typename T4, typename T5>
ScTemplateKeynode ScTemplateKeynode::Quintuple(
    T1 const & param1,
    T2 const & param2,
    T3 const & param3,
    T4 const & param4,
    T5 const & param5) noexcept
{
  static_assert(IsValidTemplateItem<T1>::value, "Parameter 1 must be either ScKeynode or ScTemplateItem");
  static_assert(IsValidTemplateItem<T2>::value, "Parameter 2 must be either ScKeynode or ScTemplateItem");
  static_assert(IsValidTemplateItem<T3>::value, "Parameter 3 must be either ScKeynode or ScTemplateItem");
  static_assert(IsValidTemplateItem<T4>::value, "Parameter 4 must be either ScKeynode or ScTemplateItem");
  static_assert(IsValidTemplateItem<T5>::value, "Parameter 5 must be either ScKeynode or ScTemplateItem");

  m_constructionInitializers.push_back(
      [p1 = HandleParam(param1),
       p2 = HandleParam(param2),
       p3 = HandleParam(param3),
       p4 = HandleParam(param4),
       p5 = HandleParam(param5)](ScTemplate & templ)
      {
        templ.Quintuple(p1(), p2(), p3(), p4(), p5());
      });
  return std::move(*this);
}
