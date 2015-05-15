/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
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
