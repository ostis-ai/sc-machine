/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_addr.hpp"
#include "sc_type.hpp"

class ScTemplateTriple
{
  friend class ScTemplate;

public:
  SC_DISALLOW_COPY_AND_MOVE(ScTemplateTriple);

  using ScTemplateTripleItems = std::array<ScTemplateItem, 3>;

  ScTemplateTriple(
      ScTemplateItem const & param1,
      ScTemplateItem const & param2,
      ScTemplateItem const & param3,
      size_t index);

  [[nodiscard]] ScTemplateTripleItems const & GetValues() const;

  ScTemplateItem const & operator[](size_t index) const noexcept(false);

  /* Store original index in template. Because when perform search or generation
   * we sort triples in suitable for operation order.
   * Used to triple result
   */
  size_t m_index;

protected:
  ScTemplateTripleItems m_values;
};
