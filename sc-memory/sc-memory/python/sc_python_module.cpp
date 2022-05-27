#include "sc_python_module.hpp"

#include "../sc_memory.hpp"
#include "../sc_stream.hpp"
#include "../sc_link.hpp"

#include "../kpm/sc_agent.hpp"

#include <iostream>

extern "C"
{
#include "sc-core/sc_memory_headers.h"
}

namespace
{
namespace bp = boost::python;

void translateException(utils::ScException const & e)
{
  // Use the Python 'C' API to set up an exception object
  PyErr_SetString(PyExc_RuntimeError, e.Description());
}

namespace impl
{
class PyTemplateGenResult
{
public:
  PyTemplateGenResult()
    : m_result(new ScTemplateGenResult())
  {
  }

  void Update()
  {
    m_replacements = bp::dict();
    for (auto const & it : m_result->GetReplacements())
      m_replacements[it.first] = it.second;
  }

  ScTemplateGenResult & GetResultRef()
  {
    return *m_result;
  }

  bp::object Get(bp::object & ind) const
  {
    ScAddr value;
    bp::extract<std::string> se(ind);
    if (se.check())
    {
      try
      {
        value = (*m_result)[static_cast<std::string>(se)];
      }
      catch (utils::ExceptionItemNotFound const &)
      {
        return bp::object();
      }
    }
    else
    {
      bp::extract<int64_t> ie(ind);
      if (ie.check())
        value = (*m_result)[static_cast<int64_t>(ie)];
    }

    return bp::object(value);
  }

  size_t Size() const
  {
    return m_result->Size();
  }

  bp::dict GetReplaceAliases() const
  {
    return m_replacements;
  }

private:
  std::shared_ptr<ScTemplateGenResult> m_result;
  bp::dict m_replacements;
};

// -----------------------------
class PyTemplateSearchResultItem
{
public:
  PyTemplateSearchResultItem()
    : m_item(new ScTemplateSearchResultItem(nullptr, nullptr))
  {
  }

  PyTemplateSearchResultItem(ScAddrVector const * results, ScTemplate::ReplacementsMap const * replacements)
    : m_item(new ScTemplateSearchResultItem(results, replacements))
  {
  }

  ScTemplateSearchResultItem & GetItemRef()
  {
    return *m_item;
  }

  ScAddr Get(bp::object & ind) const
  {
    ScAddr value;
    bp::extract<std::string> se(ind);
    if (se.check())
    {
      try
      {
        value = (*m_item)[static_cast<std::string>(se)];
      }
      catch (utils::ExceptionItemNotFound const &)
      {
      }
    }
    else
    {
      bp::extract<int64_t> ie(ind);
      if (ie.check())
        value = (*m_item)[static_cast<int64_t>(ie)];
    }
    return value;
  }

  size_t Size() const
  {
    return m_item->Size();
  }

private:
  std::shared_ptr<ScTemplateSearchResultItem> m_item;
};

// -----------------------------
class PyTemplateSearchResult
{
public:
  PyTemplateSearchResult()
    : m_result(new ScTemplateSearchResult())
  {
  }

  void Update()
  {
    m_replacements = bp::dict();
    for (auto const & it : m_result->GetReplacements())
      m_replacements[it.first] = it.second;
  }

  ScTemplateSearchResult & GetResultRef()
  {
    return *m_result;
  }

  size_t Size() const
  {
    return m_result->Size();
  }

  bp::object Get(size_t idx) const
  {
    PyTemplateSearchResultItem result;
    if (m_result->GetResultItemSafe(idx, result.GetItemRef()))
      return bp::object(result);

    return bp::object();
  }

  bp::dict GetReplaceAliases() const
  {
    return m_replacements;
  }

private:
  std::shared_ptr<ScTemplateSearchResult> m_result;
  bp::dict m_replacements;
};

// -----------------------------
class PyTemplateItemValue
{
public:
  PyTemplateItemValue()
  {
  }

  PyTemplateItemValue(ScAddr const & addr, std::string const & replName = std::string())
    : m_item(new ScTemplateItemValue(addr, replName.c_str()))
  {
  }

  PyTemplateItemValue(ScType const & type, std::string const & replName = std::string())
    : m_item(new ScTemplateItemValue(type, replName.c_str()))
  {
  }

  explicit PyTemplateItemValue(std::string const & name)
    : m_item(new ScTemplateItemValue(name))
  {
  }

