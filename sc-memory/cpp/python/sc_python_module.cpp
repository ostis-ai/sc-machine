#include "sc_python_module.hpp"

#define BOOST_PYTHON_STATIC_LIB 
#include <boost/python.hpp>

#include "../sc_memory.hpp"
#include "../sc_stream.hpp"

namespace
{
namespace bp = boost::python;

namespace impl
{

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
  {
    std::vector<char> buff(stream.Size());
    size_t readBytes;
    if (stream.Read((sc_char*)buff.data(), stream.Size(), readBytes))
      return bp::object(std::string(buff.begin(), buff.end()));
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

} // namespace impl

BOOST_PYTHON_MODULE(sc)
{
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

  bp::class_<ScAddr>("ScAddr", bp::init<>())
    .def("IsValid", &ScAddr::IsValid)
    .def("ToInt", &ScAddr::Hash)
    .def("__eq__", &ScAddr::operator==)
    .def("__ne__", &ScAddr::operator!=)
    ;

  bp::class_<ScType>("ScType", bp::init<>())
    .def(bp::init<ScType::RealType>())
    .def("__eq__", &ScType::operator==)
    .def("__ne__", &ScType::operator!=)
    .def("__or__", &ScType::operator|)
    .def("__and__", &ScType::operator&)
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
  PyImport_AppendInittab("sc", &initsc);
}

} // namespace py