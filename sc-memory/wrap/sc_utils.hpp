/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <exception>
#include <string>
#include <assert.h>

namespace sc
{

class Exception : public std::exception
{
public:
    Exception(std::string const & description);
    virtual ~Exception() throw();

    const char* what() const throw();

private:
    std::string mDescription;
};

}

#define check(x) assert(x)
#define error(str) { throw sc::Exception(str); }