  ScTemplateItemValue & GetItemRef() const
  {
    return *m_item;
  }

private:
  std::shared_ptr<ScTemplateItemValue> m_item;
};

// -----------------------------
bp::object _scAddrToRShift(ScAddr const & addr, std::string const & replName)
{
  return bp::object(PyTemplateItemValue(addr, replName));
}

bp::object _scTypeToRShift(ScType const & type, std::string const & replName)
{
  return bp::object(PyTemplateItemValue(type, replName));
}

// -----------------------------
class PyLinkContent
{
public:
  class Type
  {
  public:
    static uint8_t String;
    static uint8_t Int;
    static uint8_t Float;
  };

  PyLinkContent()
  {
  }

  explicit PyLinkContent(ScStream const & stream, uint8_t t)
    : m_type(t)
  {
    m_buffer.reset(new MemoryBufferSafe());
    m_buffer->Reinit(stream.Size());
    size_t readBytes = 0;
    stream.Read((sc_char *)m_buffer->Data(), stream.Size(), readBytes);
  }

  uint8_t GetType() const
  {
    return m_type;
  }

  std::string AsString() const
  {
    char const * data = (char const *)m_buffer->CData();
    return std::string(data, data + m_buffer->Size());
  }

  int32_t AsInt() const
  {
    if (m_buffer->Size() == sizeof(int8_t))
    {
      int8_t value = 0;
      m_buffer->Read(&value, sizeof(value));
      return int32_t(value);
    }
    else if (m_buffer->Size() == sizeof(int16_t))
    {
      int16_t value = 0;
      m_buffer->Read(&value, sizeof(value));
      return int32_t(value);
    }
    else if (m_buffer->Size() == sizeof(int32_t))
    {
      int32_t value = 0;
      m_buffer->Read(&value, sizeof(value));
      return value;
    }

    SC_THROW_EXCEPTION(utils::ExceptionInvalidType, "Size of content should be equal to 1, 2 or 4 bytes");

    return 0;
  }

  double AsDouble() const
  {
    if (m_buffer->Size() == sizeof(float))
    {
      float value = 0.0;
      m_buffer->Read(&value, sizeof(value));
      return value;
    }
    else if (m_buffer->Size() == sizeof(double))
    {
      double value = 0.0;
      m_buffer->Read(&value, sizeof(value));
      return value;
    }

    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidType,
        "Size of content should be equal to " << sizeof(double) << " or " << sizeof(float) << " bytes");

    return std::numeric_limits<double>::min();
  }

  bp::object AsBinary() const
  {
    PyObject * buff = PyMemoryView_FromMemory((char *)m_buffer->Data(), m_buffer->Size(), PyBUF_READ);
    return boost::python::object(boost::python::handle<>(buff));
  }

private:
  MemoryBufferSafePtr m_buffer;
  uint8_t m_type;
};

uint8_t PyLinkContent::Type::String = 0;
uint8_t PyLinkContent::Type::Int = 1;
uint8_t PyLinkContent::Type::Float = 2;

// ----------------------------

ScMemoryContext * _context_CreateInstance(std::string const & name)
{
  return new ScMemoryContext(sc_access_lvl_make_min, name.c_str());
}

bp::list _context_FindLinksByContent(ScMemoryContext & self, bp::object const & content)
{
  bp::list result;

  bp::extract<std::string> strContent(content);
  if (strContent.check())
  {
    ScStreamPtr stream = ScStreamConverter::StreamFromString(strContent);
    ScAddrVector const foundAddrs = self.FindLinksByContent(stream);
    for (auto addr : foundAddrs)
      result.append(bp::object(addr));
  }
  else
  {
    SC_THROW_EXCEPTION(utils::ExceptionNotImplemented, "Just string content type now supported");
  }

  return result;
}

bp::tuple _context_getEdgeInfo(ScMemoryContext & self, ScAddr const & addr)
{
  ScAddr src, trg;
  if (self.GetEdgeInfo(addr, src, trg))
    return bp::make_tuple(src, trg);

  return bp::make_tuple(bp::object(), bp::object());
}

template <typename T>
bool _set_contentT(ScMemoryContext & self, ScAddr const & linkAddr, bp::extract<T> & v)
{
  T const value = v;
  ScLink link(self, linkAddr);

  return link.Set(value);
}

bool _context_setLinkContent(ScMemoryContext & self, ScAddr const & linkAddr, bp::object & content)
{
  if (content.is_none() || !linkAddr.IsValid())
    return false;

  bp::extract<int32_t> l(content);
  if (l.check())
    return _set_contentT(self, linkAddr, l);

  bp::extract<double> d(content);
  if (d.check())
    return _set_contentT(self, linkAddr, d);

  bp::extract<std::string> s(content);
  if (s.check())
  {
    ScStreamPtr stream = ScStreamMakeRead(std::string(s));
    return self.SetLinkContent(linkAddr, stream);
  }

  return false;
}

bp::object _context_getLinkContent(ScMemoryContext & self, ScAddr const & linkAddr)
{
  ScStreamPtr stream = self.GetLinkContent(linkAddr);
  if (stream)
  {
    const ScLink link(self, linkAddr);
    uint8_t linkType = PyLinkContent::Type::String;

    const auto t = link.DetermineType();
    switch (t)
    {
    case ScLink::Type::Int8:
    case ScLink::Type::Int16:
    case ScLink::Type::Int32:
    case ScLink::Type::Int64:
    case ScLink::Type::UInt8:
    case ScLink::Type::UInt16:
    case ScLink::Type::UInt32:
    case ScLink::Type::UInt64:
      linkType = PyLinkContent::Type::Int;
      break;

    case ScLink::Type::Float:
    case ScLink::Type::Double:
      linkType = PyLinkContent::Type::Float;
      break;

    default:
      break;
    }
    return bp::object(PyLinkContent(*stream, linkType));
  }

  return bp::object();
}

template <typename TIteratorType>
class PyIteratorWrap
{
public:
  PyIteratorWrap()
  {
  }

