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
};

#endif // _utils_h_
