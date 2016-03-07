/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_template.hpp"

template <> ScTemplateItem3::ScTemplateItem3(ScAddr const & param1, ScType const & param2, ScType const & param3)
{
	mValues[0].setAddr(param1);
	mValues[1].setType(param2);
	mValues[2].setType(param3);
}

template <> ScTemplateItem3::ScTemplateItem3(ScType const & param1, ScType const & param2, ScAddr const & param3)
{
	mValues[0].setType(param1);
	mValues[1].setType(param2);
	mValues[2].setAddr(param3);
}

template <> ScTemplateItem3::ScTemplateItem3(ScAddr const & param1, ScType const & param2, ScAddr const & param3)
{
	mValues[0].setAddr(param1);
	mValues[1].setType(param2);
	mValues[2].setAddr(param3);
}