  explicit PyIteratorWrap(TIteratorType const & iter)
    : m_iter(iter)
  {
  }

  bool IsValid() const
  {
    SC_ASSERT(m_iter.get(), ());
    return m_iter->IsValid();
  }

  bool Next() const
  {
    SC_ASSERT(m_iter.get(), ());
    return m_iter->Next();
  }

  ScAddr Get(uint8_t index) const
  {
    SC_ASSERT(m_iter.get(), ());
    return m_iter->Get(index);
  }

protected:
  TIteratorType m_iter;
};

using PyIterator3 = PyIteratorWrap<ScIterator3Ptr>;
using PyIterator5 = PyIteratorWrap<ScIterator5Ptr>;

bp::object _context_iterator3(ScMemoryContext & self, bp::object & param1, bp::object & param2, bp::object & param3)
{
  bp::extract<ScType> pt2(param2);
  if (!pt2.check())  // second parameter should be a type anyway
    return bp::object();

  bp::extract<ScAddr> pa1(param1);
  if (pa1.check())
  {
    bp::extract<ScAddr> pa3(param3);
    if (pa3.check())  // f_a_f
      return bp::object(boost::shared_ptr<PyIterator3>(new PyIterator3(
          self.Iterator3(static_cast<ScAddr>(pa1), static_cast<ScType>(pt2), static_cast<ScAddr>(pa3)))));

    bp::extract<ScType> pt3(param3);
    if (pt3.check())  // f_a_a
      return bp::object(boost::shared_ptr<PyIterator3>(new PyIterator3(
          self.Iterator3(static_cast<ScAddr>(pa1), static_cast<ScType>(pt2), static_cast<ScType>(pt3)))));
  }

  bp::extract<ScType> pt1(param1);
  if (pt1.check())
  {
    bp::extract<ScAddr> pa3(param3);
    if (pa3.check())  // a_a_f
      return bp::object(boost::shared_ptr<PyIterator3>(new PyIterator3(
          self.Iterator3(static_cast<ScType>(pt1), static_cast<ScType>(pt2), static_cast<ScAddr>(pa3)))));
  }

  return bp::object();
}

bp::object _context_iterator5(
    ScMemoryContext & self,
    bp::object & param1,
    bp::object & param2,
    bp::object & param3,
    bp::object & param4,
    bp::object & param5)
{
  // param2 and param4 are always ScType
  bp::extract<ScType> pt2(param2);
  if (!pt2.check())
    return bp::object();

  bp::extract<ScType> pt4(param4);
  if (!pt4.check())
    return bp::object();

  bp::extract<ScAddr> pa1(param1);
  if (pa1.check())
  {
    bp::extract<ScAddr> pa3(param3);
    if (pa3.check())
    {
      bp::extract<ScAddr> pa5(param5);
      if (pa5.check())  // f_a_f_a_f
        return bp::object(boost::shared_ptr<PyIterator5>(new PyIterator5(self.Iterator5(
            static_cast<ScAddr>(pa1),
            static_cast<ScType>(pt2),
            static_cast<ScAddr>(pa3),
            static_cast<ScType>(pt4),
            static_cast<ScAddr>(pa5)))));

      bp::extract<ScType> pt5(param5);
      if (pt5.check())  // f_a_f_a_a
        return bp::object(boost::shared_ptr<PyIterator5>(new PyIterator5(self.Iterator5(
            static_cast<ScAddr>(pa1),
            static_cast<ScType>(pt2),
            static_cast<ScAddr>(pa3),
            static_cast<ScType>(pt4),
            static_cast<ScType>(pt5)))));
    }

    bp::extract<ScType> pt3(param3);
    if (!pt3.check())
      return bp::object();

    bp::extract<ScAddr> pa5(param5);
    if (pa5.check())  // f_a_a_a_f
      return bp::object(boost::shared_ptr<PyIterator5>(new PyIterator5(self.Iterator5(
          static_cast<ScAddr>(pa1),
          static_cast<ScType>(pt2),
          static_cast<ScType>(pt3),
          static_cast<ScType>(pt4),
          static_cast<ScAddr>(pa5)))));

    bp::extract<ScType> pt5(param5);
    if (pt5.check())  // f_a_a_a_a
      return bp::object(boost::shared_ptr<PyIterator5>(new PyIterator5(self.Iterator5(
          static_cast<ScAddr>(pa1),
          static_cast<ScType>(pt2),
          static_cast<ScType>(pt3),
          static_cast<ScType>(pt4),
          static_cast<ScType>(pt5)))));
  }

  bp::extract<ScType> pt1(param1);
  if (pt1.check())
  {
    bp::extract<ScAddr> pa3(param3);
    if (!pa3.check())
      return bp::object();

    bp::extract<ScAddr> pa5(param5);
    if (pa5.check())  // a_a_f_a_f
      return bp::object(boost::shared_ptr<PyIterator5>(new PyIterator5(self.Iterator5(
          static_cast<ScType>(pt1),
          static_cast<ScType>(pt2),
          static_cast<ScAddr>(pa3),
          static_cast<ScType>(pt4),
          static_cast<ScAddr>(pa5)))));

    bp::extract<ScType> pt5(param5);
    if (pt5.check())  // a_a_f_a_a
      return bp::object(boost::shared_ptr<PyIterator5>(new PyIterator5(self.Iterator5(
          static_cast<ScType>(pt1),
          static_cast<ScType>(pt2),
          static_cast<ScAddr>(pa3),
          static_cast<ScType>(pt4),
          static_cast<ScType>(pt5)))));
  }

  return bp::object();
}

bp::object _context_helperResolveSysIdtf(
    ScMemoryContext & self,
    bp::object & idtf,
    bp::object const & type = bp::object())
{
  bp::extract<std::string> se(idtf);
  if (!se.check())
  {
    SC_THROW_EXCEPTION(utils::ExceptionInvalidType, "First parameter should have an instance of str");
  }
  ScType rawType;

  if (!type.is_none())
  {
    bp::extract<ScType> te(type);
    if (!te.check())
    {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidType, "Second parameter should be None or instance of ScType");
    }
    rawType = static_cast<ScType>(te);
  }

