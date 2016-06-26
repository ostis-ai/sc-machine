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
    typedef std::pair<std::string, std::string> Property;

public:
    MetaDataManager(Cursor const & cursor);

	void Merge(MetaDataManager const & metaData);

    std::string GetProperty(std::string const & key) const;
    bool GetPropertySafe(std::string const & key, std::string & outValue) const;
    bool HasProperty(std::string const & key) const;
	void SetProperty(std::string const & key, std::string const & value);

    bool GetFlag(std::string const & key) const;
	size_t GetLineNumber() const;

    std::string GetNativeString(std::string const & key) const;
    
	/** Check if metadata is valid. If there are any errors, then throw an exciption */
	void Check() const;

protected:
    std::vector<Property> extractProperties(Cursor const & cursor) const;

private:
    
    std::unordered_map<std::string, std::string> m_properties;
	size_t m_lineNumber;

};