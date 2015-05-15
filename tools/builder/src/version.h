/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _version_h_
#define _version_h_

#include <string>

#define VERSION_ROOT    0
#define VERSION_MAJOR   1
#define VERSION_MINOR   0

class Version
{

    static std::string getString();
};

#endif // _version_h_
