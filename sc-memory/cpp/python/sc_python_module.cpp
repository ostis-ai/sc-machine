#include "sc_python_module.hpp"

#include "sc_python_includes.hpp"

#include "../sc_memory.hpp"
#include "../sc_stream.hpp"

namespace
{
namespace bp = boost::python;

void translateException(utils::ScException const & e)
{
  // Use the Python 'C' API to set up an exception object
  PyErr_SetString(PyExc_RuntimeError, e.Message());
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

  ScTemplateGenResult & GetResultRef()
  {
    return m_result.GetRef();
  }

  ScAddr Get(std::string const & name) const
  {
    return m_result.GetRef()[name];
  }

  size_t Size() const
  {
    return m_result->Size();
  }

private:
  TSharedPointer<ScTemplateGenResult> m_result;
};

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
    return m_item.GetRef();
  }

  ScAddr Get(bp::object & ind) const
  {
    bp::extract<std::string> se(ind);
    if (se.check())
      return m_item.GetRef()[static_cast<std::string>(se)];

    bp::extract<int64_t> ie(ind);
    if (ie.check())
      return m_item.GetRef()[static_cast<int64_t>(ie)];

    return ScAddr();
  }

  size_t Size() const
  {
    return m_item->Size();
  }

private:
  TSharedPointer<ScTemplateSearchResultItem> m_item;
};

class PyTemplateSearchResult
{
public:
  PyTemplateSearchResult()
    : m_result(new ScTemplateSearchResult())
  {
  }

  ScTemplateSearchResult & GetResultRef()
  {
    return m_result.GetRef();
  }

  size_t Size() const
  {
    return m_result->Size();
  }

  bp::object Get(size_t idx) const
  {
    PyTemplateSearchResultItem result;
    if (m_result->GetResultItemSave(idx, result.GetItemRef()))
      return bp::object(result);

    return bp::object();
  }

private:
  TSharedPointer<ScTemplateSearchResult> m_result;
};

class PyTemplateItemValue
{
public:
  PyTemplateItemValue() {}

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
    return m_item.GetRef();
  }

private:
  TSharedPointer<ScTemplateItemValue> m_item;
};

bp::object _scAddrToRShift(ScAddr const & addr, std::string const & replName)
{
  return bp::object(PyTemplateItemValue(addr, replName));
}

bp::object _scTypeToRShift(ScType const & type, std::string const & replName)
{
  return bp::object(PyTemplateItemValue(type, replName));
}

class PyLinkContent
{
public:
  PyLinkContent() {}

  explicit PyLinkContent(ScStream const & stream)
  {
    m_buffer = new MemoryBufferSafe();
    m_buffer->Reinit(stream.Size());
    size_t readBytes = 0;
    stream.Read((sc_char*)m_buffer->Data(), stream.Size(), readBytes);
  }

  std::string AsString() const
  {
    char const * data = (char const*)m_buffer->CData();
    return std::string(data, data + m_buffer->Size());
  }

  int64_t AsInt() const
  {
    if (m_buffer->Size() != sizeof(int64_t))
    {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidType,
                         "Size of content should be equal to " << sizeof(int64_t) << " bytes");
    }

    int64_t value = 0;
    m_buffer->Read(&value, sizeof(value));
    return value;
  }

  double AsDouble() const
  {
    if (m_buffer->Size() != sizeof(double))
    {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidType,
                         "Size of content should be equal to " << sizeof(double) << " bytes");
    }

    double value = 0.0;
    m_buffer->Read(&value, sizeof(value));
    return value;
  }

private:
  MemoryBufferSafePtr m_buffer;
};

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
  ScStreamPtr const stream = MakeReadStreamT(value);
  return self.SetLinkContent(linkAddr, *stream);
}

bool _context_setLinkContent(ScMemoryContext & self, ScAddr const & linkAddr, bp::object & content)
{
  if (content.is_none() || !linkAddr.IsValid())
    return false;

  bp::extract<int64_t> l(content);
  if (l.check())
    return _set_contentT(self, linkAddr, l);

  bp::extract<double> d(content);
  if (d.check())
    return _set_contentT(self, linkAddr, d);

  bp::extract<std::string> s(content);
  if (s.check())
  {
    std::string const value = s;
    ScStream stream(value.c_str(), value.size(), SC_STREAM_FLAG_READ);
    return self.SetLinkContent(linkAddr, stream);
  }

  return false;
}

