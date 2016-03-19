#include "Precompiled.hpp"

#include "Class.hpp"

#include "../ReservedTypes.hpp"

namespace
{
    const std::vector<std::string> nativeTypes
    {
        kTypeObject,
        kTypeMetaProperty
    };

    bool isNativeType(const std::string &qualifiedName)
    {
        return std::find(
            nativeTypes.begin(), 
            nativeTypes.end(), 
            qualifiedName
       ) != nativeTypes.end();
    }
}

BaseClass::BaseClass(const Cursor &cursor)
    : name(cursor.GetType().GetCanonicalType().GetDisplayName())
{

}

Class::Class(const Cursor &cursor, const Namespace &currentNamespace)
    : LanguageType(cursor, currentNamespace)
    , m_name(cursor.GetDisplayName())
    , m_qualifiedName(cursor.GetType().GetDisplayName())
{
    auto displayName = m_metaData.GetNativeString(kMetaDisplayName);

    if (displayName.empty())
    {
        m_displayName = m_qualifiedName;
    }
    else
    {
        m_displayName = 
            utils::GetQualifiedName(displayName, currentNamespace);
    }

    for (auto &child : cursor.GetChildren())
    {
        switch (child.GetKind())
        {
        case CXCursor_CXXBaseSpecifier:
        {
            auto baseClass = new BaseClass(child);

            m_baseClasses.emplace_back(baseClass);

            // automatically enable the type if not explicitly disabled
            if (isNativeType(baseClass->name))
                m_enabled = !m_metaData.GetFlag(kMetaDisable);
        }
            break;
        // constructor
        case CXCursor_Constructor:
            m_constructors.emplace_back(
                new Constructor(child, currentNamespace, this) 
           );
            break;
        // field
        case CXCursor_FieldDecl:
            m_fields.emplace_back(
                new Field(child, currentNamespace, this)
           );
            break;
        // static field
        case CXCursor_VarDecl:
            m_staticFields.emplace_back(
                new Global(child, Namespace(), this) 
           );
            break;
        // method / static method
        case CXCursor_CXXMethod:
            if (child.IsStatic()) 
            { 
                m_staticMethods.emplace_back(
                    new Function(child, Namespace(), this) 
               );
            }
            else 
            { 
                m_methods.emplace_back(
                    new Method(child, currentNamespace, this) 
               );
            }
            break;
        default:
            break;
        }
    }
}

Class::~Class(void)
{
    for (auto *baseClass : m_baseClasses)
        delete baseClass;

    for (auto *constructor : m_constructors)
        delete constructor;

    for (auto *field : m_fields)
        delete field;

    for (auto *staticField : m_staticFields)
        delete staticField;

    for (auto *method : m_methods)
        delete method;

    for (auto *staticMethod : m_staticMethods)
        delete staticMethod;
}

bool Class::ShouldCompile(void) const
{
    return isAccessible() && !isNativeType(m_qualifiedName);
}

bool Class::isAccessible(void) const
{
    return m_enabled;
}