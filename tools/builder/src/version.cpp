#include "version.h"

#include <sstream>

std::string Version::getString()
{
    std::stringstream ss;
    ss << VERSION_ROOT << "." << VERSION_MAJOR << "." << VERSION_MINOR;
    return ss.str();
}
