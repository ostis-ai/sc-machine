#include "Class.hpp"
#include "Constructor.hpp"
#include "Cursor.hpp"
#include "Field.hpp"
#include "Global.hpp"
#include "Method.hpp"
#include "Function.hpp"

#include "../ReservedTypes.hpp"

#include <boost/algorithm/string.hpp>

BaseClass::BaseClass(const Cursor & cursor)
  : name(cursor.GetType().GetCanonicalType().GetDisplayName())
{
}

bool BaseClass::IsNative() const
{
  return (name == Classes::Object || name == Classes::Agent || name == Classes::AgentAction || name == Classes::Module);
}

Class::Class(const Cursor & cursor, const Namespace & currentNamespace)
  : LanguageType(cursor, currentNamespace)
  , m_name(cursor.GetDisplayName())
  , m_qualifiedName(cursor.GetType().GetDisplayName())
  , m_parser(0)
{
  m_isScObject = false;
  m_displayName = cursor.GetSpelling();

  for (auto & child : cursor.GetChildren())
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
      m_constructors.emplace_back(new Constructor(child, currentNamespace, this));
      break;

      // field
    case CXCursor_FieldDecl:
      m_fields.emplace_back(new Field(child, currentNamespace));
      break;

      // static field
    case CXCursor_VarDecl:
      m_staticFields.emplace_back(new Global(child, Namespace(), this));
      break;

      // method / static method
    case CXCursor_CXXMethod:
      if (child.IsStatic())
      {
        m_staticMethods.emplace_back(new Function(child, Namespace()));
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
          m_methods.emplace_back(new Method(child, currentNamespace));
        }
      }
      break;

    default:
      break;
    }
  }

  m_metaData.Check();
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
  for (auto const & baseClass : m_baseClasses)
  {
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

#define _GENERATE_INIT_CODE(FuncName, Method, PreModifier, PostModifier) \
  outCode << PreModifier << " bool " << FuncName << "() " << PostModifier << " \\\n{ \\\n"; \
  outCode << "    ScMemoryContext ctx(sc_access_lvl_make_min, \"" << m_name << "::" << FuncName << "\"); \\\n"; \
  outCode << "    bool result = true; \\\n"; \
  Method(outCode); \
  outCode << "    return result; \\\n"; \
  outCode << "}\n";

void Class::GenerateCodeInit(std::stringstream & outCode) const
{
  _GENERATE_INIT_CODE("_InitInternal", GenerateFieldsInitCode, "", "override")
}

void Class::GenerateCodeStaticInit(std::stringstream & outCode) const
{
  _GENERATE_INIT_CODE("_InitStaticInternal", GenerateStaticFieldsInitCode, "static", "")
}

void Class::GenerateFieldsInitCode(std::stringstream & outCode) const
{
  for (auto const & field : m_fields)
  {
    outCode << "    ";
    field->GenarateInitCode(outCode);
    outCode << " \\\n";
  }
}

void Class::GenerateStaticFieldsInitCode(std::stringstream & outCode) const
{
  for (auto const & field : m_staticFields)
  {
    outCode << "\t";
    field->GenerateInitCode(outCode);
    outCode << " \\\n";
  }

  // init data specified for an agents
  if (IsActionAgent())
  {
    outCode << "\t";
    Field::GenerateResolveKeynodeCode(
        m_metaData.GetNativeString(Props::AgentCommandClass),
        "ms_cmdClass_" + m_displayName,
        "ScType::NodeConstClass",
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
      outCode << "\\\n\tvirtual sc_result RunImpl(ScAddr const & requestAddr, ScAddr const & resultAddr) override; ";
      listenAddr = "GetCommandInitiatedAddr()";
      eventType = "ScEvent::Type::AddOutputEdge";
      instConstructParams = "ms_cmdClass_" + m_displayName + ", ";

      outCode << "\\\nprotected: ";
      outCode << "\\\n\t" << constrCode;
      outCode << m_displayName
              << "(ScAddr const & cmdClassAddr, char const * name, sc_uint8 accessLvl) : Super(cmdClassAddr, name, "
                 "accessLvl) {}";
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
      outCode << "\\\n	virtual sc_result Run(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & "
                 "otherAddr) override; ";
    }

    outCode << "\\\nprivate:";
    outCode << "\\\n	static std::unique_ptr<ScEvent> ms_event;";
    outCode << "\\\n    static std::unique_ptr<ScMemoryContext> ms_context;";
    if (isActionAgent)
    {
      outCode << "\\\n	static ScAddr ms_cmdClass_" << m_displayName << ";";
    }

    // static function for handler
    outCode << "\\\npublic: ";
    if (isActionAgent)
    {
      outCode << "\\\n  static ScAddr const & GetCommandClassAddr() { return ms_cmdClass_" << m_displayName << "; }";
    }
    outCode << "\\\n	static bool handler_emit"
            << "(ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & otherAddr)";
    outCode << "\\\n	{";
    outCode << "\\\n		" << m_displayName << " Instance(" << instConstructParams << "\"" << m_displayName
            << "\", sc_access_lvl_make_min);";
    outCode << "\\\n		"
            << "return Instance.Run(addr, edgeAddr, otherAddr) == SC_RESULT_OK;";
    outCode << "\\\n	}";

    // register/unregister
    outCode << "\\\n	static void RegisterHandler()";
    outCode << "\\\n	{";
    outCode << "\\\n		SC_ASSERT(!ms_event.get(), ());";
    outCode << "\\\n		SC_ASSERT(!ms_context.get(), ());";
    outCode << "\\\n		ms_context.reset(new ScMemoryContext(sc_access_lvl_make_min, \"handler_" << m_displayName
            << "\"));";
    outCode << "\\\n		ms_event.reset(new ScEvent(*ms_context, " << listenAddr << ", " << eventType << ", &"
            << m_displayName << "::handler_emit"
            << "));";
    outCode << "\\\n        if (ms_event.get())";
    outCode << "\\\n        {";

    /// TODO: Use common log system
    if (isActionAgent)
    {
      outCode << "\\\n            SC_LOG_INFO(\"Register agent " << m_displayName << " to action "
              << m_metaData.GetNativeString(Props::AgentCommandClass) << "\");";
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

    outCode << "\\\n	static void UnregisterHandler()";
    outCode << "\\\n	{";
    outCode << "\\\n		ms_event.reset();";
    outCode << "\\\n		ms_context.reset();";
    outCode << "\\\n	}";
  }
  else if (IsModule())  // overrides for modules
  {
    outCode << "\\\npublic:";
    outCode << "\\\n	sc_result InitializeGenerated()";
    outCode << "\\\n	{";
    outCode << "\\\n		if (!ScKeynodes::Init())";
    outCode << "\\\n			return SC_RESULT_ERROR;";

    outCode << "\\\n		if (!ScAgentInit(false))";
    outCode << "\\\n			return SC_RESULT_ERROR;";

    outCode << "\\\n		return InitializeImpl();";
    outCode << "\\\n	}";

    outCode << "\\\n	sc_result ShutdownGenerated()";
    outCode << "\\\n	{";
    outCode << "\\\n		return ShutdownImpl();";
    outCode << "\\\n	}";

    std::string loadPriority;
    if (!m_metaData.GetPropertySafe(Props::LoadOrder, loadPriority))
      loadPriority = "1000";

    outCode << "\\\n	sc_uint32 GetLoadPriorityGenerated()";
    outCode << "\\\n	{";
    outCode << "\\\n		return " << loadPriority << ";";
    outCode << "\\\n	}";
  }
}

void Class::GenerateImpl(std::stringstream & outCode) const
{
  if (IsAgent())
  {
    outCode << "\\\nstd::unique_ptr<ScEvent> " << m_displayName << "::ms_event;";
    outCode << "\\\nstd::unique_ptr<ScMemoryContext> " << m_displayName << "::ms_context;";
    if (IsActionAgent())
    {
      outCode << "\\\nScAddr " << m_displayName << "::ms_cmdClass_" << m_displayName << ";";
    }
  }
}

std::string Class::GetGeneratedBodyLine() const
{
  for (auto const & field : m_methods)
  {
    MetaDataManager const & meta = field->GetMetaData();
    std::string propBody;

    if (meta.GetPropertySafe(Props::Body, propBody))
      return propBody;
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
  for (auto const & cl : m_baseClasses)
  {
    if (cl->name == name)
      return cl.get();
  }

  return 0;
}

BaseClass const * Class::GetBaseAgentClass() const
{
  for (auto const & cl : m_baseClasses)
  {
    if (cl->name.find("ScAgent") != std::string::npos)
      return cl.get();
  }

  return 0;
}
