/* ----------------------------------------------------------------------------
** © 201x Austin Brunkhorst, All Rights Reserved.
**
** MetaDataManager.h
** --------------------------------------------------------------------------*/

#pragma once

#include "Cursor.hpp"

class ReflectionParser;

class MetaDataManager
{
public:
    MetaDataManager(const Cursor &cursor);

    std::string GetProperty(const std::string &key) const;
    bool GetFlag(const std::string &key) const;

    std::string GetNativeString(const std::string &key) const;

private:
    typedef std::pair<std::string, std::string> Property;

    std::unordered_map<std::string, std::string> m_properties;

    std::vector<Property> extractProperties(const Cursor &cursor) const;
};