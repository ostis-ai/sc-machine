/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_utils.hpp"


ScException::ScException(std::string const & description)
    : mDescription(description)
{

}

ScException::~ScException() throw()
{

}

const char* ScException::what() const throw()
{
    return mDescription.c_str();
}

namespace utils
{

	std::string StringUtils::replaceAll(std::string const & source, std::string const & replaceWhat, std::string const & replaceWithWhat)
	{
		std::string result = source;
		std::string::size_type pos = 0;
		while (1)
		{
			pos = result.find(replaceWhat, pos);
			if (pos == std::string::npos) break;
			result.replace(pos, replaceWhat.size(), replaceWithWhat);
			pos += replaceWithWhat.size();
		}
		return result;
	}

}