bp::object _context_getLinkContent(ScMemoryContext & self, ScAddr const & linkAddr)
{
  ScStream stream;
  if (self.GetLinkContent(linkAddr, stream))
    return bp::object(PyLinkContent(stream));

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
    SC_ASSERT(m_iter.IsPtrValid(), ());
    return m_iter->IsValid();
  }

  bool Next() const
  {
    SC_ASSERT(m_iter.IsPtrValid(), ());
    return m_iter->Next();
  }

  ScAddr Get(uint8_t index) const
  {
    SC_ASSERT(m_iter.IsPtrValid(), ());
    return m_iter->Get(index);
  }

protected:
  TIteratorType m_iter;
};

using PyIterator3 = PyIteratorWrap<ScIterator3Ptr>;
using PyIterator5 = PyIteratorWrap<ScIterator5Ptr>;

bp::object _context_iterator3(ScMemoryContext & self,
                              bp::object & param1,
                              bp::object & param2,
                              bp::object & param3)
{
  bp::extract<ScType> pt2(param2);
  if (!pt2.check()) // second parameter should be a type anyway
    return bp::object();

  bp::extract<ScAddr> pa1(param1);
  if (pa1.check())
  {
    bp::extract<ScAddr> pa3(param3);
    if (pa3.check()) // f_a_f
      return bp::object(PyIterator3(self.Iterator3(static_cast<ScAddr>(pa1),
                                                   static_cast<ScType>(pt2),
                                                   static_cast<ScAddr>(pa3))));

    bp::extract<ScType> pt3(param3);
    if (pt3.check()) // f_a_a
      return bp::object(PyIterator3(self.Iterator3(static_cast<ScAddr>(pa1),
                                                   static_cast<ScType>(pt2),
                                                   static_cast<ScType>(pt3))));
  }

  bp::extract<ScType> pt1(param1);
  if (pt1.check())
  {
    bp::extract<ScAddr> pa3(param3);
    if (pa3.check()) // a_a_f
      return bp::object(PyIterator3(self.Iterator3(static_cast<ScType>(pt1),
                                                   static_cast<ScType>(pt2),
                                                   static_cast<ScAddr>(pa3))));
  }

  return bp::object();
}

bp::object _context_iterator5(ScMemoryContext & self,
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
      if (pa5.check()) // f_a_f_a_f
        return bp::object(PyIterator5(self.Iterator5(static_cast<ScAddr>(pa1),
                                                     static_cast<ScType>(pt2),
                                                     static_cast<ScAddr>(pa3),
                                                     static_cast<ScType>(pt4),
                                                     static_cast<ScAddr>(pa5))));

      bp::extract<ScType> pt5(param5);
      if (pt5.check()) // f_a_f_a_a
        return bp::object(PyIterator5(self.Iterator5(static_cast<ScAddr>(pa1),
                                                     static_cast<ScType>(pt2),
                                                     static_cast<ScAddr>(pa3),
                                                     static_cast<ScType>(pt4),
                                                     static_cast<ScType>(pt5))));
    }

    bp::extract<ScType> pt3(param3);
    if (!pt3.check())
      return bp::object();

    bp::extract<ScAddr> pa5(param5);
    if (pa5.check()) // f_a_a_a_f
      return bp::object(PyIterator5(self.Iterator5(static_cast<ScAddr>(pa1),
                                                   static_cast<ScType>(pt2),
                                                   static_cast<ScType>(pt3),
                                                   static_cast<ScType>(pt4),
                                                   static_cast<ScAddr>(pa5))));

    bp::extract<ScType> pt5(param5);
    if (pt5.check()) // f_a_a_a_a
      return bp::object(PyIterator5(self.Iterator5(static_cast<ScAddr>(pa1),
                                                   static_cast<ScType>(pt2),
                                                   static_cast<ScType>(pt3),
                                                   static_cast<ScType>(pt4),
                                                   static_cast<ScType>(pt5))));
  }

  bp::extract<ScType> pt1(param1);
  if (pt1.check())
  {
    bp::extract<ScAddr> pa3(param3);
    if (!pa3.check())
      return bp::object();

    bp::extract<ScAddr> pa5(param5);
    if (pa5.check()) // a_a_f_a_f
      return bp::object(PyIterator5(self.Iterator5(static_cast<ScType>(pt1),
                                                   static_cast<ScType>(pt2),
                                                   static_cast<ScAddr>(pa3),
                                                   static_cast<ScType>(pt4),
                                                   static_cast<ScAddr>(pa5))));

    bp::extract<ScType> pt5(param5);
    if (pt5.check()) // a_a_f_a_a
      return bp::object(PyIterator5(self.Iterator5(static_cast<ScType>(pt1),
                                                   static_cast<ScType>(pt2),
                                                   static_cast<ScAddr>(pa3),
                                                   static_cast<ScType>(pt4),
                                                   static_cast<ScType>(pt5))));
  }

  return bp::object();
}

