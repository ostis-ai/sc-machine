/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc-memory/cpp/sc_memory.hpp"

#include <string>

namespace scp
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

/*! Resolves order role relation (1'-10') (relationAddr) of given arc (arcAddr)
 * @returns true, if relation resolved
 */
bool resolveOrderRoleRelation(ScMemoryContext & ctx, ScAddr const & arcAddr, ScAddr &relationAddr);

/*! Resolves atomic operator type (operatorType) of given operator (operatorAddr)
 * @returns true, if relation resolved
 */
bool resolveOperatorType(ScMemoryContext & ctx, ScAddr const &operatorAddr, ScAddr &operatorType);

/*! Prints system identifier or address of sc-element (elemAddr)
 */
void printSystemIdentifier(ScMemoryContext & ctx, ScAddr const & elemAddr);

/*! Prints semantic neighborhood of sc-element (elemAddr)
 */
void printInfo(ScMemoryContext & ctx, ScAddr const & elemAddr);

/*! Logs error about unknown type of given operator (addr)
 */
void logUnknownOperatorTypeError(ScMemoryContext & ctx, ScAddr &addr);

/*! Logs error with text (text) and element addr (addr)
 */
void logSCPError(ScMemoryContext & ctx, std::string text, ScAddr &addr);

}
}
