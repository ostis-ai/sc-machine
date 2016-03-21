#include "MacrosManager.hpp"

MacrosManager::MacrosManager()
{

}

MacrosManager::~MacrosManager()
{

}

bool MacrosManager::AddMacros(MacrosInfo const & inMacros)
{
    tMacrosLineMap & lineMap = m_macrosStorage[inMacros.m_type];
    tMacrosLineMap::const_iterator it = lineMap.find(inMacros.m_line);
    if ((it != lineMap.end()) && (it->second == inMacros))
    {
        return false;
    } 
    else if (it == lineMap.end())
    {
        lineMap[inMacros.m_line] = inMacros;
    }

    return true;
}

bool MacrosManager::FindMacros(size_t line, std::string const & fileName, MacrosInfo & outResult) const
{
    MacrosInfo bodyMacros, propertyMacros, classMacros;
    FindMacrosInternal(MacrosInfo::MT_BODY, line, fileName, bodyMacros);
    FindMacrosInternal(MacrosInfo::MT_CLASS, line, fileName, classMacros);
    FindMacrosInternal(MacrosInfo::MT_PROPERTY, line, fileName, propertyMacros);

    size_t count = 0;
    count += bodyMacros.IsValid() ? 1 : 0;
    count += classMacros.IsValid() ? 1 : 0;
    count += propertyMacros.IsValid() ? 1 : 0;

    if (count > 1) // invalid state
    {
        std::cout << "More that one SC_... macros used at line " << line << " in " << fileName << std::endl;
    }
    else if (count == 1)
    {
        if (bodyMacros.IsValid())
            outResult = bodyMacros;
        else if (classMacros.IsValid())
            outResult = classMacros;
        else
            outResult = propertyMacros;

        assert(propertyMacros.IsValid());

        return true;
    }

    return false;
}

void MacrosManager::Clear()
{
    m_macrosStorage = tMacrosTypedMap();
}

bool MacrosManager::FindMacrosInternal(MacrosInfo::Type type, size_t line, std::string const & fileName, MacrosInfo & outResult) const
{
    tMacrosTypedMap::const_iterator itTyped = m_macrosStorage.find(type);
    if (itTyped != m_macrosStorage.end())
    {
        tMacrosLineMap const & lineMap = itTyped->second;
        tMacrosLineMap::const_iterator itLine = lineMap.find(line);
        if (itLine != lineMap.end())
        {
            outResult = itLine->second;
            return true;
        }
    }

    return false;
}