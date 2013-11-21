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

    //! Make file path normalized
    static String normalizeFilePath(const String& init, bool makeLowerCase);
};

#endif // _utils_h_
