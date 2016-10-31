/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_types.hpp"
#include "sc_addr.hpp"
#include "sc_utils.hpp"

class ScStruct
{
public:
	_SC_EXTERN ScStruct(ScMemoryContext * ctx, ScAddr const & structAddr);

	/* Append element into sc-structure. If element already exist, then doesn't append it and return false; otherwise returns true. */
	_SC_EXTERN bool append(ScAddr const & elAddr);
	_SC_EXTERN bool append(ScAddr const & elAddr, ScAddr const & attrAddr);

	/* Remove element from sc-tructure and returns true, if element removed. */
	_SC_EXTERN bool remove(ScAddr const & elAddr);

	/* Operator equal to append */
	_SC_EXTERN ScStruct & operator << (ScAddr const & elAddr);
	_SC_EXTERN ScStruct & operator << (class ScTemplateGenResult const & res);
	/* Operator equal to remove */
	_SC_EXTERN ScStruct & operator >> (ScAddr const & elAddr);

	/* Check if specified element exist in structure */
	_SC_EXTERN bool hasElement(ScAddr const & elAddr) const;

	_SC_EXTERN ScAddr const & operator * () const;

	/* Check if structure has no elements */
	_SC_EXTERN bool isEmpty() const;

	/// TODO: implement +, -, == operators
private:
	ScAddr mAddr;
	ScMemoryContext * mContext;
};