  std::string const idtfValue = static_cast<std::string>(se);
  ScAddr resultAddr = self.HelperResolveSystemIdtf(idtfValue, rawType);
  return bp::object(resultAddr);
}

bp::object _context_helperFindBySystemIdtf(ScMemoryContext & self, bp::object & idtf)
{
  bp::extract<std::string> se(idtf);
  if (!se.check())
  {
    SC_THROW_EXCEPTION(utils::ExceptionInvalidType, "First parameter should have an instance of str");
  }

  std::string const idtfValue = static_cast<std::string>(se);
  ScAddr resultAddr = self.HelperFindBySystemIdtf(idtfValue);
  return bp::object(resultAddr);
}

ScTemplateItemValue ResolveTemplateParam(bp::object & p)
{
  bp::extract<impl::PyTemplateItemValue> ve(p);
  if (ve.check())
    return static_cast<impl::PyTemplateItemValue>(ve).GetItemRef();

  bp::extract<std::string> se(p);
  if (se.check())
    return ScTemplateItemValue(static_cast<std::string>(se));

  bp::extract<ScAddr> ae(p);
  if (ae.check())
    return ScTemplateItemValue(static_cast<ScAddr>(ae));

  bp::extract<ScType> te(p);
  if (te.check())
    return ScTemplateItemValue(static_cast<ScType>(te));

  SC_THROW_EXCEPTION(
      utils::ExceptionInvalidType, "Parameter to template should be ScAddr, ScType, string or replacement");
  return {};
};

