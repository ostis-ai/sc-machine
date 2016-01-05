/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "iotTypes.hpp"

namespace sc
{
	class MemoryContext;
	class Addr;
}

namespace iot
{
	namespace Utils
	{

		/*! Adds element (elAddr) into set (setAddr).
		 * @returns If element already exists in set, then do nothing and returns false. Otherwise
		 * add element into set and returns true.
		 */
		bool addToSet(sc::MemoryContext & ctx, sc::Addr const & setAddr, sc::Addr const & elAddr);

		/*! Remove element (elAddr) from set (setAddr)
		 * @returns If element doesn't exist in set, then do nothing and returns false. Otherwise
		 * remove element from set and returns true.
		 */
		bool removeFromSet(sc::MemoryContext & ctx, sc::Addr const & setAddr, sc::Addr const & elAddr);

		/*! Add mass value (in grams) for a spceified object
		 * @param objAddr sc-addr of object to add mass
		 * @param valueAddr sc-addr of sc-link that contains mass value
		 */
		void setMass(sc::MemoryContext & ctx, sc::Addr const & objAddr, sc::Addr const & valueAddr);
	}
}