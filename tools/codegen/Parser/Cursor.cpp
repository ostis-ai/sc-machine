#include "Precompiled.hpp"

#include "Cursor.hpp"

#include "MetaUtils.hpp"

Cursor::Cursor(const CXCursor &handle)
    : m_handle(handle)
{ 
    CXSourceLocation loc = clang_getCursorLocation(m_handle);
    CXFile file;
    unsigned int line, column, offset;
    clang_getSpellingLocation(loc, &file, &line, &column, &offset);

    if (file)
        m_fileName = clang_getCString(clang_getFileName(file));

    m_line = line;
}

CXCursorKind Cursor::GetKind(void) const
{
    return m_handle.kind;
}

Cursor Cursor::GetLexicalParent(void) const
{
    return clang_getCursorLexicalParent(m_handle);
}

std::string Cursor::GetSpelling(void) const
{
    std::string spelling;

    utils::ToString(clang_getCursorSpelling(m_handle), spelling);

    return spelling;
}

std::string Cursor::GetDisplayName(void) const
{
    std::string displayName;

    utils::ToString(clang_getCursorDisplayName(m_handle), displayName);

    return displayName;
}

std::string const & Cursor::GetFileName() const
{
    return m_fileName;
}

size_t Cursor::GetLineNumber() const
{
    return m_line;
}

bool Cursor::IsDefinition(void) const
{
    return clang_isCursorDefinition(m_handle) ? true : false;
}

bool Cursor::IsStatic(void) const
{
    return clang_CXXMethod_isStatic(m_handle) ? true : false;
}

CX_CXXAccessSpecifier Cursor::GetAccessModifier(void) const
{
    return clang_getCXXAccessSpecifier(m_handle);
}


CursorType Cursor::GetType(void) const
{
    return clang_getCursorType(m_handle);
}

CursorType Cursor::GetReturnType(void) const
{
    return clang_getCursorResultType(m_handle);
}

CursorType Cursor::GetTypedefType(void) const
{
    return clang_getTypedefDeclUnderlyingType(m_handle);
}

Cursor::List Cursor::GetChildren(void) const
{
    List children;

    auto visitor = [](CXCursor cursor, CXCursor parent, CXClientData data)
    {
        auto container = static_cast<List *>(data);

        container->emplace_back(cursor);

        if (cursor.kind == CXCursor_LastPreprocessing)
            return CXChildVisit_Break;

        return CXChildVisit_Continue;
    };

    clang_visitChildren(m_handle, visitor, &children);

    return children;
}

void Cursor::VisitChildren(Visitor visitor, void *data)
{
    clang_visitChildren(m_handle, visitor, data);
}