class PyTemplate
{
public:
  PyTemplate()
    : m_impl(new ScTemplate())
  {
  }

  void Triple(bp::object & param1, bp::object & param2, bp::object & param3)
  {
    m_impl->Triple(ResolveTemplateParam(param1), ResolveTemplateParam(param2), ResolveTemplateParam(param3));
  }

  void TripleWithRelation(
      bp::object & param1,
      bp::object & param2,
      bp::object & param3,
      bp::object & param4,
      bp::object & param5)
  {
    m_impl->TripleWithRelation(
        ResolveTemplateParam(param1),
        ResolveTemplateParam(param2),
        ResolveTemplateParam(param3),
        ResolveTemplateParam(param4),
        ResolveTemplateParam(param5));
  }

  bool HasReplacement(std::string const & name)
  {
    return m_impl->HasReplacement(name);
  }

  ScTemplate & GetItemRef() const
  {
    return *m_impl;
  }

private:
  std::shared_ptr<ScTemplate> m_impl;
};

class PyTemplateGenParams
{
public:
  PyTemplateGenParams()
    : m_impl(new ScTemplateParams())
  {
  }

  void Add(std::string const & paramName, ScAddr const & value)
  {
    m_impl->Add(paramName, value);
  }

  bp::object Get(std::string const & paramName)
  {
    ScAddr result;
    if (m_impl->Get(paramName, result))
      return bp::object(result);

    return bp::object();
  }

  bool IsEmpty() const
  {
    return m_impl->IsEmpty();
  }

  ScTemplateParams & GetItemRef() const
  {
    return *m_impl;
  }

private:
  std::shared_ptr<ScTemplateParams> m_impl;
};

bp::object _context_helperGenTemplate(ScMemoryContext & self, PyTemplate & templ, PyTemplateGenParams & params)
{
  PyTemplateGenResult result;
  if (self.HelperGenTemplate(templ.GetItemRef(), result.GetResultRef(), params.GetItemRef()))
  {
    result.Update();
    return bp::object(result);
  }

  return bp::object();
}

bp::object _context_helperSearchTemplate(ScMemoryContext & self, PyTemplate & templ)
{
  PyTemplateSearchResult result;
  self.HelperSearchTemplate(templ.GetItemRef(), result.GetResultRef());
  result.Update();
  return bp::object(result);
}

bp::object _context_helperBuildTemplate(ScMemoryContext & self, bp::object & data)
{
  bp::extract<ScAddr> addr(data);
  if (addr.check())
  {
    PyTemplate templ;
    if (self.HelperBuildTemplate(templ.GetItemRef(), static_cast<ScAddr>(addr)))
      return bp::object(templ);
  }

  bp::extract<std::string> str(data);
  if (str.check())
  {
    PyTemplate templ;
    std::string const value = str;
    if (self.HelperBuildTemplate(templ.GetItemRef(), value))
      return bp::object(templ);
  }

  SC_THROW_EXCEPTION(utils::ExceptionInvalidType, "Second parameter should be ScAddr or string");

  return bp::object();
}

bp::object ScAddrFromHash(ScAddr::HashType const value)
{
  return bp::object(ScAddr(value));
}

std::string GetConfigValue(std::string const & group, std::string const & key)
{
  char const * value = sc_config_get_value_string(group.c_str(), key.c_str());
  return value ? std::string(value) : "";
}

class ScAgentCommandImpl
{
public:
  static ScAddr CreateCommand(ScMemoryContext & ctx, ScAddr const & cmdClassAddr, bp::list params)
  {
    ScAddrVector _params;
    bp::ssize_t count = bp::len(params);
    for (bp::ssize_t i = 0; i < count; ++i)
      _params.emplace_back(bp::extract<ScAddr>(params[i]));

    return ScAgentAction::CreateCommand(ctx, cmdClassAddr, _params);
  }

  static bool RunCommand(ScMemoryContext & ctx, ScAddr const & cmdAddr)
  {
    return ScAgentAction::InitiateCommand(ctx, cmdAddr);
  }

  static bool RunCommandWait(ScMemoryContext & ctx, ScAddr const & cmdAddr, uint32_t waitTimeOutMS = 5000)
  {
    return ScAgentAction::InitiateCommandWait(ctx, cmdAddr, waitTimeOutMS);
  }

