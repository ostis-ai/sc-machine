#include "Precompiled.hpp"

#include "Class.hpp"
#include "Constructor.hpp"
#include "Field.hpp"
#include "Global.hpp"
#include "Method.hpp"
#include "Function.hpp"

#include "../ReservedTypes.hpp"

BaseClass::BaseClass(const Cursor &cursor)
    : name(cursor.GetType().GetCanonicalType().GetDisplayName())
{

}

bool BaseClass::IsNative() const
{
	return (name == Classes::Object ||
			name == Classes::Agent ||
			name == Classes::AgentAction ||
			name == Classes::Module);
}

Class::Class(const Cursor &cursor, const Namespace &currentNamespace)
    : LanguageType(cursor, currentNamespace)
    , m_name(cursor.GetDisplayName())
    , m_qualifiedName(cursor.GetType().GetDisplayName())
	, m_parser(0)
{
    m_isScObject = false;

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
			/// One way to check i

            // automatically enable the type if not explicitly disabled
			if (baseClass->IsNative())
			{
				m_isScObject = true;
				m_metaData.SetProperty(ParserMeta::ParentClass, baseClass->name);
			}
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
                    m_metaData.Merge(MetaDataManager(child));
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

	m_metaData.Check();
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

bool Class::IsActionAgent() const
{
    BaseClass const * agentClass = GetBaseAgentClass();
    return (agentClass && agentClass->name == Classes::AgentAction);
}

bool Class::IsModule() const
{
	for (tBaseClassVector::const_iterator it = m_baseClasses.begin(); it != m_baseClasses.end(); ++it)
	{
		BaseClass * const baseClass = *it;
		if (baseClass->name == Classes::Module)
			return true;
	}

	return false;
}

void Class::GenerateCode(std::string const & fileId, std::stringstream & outCode, ReflectionParser * parser) const
{
	m_parser = parser;

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
	if (IsActionAgent())
	{
		outCode << "\t";
		Field::GenerateResolveKeynodeCode(m_metaData.GetNativeString(Props::AgentCommandClass),
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
		BaseClass const * agentClass = GetBaseAgentClass();
		if (!agentClass)
		{
			EMIT_ERROR("Invalid base class for Agent " << m_displayName);
		}
        const bool isActionAgent = IsActionAgent();

		std::string listenAddr;
		std::string eventType;
		std::string instConstructParams;
		std::string constrCode;

		outCode << "\\\nprivate:";
		outCode << "\\\n\ttypedef " << agentClass->name << " Super;";

        if (isActionAgent)
		{
			outCode << "\\\n\tvirtual sc_result runImpl(ScAddr const & requestAddr, ScAddr const & resultAddr); ";
			listenAddr = "GetCommandInitiatedAddr()";
			eventType = "SC_EVENT_ADD_OUTPUT_ARC";
			instConstructParams = "msCmdClass_" + m_displayName + ", ";

			outCode << "\\\nprotected: ";
			outCode << "\\\n\t" << constrCode;
			outCode << m_displayName << "(ScAddr const & cmdClassAddr, char const * name, sc_uint8 accessLvl) : Super(cmdClassAddr, name, accessLvl) {}";
		}
		else
		{
			// determine listen addr and event type
			std::string eventData;
			if (m_metaData.GetPropertySafe(Props::Event, eventData))
			{
				size_t pos = eventData.find_first_of(",");
				if ((pos != std::string::npos) && (pos < eventData.size()))
				{
					listenAddr = eventData.substr(0, pos);
					eventType = eventData.substr(pos + 1);
					
					boost::trim(listenAddr);
					boost::trim(eventType);
				}
				else
				{
					EMIT_ERROR("Invalid value of " << Props::Event << " in " << m_displayName);
				}
			}
			else
			{
				EMIT_ERROR(Props::Event << " not specified for " << m_displayName);
			}

			// default constructor for a handler
			outCode << "\\\nprotected: ";
			outCode << "\\\n	" << constrCode;
			outCode << m_displayName << "(char const * name, sc_uint8 accessLvl) : Super(name, accessLvl) {}";
			outCode << "\\\n	virtual sc_result run(ScAddr const & listedAddr, ScAddr const & argAddr) override; ";

		}
		
		outCode << "\\\nprivate:";
		outCode << "\\\n	static sc_event * msEventPtr; ";
        if (isActionAgent)
        {
            outCode << "\\\n	static ScAddr msCmdClass_" << m_displayName << ";";
        }

		// static function for handler
		outCode << "\\\npublic: ";
		outCode << "\\\n	static sc_result handler_" << m_displayName << "(sc_event const * event, sc_addr arg) ";
		outCode << "\\\n	{";
		outCode << "\\\n		" << m_displayName << " Instance(" << instConstructParams << "\"" << m_displayName << "\", sc_access_lvl_make_min);";
		outCode << "\\\n		" << "return Instance.run(ScAddr(sc_event_get_element(event)), ScAddr(arg));";
		outCode << "\\\n	}";

		// register/unregister
		outCode << "\\\n	static void registerHandler()";
		outCode << "\\\n	{";
		outCode << "\\\n		check_expr(!msEventPtr); ";
		outCode << "\\\n		ScMemoryContext ctx(sc_access_lvl_make_min, \"handler_" << m_displayName << "\"); ";
		outCode << "\\\n		msEventPtr = sc_event_new(ctx.getRealContext(), " << listenAddr << ".getRealAddr(), " << eventType << ", 0, &" << m_displayName << "::handler_" << m_displayName << ", 0);";
		outCode << "\\\n        if (msEventPtr)";
		outCode << "\\\n        {";

		/// TODO: Use common log system
		if (isActionAgent)
		{
			outCode << "\\\n            SC_LOG_INFO(\"Register agent " << m_displayName << " to action " << m_metaData.GetNativeString(Props::AgentCommandClass) << "\");";
		}
		else
		{
			outCode << "\\\n            SC_LOG_INFO(\"Register agent " << m_displayName << "\");";
		}
		outCode << "\\\n        }";
		outCode << "\\\n        else";
		outCode << "\\\n        {";
		outCode << "\\\n            SC_LOG_ERROR(\"Can't register agent " << m_displayName << "\");";
		outCode << "\\\n        }";
	
		outCode << "\\\n	}";
		
		outCode << "\\\n	static void unregisterHandler()";
		outCode << "\\\n	{";
		outCode << "\\\n		if (msEventPtr) ";
		outCode << "\\\n		{";
		outCode << "\\\n			sc_event_destroy(msEventPtr);";
		outCode << "\\\n			msEventPtr = 0;";
		outCode << "\\\n		}";
		outCode << "\\\n	}";
	}
	else if (IsModule())	// overrides for modules
	{
		outCode << "\\\npublic:";
		outCode << "\\\n	sc_result _initialize()";
		outCode << "\\\n	{";
		outCode << "\\\n		if (!ScAgentInit())";
		outCode << "\\\n			return SC_RESULT_ERROR;";

		outCode << "\\\n		return initializeImpl();";
		outCode << "\\\n	}";

		outCode << "\\\n	sc_result _shutdown()";
		outCode << "\\\n	{";
		outCode << "\\\n		return shutdownImpl();";
		outCode << "\\\n	}";

		std::string loadPriority;
		if (!m_metaData.GetPropertySafe(Props::LoadOrder, loadPriority))
			loadPriority = "1000";

		outCode << "\\\n	sc_uint32 _getLoadPriority()";
		outCode << "\\\n	{";
		outCode << "\\\n		return " << loadPriority << ";";
		outCode << "\\\n	}";
	}
}

void Class::GenerateImpl(std::stringstream & outCode) const
{
	if (IsAgent())
	{
		outCode << "\\\nsc_event * " << m_displayName << "::msEventPtr = 0;";
        if (IsActionAgent())
        {
            outCode << "\\\nScAddr " << m_displayName << "::msCmdClass_" << m_displayName << ";";
        }
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

std::string Class::GetQualifiedName() const
{
	return m_qualifiedName;
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

BaseClass const * Class::GetBaseAgentClass() const
{
	for (tBaseClassVector::const_iterator it = m_baseClasses.begin(); it != m_baseClasses.end(); ++it)
	{
		if ((*it)->name.find("ScAgent") != std::string::npos)
			return *it;
	}

	return 0;
}
