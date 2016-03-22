#include "Precompiled.hpp"

#include "Class.hpp"

#include "../ReservedTypes.hpp"

BaseClass::BaseClass(const Cursor &cursor)
    : name(cursor.GetType().GetCanonicalType().GetDisplayName())
{

}

Class::Class(const Cursor &cursor, const Namespace &currentNamespace)
    : LanguageType(cursor, currentNamespace)
    , m_name(cursor.GetDisplayName())
    , m_qualifiedName(cursor.GetType().GetDisplayName())
{
    isScObject = false;

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
            if (baseClass->name == "ScObject")
                isScObject = true;
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

bool Class::ShouldGenerate(void) const
{
    return isScObject;
}

#define _GENERATE_INIT_CODE(FuncName, Method, Modifier) \
    outCode << Modifier << " bool " << FuncName << "() \\\n{ \\\n"; \
    outCode << "    ScMemoryContext ctx(sc_access_lvl_make_max, \"" << m_name << "::" << FuncName << "\"); \\\n"; \
    outCode << "    bool result = true; \\\n"; \
    Method(outCode); \
    outCode << "    return result; \\\n"; \
    outCode << "}\n";

void Class::GenerateCodeInit(std::stringstream & outCode) const
{
    _GENERATE_INIT_CODE("_initInternal", GenerateFieldsInitCode, "")
}

void Class::GenerateCodeStaticInit(std::stringstream & outCode) const
{
    _GENERATE_INIT_CODE("_initStaticInternal", GenerateStaticFieldsInitCode, "static")
}

void Class::GenerateFieldsInitCode(std::stringstream & outCode) const
{
    for (tFieldsVector::const_iterator it = m_fields.begin(); it != m_fields.end(); ++it)
    {
        Field * field = *it;
        outCode << "    ";
        field->GenarateInitCode(outCode);
        outCode << " \\\n";
    }
}

void Class::GenerateStaticFieldsInitCode(std::stringstream & outCode) const
{
    for (tStaticFieldsVector::const_iterator it = m_staticFields.begin(); it != m_staticFields.end(); ++it)
    {
        Global * field = *it;
        outCode << "    ";
        field->GenerateInitCode(outCode);
        outCode << " \\\n";
    }
}

std::string Class::GetGeneratedBodyLine() const
{
    for (tMethodVector::const_iterator it = m_methods.begin(); it != m_methods.end(); ++it)
    {
        Method const * const field = *it;
        MetaDataManager const & meta = field->GetMetaData();
        std::string propBody;

        if (meta.GetPropertySafe(Props::Body, propBody))
        {
            return propBody;
        }
    }

    EMIT_ERROR("Can't find " << Props::Body << " meta info");

    return "";
}