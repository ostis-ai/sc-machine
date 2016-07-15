#pragma once

#include "CursorType.hpp"

class Cursor
{
public:
    typedef std::vector<Cursor> List;
    typedef CXCursorVisitor Visitor;

    Cursor(const CXCursor &handle);

    CXCursorKind GetKind(void) const;

    Cursor GetLexicalParent(void) const;

    std::string GetSpelling(void) const;
    std::string GetDisplayName(void) const;
    std::string const & GetFileName() const;
    size_t GetLineNumber() const;

    bool IsDefinition(void) const;
    bool IsStatic(void) const;

    CX_CXXAccessSpecifier GetAccessModifier(void) const;
    //CX_StorageClass GetStorageClass(void) const;

    CursorType GetType(void) const;
    CursorType GetReturnType(void) const;
    CursorType GetTypedefType(void) const;

    List GetChildren(void) const;
    void VisitChildren(Visitor visitor, void *data = nullptr);

private:
    CXCursor m_handle;

    std::string m_fileName;
    size_t m_line;
};
