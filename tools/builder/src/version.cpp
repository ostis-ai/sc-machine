/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "version.h"

#include <sstream>

std::string Version::getString()
{
    std::stringstream ss;
    ss << VERSION_ROOT << "." << VERSION_MAJOR << "." << VERSION_MINOR;
    return ss.str();
}
