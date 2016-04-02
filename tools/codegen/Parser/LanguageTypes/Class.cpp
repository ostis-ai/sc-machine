#include "Precompiled.hpp"

#include "Class.hpp"

#include "../ReservedTypes.hpp"

BaseClass::BaseClass(const Cursor &cursor)
    : name(cursor.GetType().GetCanonicalType().GetDisplayName())
{

}

bool BaseClass::IsNative() const
{
	return (name == Classes::Object ||
			name == Classes::Agent);
}

Class::Class(const Cursor &cursor, const Namespace &currentNamespace)
    : LanguageType(cursor, currentNamespace)
    , m_name(cursor.GetDisplayName())
    , m_qualifiedName(cursor.GetType().GetDisplayName())
{
    m_isScObject = false;

    /*auto displayName = m_metaData.GetNativeString(kMetaDisplayName);

    if (displayName.empty())
    {
        m_displayName = m_qualifiedName;
    }
    else
    {
        m_displayName = utils::GetQualifiedName(displayName, currentNamespace);
    }*/

	m_displayName = cursor.GetSpelling();

    for (auto &child : cursor.GetChildren())
    {
        switch (child.GetKind())
        {

        case CXCursor_CXXBaseSpecifier:
        {
            auto baseClass = new BaseClass(child);

            m_baseClasses.emplace_back(baseClass);

			/// TODO: Implement recursive base class parsing (disable check of filename, and parse whole hierarchy)
			/// Possible best way to check if it has SC_CLASS field

            // automatically enable the type if not explicitly disabled
			if (baseClass->IsNative())
                m_isScObject = true;
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
				m_staticMethods.emplace_back(new Function(child, Namespace(), this));
            }
            else 
            {
				std::string const name = child.GetSpelling();
				if (name == "__null_meta")
				{
					m_metaData = MetaDataManager(child);
				}
				else
				{
					m_methods.emplace_back(new Method(child, currentNamespace, this));
				}
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
    return m_isScObject;
}

bool Class::IsAgent() const
{
	return m_metaData.HasProperty(Props::Agent);
}

void Class::GenerateCode(std::string const & fileId, std::stringstream & outCode) const
{
	std::string const line = GetGeneratedBodyLine();

	outCode << "\n\n#define " << fileId << "_" << line << "_init ";
	GenerateCodeInit(outCode);

	outCode << "\n\n#define " << fileId << "_" << line << "_initStatic ";
	GenerateCodeStaticInit(outCode);

	outCode << "\n\n#define " << fileId << "_" << line << "_decl ";
	GenerateDeclarations(outCode);

	outCode << "\n\n#define " << fileId << "_" << m_displayName << "_impl ";
	GenerateImpl(outCode);
}

#define _GENERATE_INIT_CODE(FuncName, Method, Modifier) \
    outCode << Modifier << " bool " << FuncName << "() \\\n{ \\\n"; \
    outCode << "    ScMemoryContext ctx(sc_access_lvl_make_min, \"" << m_name << "::" << FuncName << "\"); \\\n"; \
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
        outCode << "\t";
        field->GenerateInitCode(outCode);
        outCode << " \\\n";
    }

	// init data specified for an agents
	if (IsAgent())
	{
		outCode << "\t";
		Global::GenerateResolveKeynodeCode(m_metaData.GetNativeString(Props::AgentCommandClass),
			"msCmdClass_" + m_displayName,
			true,
			outCode);
		outCode << " \\\n";
	}
}

void Class::GenerateDeclarations(std::stringstream & outCode) const
{
	// overrides for agents
	if (IsAgent())
	{
		BaseClass const * agentClass = GetBaseClass(Classes::Agent);
		if (!agentClass)
		{
			EMIT_ERROR("Invalid base class for Agent " << m_displayName);
		}

		if (!m_metaData.HasProperty(Props::AgentCommandClass))
		{
			EMIT_ERROR(Props::AgentCommandClass << " not specified for " << m_displayName);
		}

		
		outCode << "\\\nprivate:";
		outCode << "\\\n\ttypedef " << agentClass->name << " Super;";
		outCode << "\\\n\tvirtual sc_result runImpl(ScAddr const & requestAddr, ScAddr const & resultAddr); ";
		outCode << "\\\n\tstatic sc_event * msEventPtr; ";
		outCode << "\\\n\tstatic ScAddr msCmdClass_" << m_displayName << ";";

		// static function for handler
		outCode << "\\\npublic: ";
		outCode << "\\\n \tstatic sc_result handler_" << m_displayName << "(sc_event const * event, sc_addr arg) ";
		outCode << "\\\n\t{";
		outCode << "\\\n\t\t" << m_displayName << " Instance(msCmdClass_" << m_displayName <<", \"" << m_displayName << "\", sc_access_lvl_make_min);";
		outCode << "\\\n\t\t" << "return Instance.run(ScAddr(arg));";
		outCode << "\\\n\t}";

		// register/unregister
		outCode << "\\\n\tstatic void registerHandler()";
		outCode << "\\\n\t{";
		outCode << "\\\n\t\tcheck_expr(!msEventPtr); ";
		outCode << "\\\n\t\tScMemoryContext ctx(sc_access_lvl_make_min, \"handler_" << m_displayName << "\"); ";
		outCode << "\\\n\t\tmsEventPtr = sc_event_new(ctx.getRealContext(), GetCommandInitiatedAddr().getRealAddr(), SC_EVENT_ADD_OUTPUT_ARC, 0, &" << m_displayName << "::handler_" << m_displayName << ", 0);";
		outCode << "\\\n\t}";

		outCode << "\\\n\tstatic void unregisterHandler()";
		outCode << "\\\n\t{";
		outCode << "\\\n\t\tif (msEventPtr) ";
		outCode << "\\\n\t\t{";
		outCode << "\\\n\t\t\tsc_event_destroy(msEventPtr);";
		outCode << "\\\n\t\t\tmsEventPtr = 0;";
		outCode << "\\\n\t\t}";
		outCode << "\\\n\t}";

		// default constructor for a handler
		outCode << "\\\nprotected: ";
		outCode << "\\\n\t" << m_displayName << "(ScAddr const & cmdClassAddr, char const * name, sc_uint8 accessLvl) : Super(cmdClassAddr, name, accessLvl) {}";
	}
}

void Class::GenerateImpl(std::stringstream & outCode) const
{
	if (IsAgent())
	{
		outCode << "\\\nsc_event * " << m_displayName << "::msEventPtr = 0;";
		outCode << "\\\nScAddr " << m_displayName << "::msCmdClass_" << m_displayName << ";";
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

BaseClass const * Class::GetBaseClass(std::string const & name) const
{
	for (tBaseClassVector::const_iterator it = m_baseClasses.begin(); it != m_baseClasses.end(); ++it)
	{
		if ((*it)->name == name)
			return *it;
	}

	return 0;
}