bp::object _context_helperResolveSysIdtf(ScMemoryContext & self, bp::object & idtf, bp::object & type = bp::object())
{
  bp::extract<std::string> se(idtf);
  if (!se.check())
  {
    SC_THROW_EXCEPTION(utils::ExceptionInvalidType,
                       "First parameter should have an instance of str");
  }
  ScType rawType;

  if (!type.is_none())
  {
    bp::extract<ScType> te(type);
    if (!te.check())
    {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidType,
                         "Second parameter should be None or instance of ScType")
    }
    rawType = static_cast<ScType>(te);
  }

  std::string const idtfValue = static_cast<std::string>(se);
  ScAddr resultAddr = self.HelperResolveSystemIdtf(idtfValue, rawType);
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

  SC_THROW_EXCEPTION(utils::ExceptionInvalidType,
                     "Parameter to template should be ScAddr, ScType, string or replacement");
  return{};
};

void _templateTriple(ScTemplate & templ, bp::object & param1, bp::object & param2, bp::object & param3)
{
  templ.Triple(ResolveTemplateParam(param1),
               ResolveTemplateParam(param2),
               ResolveTemplateParam(param3));
}

void _templateTripleWithRelation(ScTemplate & templ, bp::object & param1, bp::object & param2,
                                 bp::object & param3, bp::object & param4, bp::object & param5)
{
  templ.TripleWithRelation(ResolveTemplateParam(param1),
                           ResolveTemplateParam(param2),
                           ResolveTemplateParam(param3),
                           ResolveTemplateParam(param4),
                           ResolveTemplateParam(param5));
}

void _templateGenParamsAdd(ScTemplateGenParams & self, std::string const & paramName, ScAddr const & value)
{
  self.Add(paramName, value);
}

bp::object _templateGenParamsGet(ScTemplateGenParams & self, std::string const & paramName)
{
  ScAddr result;
  if (self.Get(paramName, result))
    return bp::object(result);

  return bp::object();
}

bp::object _context_helperGenTemplate(ScMemoryContext & self, ScTemplate & templ, ScTemplateGenParams & params)
{
  PyTemplateGenResult result;
  if (self.HelperGenTemplate(templ, result.GetResultRef(), params))
    return bp::object(result);

  return bp::object();
}

bp::object _context_helperSearchTemplate(ScMemoryContext & self, ScTemplate & templ)
{
  PyTemplateSearchResult result;
  self.HelperSearchTemplate(templ, result.GetResultRef());
  return bp::object(result);
}

bp::object _context_helperBuildTemplate(ScMemoryContext & self, ScAddr const & templAddr)
{
  ScTemplate templ;
  if (self.HelperBuildTemplate(templ, templAddr))
    return bp::object(templ);

  return bp::object();
}

} // namespace impl

