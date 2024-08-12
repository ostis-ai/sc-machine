/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_addr.hpp"

#include "sc_utils.hpp"

class ScSet : public ScAddr
{
  friend class ScAgentContext;

public:
  _SC_EXTERN ~ScSet();

  _SC_EXTERN ScSet(ScSet const & other);

  _SC_EXTERN ScSet & operator=(ScSet const &);

  /* Append element into sc-hash-map. If element already exist, then doesn't append it and return false; otherwise
   * returns true. */
  _SC_EXTERN bool Append(ScAddr const & elAddr);
  _SC_EXTERN bool Append(ScAddr const & elAddr, ScAddr const & attrAddr);

  /* Remove element from sc-structure and returns true, if element removed. */
  _SC_EXTERN bool Remove(ScAddr const & elAddr);

  /* Operator equal to append */
  _SC_EXTERN ScSet & operator<<(ScAddr const & elAddr);
  _SC_EXTERN ScSet & operator<<(class ScTemplateResultItem const & res);

  /* Operator equal to remove */
  _SC_EXTERN ScSet & operator>>(ScAddr const & elAddr);

  /* Check if specified element exist in set */
  _SC_EXTERN bool HasElement(ScAddr const & elAddr) const;

  /* Check if set has no elements */
  _SC_EXTERN bool IsEmpty() const;

  /// TODO: implement +, -, == operators

protected:
  _SC_EXTERN ScSet(class ScMemoryContext * ctx, ScAddr const & setAddr = ScAddr::Empty);

  _SC_EXTERN ScSet(ScMemoryContext & ctx, ScAddr const & setAddr = ScAddr::Empty);

private:
  ScMemoryContext * m_ctx;
};

class ScStruct : public ScSet
{
  friend class ScAgentContext;
  friend class ScTemplateLoader;

protected:
  _SC_EXTERN ScStruct(ScMemoryContext * ctx, ScAddr const & structAddr = ScAddr::Empty);

  _SC_EXTERN ScStruct(ScMemoryContext & ctx, ScAddr const & structAddr = ScAddr::Empty);
};
