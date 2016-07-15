#pragma once

#include "Types.hpp"


struct MacrosInfo
{
    typedef enum
    {
        MT_NONE,
        MT_CLASS,
        MT_PROPERTY,
        MT_BODY

    } Type;

    MacrosInfo()
        : m_type(MT_NONE)
        , m_line(0)
        , m_column(0)
    {

    }

    MacrosInfo(std::string const & name, std::string const & code, size_t line, size_t column, std::string const & filename)
        : m_name(name)
        , m_code(code)
        , m_line(line)
        , m_column(column)
        , m_fileName(filename)
    {
        if (m_name == "SC_CLASS")
            m_type = MT_CLASS;
        else if (m_name == "SC_PROPERTY")
            m_type = MT_PROPERTY;
        else if (m_name == "SC_GENERATED_BODY")
            m_type = MT_BODY;
        else
            m_type = MT_NONE;
    }

    bool IsValid() const
    {
        return (m_type != MT_NONE);
    }

    bool operator == (MacrosInfo const & other) const
    {
        return (m_name == other.m_name) && (m_fileName == other.m_fileName)
            && (m_line == other.m_line) && (m_column == other.m_column)
            && (m_type == other.m_type);
    }

    static bool RequestProcess(std::string const & name)
    {
        return (name == "SC_CLASS") || (name == "SC_PROPERTY") || (name == "SC_GENERATED_BODY");
    }


    std::string m_name;
    std::string m_code;
    Type m_type;
    
    // location
    size_t m_line;
    size_t m_column;
    std::string m_fileName;
};

class MacrosManager
{
public:
    MacrosManager();
    ~MacrosManager();

    bool AddMacros(MacrosInfo const & inMacros);
    bool FindMacros(size_t line, std::string const & fileName, MacrosInfo & outResult) const;

    void Clear();

private:
    bool FindMacrosInternal(MacrosInfo::Type type, size_t line, std::string const & fileName, MacrosInfo & outResult) const;

private:
    typedef std::map< size_t, MacrosInfo> tMacrosLineMap;
    typedef std::map< MacrosInfo::Type, tMacrosLineMap> tMacrosTypedMap;

    tMacrosTypedMap m_macrosStorage;
};