  static ScAddr GetCommandResultAddr(ScMemoryContext & ctx, ScAddr const & cmdAddr)
  {
    return ScAgentAction::GetCommandResultAddr(ctx, cmdAddr);
  }
};

}  // namespace impl

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(ScMemoryContext_CreateLink_overload, ScMemoryContext::CreateLink, 0, 1)

BOOST_PYTHON_MODULE(sc)
{
  bp::register_exception_translator<utils::ScException>(&translateException);

  def("ScAddrFromHash", bp::make_function(&impl::ScAddrFromHash));
  def("getScConfigValue", bp::make_function(&impl::GetConfigValue));

  bp::class_<ScMemoryContext, boost::noncopyable>("ScMemoryContext", bp::no_init)
      .def("Create", &impl::_context_CreateInstance, bp::return_value_policy<bp::manage_new_object>())
      .staticmethod("Create")
      .def("CreateNode", &ScMemoryContext::CreateNode, bp::return_value_policy<bp::return_by_value>())
      .def("CreateEdge", &ScMemoryContext::CreateEdge)
      .def(
          "CreateLink",
          &ScMemoryContext::CreateLink,
          ScMemoryContext_CreateLink_overload(bp::args("type"), "Create sc-link"))
      .def("DeleteElement", &ScMemoryContext::EraseElement)
      .def("GetName", &ScMemoryContext::GetName, bp::return_value_policy<bp::return_by_value>())
      .def("IsElement", &ScMemoryContext::IsElement)
      .def("GetElementType", &ScMemoryContext::GetElementType)
      .def("FindLinksByContent", impl::_context_FindLinksByContent)
      .def("GetEdgeInfo", impl::_context_getEdgeInfo)
      .def("SetLinkContent", impl::_context_setLinkContent)
      .def("GetLinkContent", impl::_context_getLinkContent)
      .def("Iterator3", impl::_context_iterator3)
      .def("Iterator5", impl::_context_iterator5)
      .def("HelperResolveSystemIdtf", impl::_context_helperResolveSysIdtf)
      .def("HelperSetSystemIdtf", &ScMemoryContext::HelperSetSystemIdtf)
      .def("HelperGetSystemIdtf", &ScMemoryContext::HelperGetSystemIdtf)
      .def("HelperFindBySystemIdtf", impl::_context_helperFindBySystemIdtf)
      .def("HelperCheckEdge", &ScMemoryContext::HelperCheckEdge)
      .def("HelperGenTemplate", impl::_context_helperGenTemplate)
      .def("HelperSearchTemplate", impl::_context_helperSearchTemplate)
      .def("HelperBuildTemplate", impl::_context_helperBuildTemplate);

  bp::class_<impl::PyIterator3, boost::shared_ptr<impl::PyIterator3>, boost::noncopyable>("ScIterator3", bp::no_init)
      .def("Next", &impl::PyIterator3::Next)
      .def("IsValid", &impl::PyIterator3::IsValid)
      .def("Get", &impl::PyIterator3::Get);

  bp::class_<impl::PyIterator5, boost::shared_ptr<impl::PyIterator5>>("ScIterator5", bp::no_init)
      .def("Next", &impl::PyIterator5::Next)
      .def("IsValid", &impl::PyIterator5::IsValid)
      .def("Get", &impl::PyIterator5::Get);

  bp::class_<impl::PyLinkContent>("ScLinkContent", bp::no_init)
      .def("AsString", &impl::PyLinkContent::AsString)
      .def("AsInt", &impl::PyLinkContent::AsInt)
      .def("AsFloat", &impl::PyLinkContent::AsDouble)
      .def("AsBinary", &impl::PyLinkContent::AsBinary)
      .def("GetType", &impl::PyLinkContent::GetType)
      .def_readonly("String", &impl::PyLinkContent::Type::String)
      .def_readonly("Int", &impl::PyLinkContent::Type::Int)
      .def_readonly("Float", &impl::PyLinkContent::Type::Float);

  bp::class_<impl::PyTemplateGenResult>("ScTemplateGenResult", bp::no_init)
      .def("Size", &impl::PyTemplateGenResult::Size)
      .def("__getitem__", &impl::PyTemplateGenResult::Get)
      .def("Aliases", &impl::PyTemplateGenResult::GetReplaceAliases);

  bp::class_<impl::PyTemplateSearchResultItem>("ScTemplateSearchResultItem", bp::no_init)
      .def("Size", &impl::PyTemplateSearchResultItem::Size)
      .def("__getitem__", &impl::PyTemplateSearchResultItem::Get);

  bp::class_<impl::PyTemplateSearchResult>("ScTemplateSearchResult", bp::no_init)
      .def("Size", &impl::PyTemplateSearchResult::Size)
      .def("__getitem__", &impl::PyTemplateSearchResult::Get)
      .def("Aliases", &impl::PyTemplateSearchResult::GetReplaceAliases);

  bp::class_<impl::PyTemplateItemValue>("ScTemplateItemValue", bp::no_init);

  bp::class_<impl::PyTemplateGenParams>("ScTemplateParams", bp::init<>())
      .def("Add", &impl::PyTemplateGenParams::Add)
      .def("Get", &impl::PyTemplateGenParams::Get)
      .def("IsEmpty", &impl::PyTemplateGenParams::IsEmpty);

  bp::class_<impl::PyTemplate>("ScTemplate", bp::init<>())
      .def("Triple", &impl::PyTemplate::Triple)
      .def("TripleWithRelation", &impl::PyTemplate::TripleWithRelation)
      .def("HasReplacement", &impl::PyTemplate::HasReplacement);

  bp::class_<ScAddr>("ScAddr", bp::init<>())
      .def(bp::init<ScAddr::HashType>())
      .def("IsValid", &ScAddr::IsValid)
      .def("ToInt", &ScAddr::Hash)
      .def("__eq__", &ScAddr::operator==)
      .def("__ne__", &ScAddr::operator!=)
      .def("__rshift__", impl::_scAddrToRShift)
      .def("rshift", impl::_scAddrToRShift);

  bp::class_<ScType>("ScType", bp::init<>())
      .def(bp::init<ScType::RealType>())
      .def("__eq__", &ScType::operator==)
      .def("__ne__", &ScType::operator!=)
      .def("__or__", &ScType::operator|)
      .def("__and__", &ScType::operator&)
      .def("__rshift__", impl::_scTypeToRShift)
      .def("rshift", impl::_scTypeToRShift)
      .def("IsLink", &ScType::IsLink)
      .def("IsEdge", &ScType::IsEdge)
      .def("IsNode", &ScType::IsNode)
      .def("IsUnknown", &ScType::IsUnknown)
      .def("IsConst", &ScType::IsConst)
      .def("IsVar", &ScType::IsVar)
      .def("ToInt", &ScType::operator*)

      .def_readonly("Unknown", &ScType::Unknown)
      .def_readonly("Const", &ScType::Const)
      .def_readonly("Var", &ScType::Var)
      .def_readonly("Node", &ScType::Node)
      .def_readonly("Link", &ScType::Link)

      .def_readonly("EdgeUCommon", &ScType::EdgeUCommon)
      .def_readonly("EdgeDCommon", &ScType::EdgeDCommon)
      .def_readonly("EdgeUCommonConst", &ScType::EdgeUCommonConst)
      .def_readonly("EdgeDCommonConst", &ScType::EdgeDCommonConst)
      .def_readonly("EdgeAccess", &ScType::EdgeAccess)
      .def_readonly("EdgeAccessConstPosPerm", &ScType::EdgeAccessConstPosPerm)
      .def_readonly("EdgeAccessConstNegPerm", &ScType::EdgeAccessConstNegPerm)
      .def_readonly("EdgeAccessConstFuzPerm", &ScType::EdgeAccessConstFuzPerm)
      .def_readonly("EdgeAccessConstPosTemp", &ScType::EdgeAccessConstPosTemp)
      .def_readonly("EdgeAccessConstNegTemp", &ScType::EdgeAccessConstNegTemp)
      .def_readonly("EdgeAccessConstFuzTemp", &ScType::EdgeAccessConstFuzTemp)
      .def_readonly("EdgeUCommonVar", &ScType::EdgeUCommonVar)
      .def_readonly("EdgeDCommonVar", &ScType::EdgeDCommonVar)
      .def_readonly("EdgeAccessVarPosPerm", &ScType::EdgeAccessVarPosPerm)
      .def_readonly("EdgeAccessVarNegPerm", &ScType::EdgeAccessVarNegPerm)
      .def_readonly("EdgeAccessVarFuzPerm", &ScType::EdgeAccessVarFuzPerm)
      .def_readonly("EdgeAccessVarPosTemp", &ScType::EdgeAccessVarPosTemp)
      .def_readonly("EdgeAccessVarNegTemp", &ScType::EdgeAccessVarNegTemp)
      .def_readonly("EdgeAccessVarFuzTemp", &ScType::EdgeAccessVarFuzTemp)

      .def_readonly("NodeConst", &ScType::NodeConst)
      .def_readonly("NodeVar", &ScType::NodeVar)
      .def_readonly("LinkConst", &ScType::LinkConst)
      .def_readonly("LinkVar", &ScType::LinkVar)

      .def_readonly("NodeConstStruct", &ScType::NodeConstStruct)
      .def_readonly("NodeConstTuple", &ScType::NodeConstTuple)
      .def_readonly("NodeConstRole", &ScType::NodeConstRole)
      .def_readonly("NodeConstNoRole", &ScType::NodeConstNoRole)
      .def_readonly("NodeConstClass", &ScType::NodeConstClass)
      .def_readonly("NodeConstAbstract", &ScType::NodeConstAbstract)
      .def_readonly("NodeConstMaterial", &ScType::NodeConstMaterial)

      .def_readonly("NodeVarStruct", &ScType::NodeVarStruct)
      .def_readonly("NodeVarTuple", &ScType::NodeVarTuple)
      .def_readonly("NodeVarRole", &ScType::NodeVarRole)
      .def_readonly("NodeVarNoRole", &ScType::NodeVarNoRole)
      .def_readonly("NodeVarClass", &ScType::NodeVarClass)
      .def_readonly("NodeVarAbstract", &ScType::NodeVarAbstract)
      .def_readonly("NodeVarMaterial", &ScType::NodeVarMaterial);

  bp::enum_<sc_result>("ScResult")
      .value("Ok", SC_RESULT_OK)
      .value("Error", SC_RESULT_ERROR)
      .value("ErrorInvalidParams", SC_RESULT_ERROR_INVALID_PARAMS)
      .value("ErrorInvalidType", SC_RESULT_ERROR_INVALID_TYPE)
      .value("ErrorIO", SC_RESULT_ERROR_IO)
      .value("ErrorInvalidState", SC_RESULT_ERROR_INVALID_STATE)
      .value("ErrorNotFound", SC_RESULT_ERROR_NOT_FOUND)
      .value("ErrorNoWriteRights", SC_RESULT_ERROR_NO_WRITE_RIGHTS)
      .value("ErrorNoReadRights", SC_RESULT_ERROR_NO_READ_RIGHTS)
      .value("ErrorNoRights", SC_RESULT_ERROR_NO_RIGHTS)
      .value("No", SC_RESULT_NO)
      .value("Unknown", SC_RESULT_UNKNOWN);

  bp::class_<ScKeynodes>("ScKeynodesImpl", bp::no_init)
      .def(
          "GetResultCodeAddr",
          bp::make_function(&ScKeynodes::GetResultCodeAddr, bp::return_value_policy<bp::return_by_value>()))
      .staticmethod("GetResultCodeAddr")
      .def(
          "GetResultCodeByAddr",
          bp::make_function(&ScKeynodes::GetResultCodeByAddr, bp::return_value_policy<bp::return_by_value>()))
      .staticmethod("GetResultCodeByAddr")
      .def_readonly("kCommandStateAddr", &ScKeynodes::kCommandStateAddr)
      .def_readonly("kCommandInitiatedAddr", &ScKeynodes::kCommandInitiatedAddr)
      .def_readonly("kCommandProgressdAddr", &ScKeynodes::kCommandProgressdAddr)
      .def_readonly("kCommandFinishedAddr", &ScKeynodes::kCommandFinishedAddr);

  bp::class_<impl::ScAgentCommandImpl>("ScAgentCommandImpl", bp::no_init)
      .def("CreateCommand", &impl::ScAgentCommandImpl::CreateCommand)
      .staticmethod("CreateCommand")
      .def("RunCommand", &impl::ScAgentCommandImpl::RunCommand)
      .staticmethod("RunCommand")
      .def("RunCommandWait", &impl::ScAgentCommandImpl::RunCommandWait)
      .staticmethod("RunCommandWait")
      .def("GetCommandResultAddr", &impl::ScAgentCommandImpl::GetCommandResultAddr)
      .staticmethod("GetCommandResultAddr");
}

}  // namespace

namespace py
{
void ScPythonMemoryModule::Initialize()
{
  PyImport_AppendInittab("sc", &PyInit_sc);
}

}  // namespace py
