/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_struct.hpp"
#include "sc_memory.hpp"
#include "sc_template.hpp"

ScStruct::ScStruct(ScMemoryContext * ctx, ScAddr const & structAddr)
    : mAddr(structAddr)
    , mContext(ctx)

{
}

bool ScStruct::append(ScAddr const & elAddr)
{
	check_expr(mContext);
	if (!hasElement(elAddr))
		return mContext->createEdge(sc_type_arc_pos_const_perm, mAddr, elAddr).isValid();

	return false;
}

bool ScStruct::append(ScAddr const & elAddr, ScAddr const & attrAddr)
{
	check_expr(mContext);
	if (!hasElement(elAddr))
	{
		ScAddr const edge = mContext->createEdge(sc_type_arc_pos_const_perm, mAddr, elAddr);
		if (edge.isValid())
		{
			ScAddr const edge2 = mContext->createEdge(sc_type_arc_pos_const_perm, attrAddr, edge);
			if (edge2.isValid())
				return true;

			// cleanup
			mContext->eraseElement(edge);
		}
	}

	return false;
}

bool ScStruct::remove(ScAddr const & elAddr)
{
	check_expr(mContext);
	bool found = false;
	ScIterator3Ptr iter = mContext->iterator3(mAddr, SC_TYPE(sc_type_arc_pos_const_perm), elAddr);
	while (iter->next())
	{
		mContext->eraseElement(iter->value(1));
		found = true;
	}

	return found;
}

bool ScStruct::hasElement(ScAddr const & elAddr) const
{
	check_expr(mContext);
	return mContext->helperCheckArc(mAddr, elAddr, sc_type_arc_pos_const_perm);
}

ScStruct & ScStruct::operator << (ScAddr const & elAddr)
{
	append(elAddr);
	return *this;
}

ScStruct & ScStruct::operator << (ScTemplateGenResult const & res)
{
	size_t const res_num = res.getSize();
	for (size_t i = 0; i < res_num; ++i)
		append(res.mResult[i]);

	return *this;
}

ScStruct & ScStruct::operator >> (ScAddr const & elAddr)
{
	remove(elAddr);
	return *this;
}

ScAddr const & ScStruct::operator * () const
{
	return mAddr;
}

bool ScStruct::isEmpty() const
{
	check_expr(mContext);
	ScIterator3Ptr iter = mContext->iterator3(mAddr, SC_TYPE(sc_type_arc_pos_const_perm), SC_TYPE(0));
	return !iter->next();
}
