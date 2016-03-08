/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "iotTypes.hpp"


class ScMemoryContext;
class ScAddr;


namespace iot
{
	namespace Utils
	{

		/*! Adds element (elAddr) into set (setAddr).
		 * @returns If element already exists in set, then do nothing and returns false. Otherwise
		 * add element into set and returns true.
		 */
        bool addToSet(ScMemoryContext & ctx, ScAddr const & setAddr, ScAddr const & elAddr);

		/*! Remove element (elAddr) from set (setAddr)
		 * @returns If element doesn't exist in set, then do nothing and returns false. Otherwise
		 * remove element from set and returns true.
		 */
        bool removeFromSet(ScMemoryContext & ctx, ScAddr const & setAddr, ScAddr const & elAddr);

		/*! Add mass value (in grams) for a spceified object
		 * @param objAddr sc-addr of object to add mass
		 * @param valueAddr sc-addr of sc-link that contains mass value
		 */
        void setMass(ScMemoryContext & ctx, ScAddr const & objAddr, ScAddr const & valueAddr);

		/*! Trying to find main identifier of specified element with specified language
		 */
        ScAddr findMainIdtf(ScMemoryContext & ctx, ScAddr const & elAddr, ScAddr const langAddr);
	}
}