BOOST_PYTHON_MODULE(sc)
{
  bp::register_exception_translator<utils::ScException>(&translateException);

  bp::class_<ScMemoryContext>("ScMemoryContext", bp::init<uint8_t, std::string>())
    .def("CreateNode", &ScMemoryContext::CreateNode)
    .def("CreateEdge", &ScMemoryContext::CreateEdge)
    .def("CreateLink", &ScMemoryContext::CreateLink)
    .def("GetName", &ScMemoryContext::GetName)
    .def("IsElement", &ScMemoryContext::IsElement)
    .def("GetElementType", &ScMemoryContext::GetElementType)
    .def("GetEdgeInfo", impl::_context_getEdgeInfo)
    .def("SetLinkContent", impl::_context_setLinkContent)
    .def("GetLinkContent", impl::_context_getLinkContent)
    .def("Iterator3", impl::_context_iterator3)
    .def("Iterator5", impl::_context_iterator5)
    .def("HelperResolveSystemIdtf", impl::_context_helperResolveSysIdtf)
    .def("HelperSetSystemIdtf", &ScMemoryContext::HelperSetSystemIdtf)
    .def("HelperGetSystemIdtf", &ScMemoryContext::HelperGetSystemIdtf)
    .def("HelperCheckEdge", &ScMemoryContext::HelperCheckEdge)
    .def("HelperGenTemplate", impl::_context_helperGenTemplate)
    .def("HelperSearchTemplate", impl::_context_helperSearchTemplate)
    .def("HelperBuildTemplate", impl::_context_helperBuildTemplate)
    ;

  bp::class_<impl::PyIterator3>("ScIterator3")
    .def("Next", &impl::PyIterator3::Next)
    .def("IsValid", &impl::PyIterator3::IsValid)
    .def("Get", &impl::PyIterator3::Get)
    ;

  bp::class_<impl::PyIterator5>("ScIterator5")
    .def("Next", &impl::PyIterator5::Next)
    .def("IsValid", &impl::PyIterator5::IsValid)
    .def("Get", &impl::PyIterator5::Get)
    ;

  bp::class_<impl::PyLinkContent>("ScLinkContent")
    .def("AsString", &impl::PyLinkContent::AsString)
    .def("AsInt", &impl::PyLinkContent::AsInt)
    .def("AsFloat", &impl::PyLinkContent::AsDouble)
    ;

  bp::class_<impl::PyTemplateGenResult>("ScTemplateGenResult")
    .def("Size", &impl::PyTemplateGenResult::Size)
    .def("__getitem__", &impl::PyTemplateGenResult::Get)
    ;

  bp::class_<impl::PyTemplateSearchResultItem>("ScTemplateSearchResultItem")
    .def("Size", &impl::PyTemplateSearchResultItem::Size)
    .def("__getitem__", &impl::PyTemplateSearchResultItem::Get)
    ;

  bp::class_<impl::PyTemplateSearchResult>("ScTemplateSearchResult")
    .def("Size",&impl::PyTemplateSearchResult::Size)
    .def("__getitem__", &impl::PyTemplateSearchResult::Get)
    ;

  bp::class_<impl::PyTemplateItemValue>("ScTemplateItemValue")
    ;

  bp::class_<ScTemplateGenParams>("ScTemplateGenParams")
    .def("Add", impl::_templateGenParamsAdd)
    .def("Get", impl::_templateGenParamsGet)
    .def("IsEmpty", &ScTemplateGenParams::IsEmpty)
    ;

  bp::class_<ScTemplate>("ScTemplate")
    .def("Triple", impl::_templateTriple)
    .def("TripleWithRelation", impl::_templateTripleWithRelation)
    ;

  bp::class_<ScAddr>("ScAddr", bp::init<>())
    .def("IsValid", &ScAddr::IsValid)
    .def("ToInt", &ScAddr::Hash)
    .def("__eq__", &ScAddr::operator==)
    .def("__ne__", &ScAddr::operator!=)
    .def("__rshift__", impl::_scAddrToRShift)
    .def("rshift", impl::_scAddrToRShift)
    ;

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
    .def("IsValid", &ScType::IsValid)
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
    .def_readonly("NodeVarMaterial", &ScType::NodeVarMaterial)
    ;
}


} // namespace 

namespace py
{

void ScPythonMemoryModule::Initialize()
{
  PyImport_AppendInittab("sc", &PyInit_sc);//&initsc);
}

} // namespace py