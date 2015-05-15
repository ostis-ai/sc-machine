/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_utils.hpp"

namespace sc
{

Exception::Exception(std::string const & description)
    : mDescription(description)
{

}

Exception::~Exception() throw()
{

}

const char* Exception::what() const throw()
{
    return mDescription.c_str();
}


}
