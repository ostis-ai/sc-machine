/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2014 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

#ifndef _utils_h_
#define _utils_h_

#include "types.h"

class StringUtil
{
public:

    static void toLowerCase(String& str);
    static void toUpperCase(String &str);

    //! Check if string starts with specified pattern
    static bool startsWith(const String& str, const String& pattern, bool lowerCase);

    //! Check if string ends with specified pattern
    static bool endsWith(const String &str, const String &pattern, bool lowerCase);

    //! Split file name to path and name
    static void splitFilename(const String& qualifiedName, String& outBasename, String& outPath);

    //! Returns file extension
    static String getFileExtension(const String &filename);

    //! Make file path normalized
    static String normalizeFilePath(const String& init, bool makeLowerCase);

    //! Replace all substrings in string
    static const String replaceAll(const String& source, const String& replaceWhat, const String& replaceWithWhat);

};

#define PARSE_ERROR(source, line, description) {std::stringstream ss; ss << "Error: '" << (description) << "' in " << (source) << " at line " << (line); throw std::exception()};

#endif // _utils_h_
