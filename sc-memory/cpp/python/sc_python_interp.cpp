
#include "sc_python_threads.hpp"
#include "sc_python_module.hpp"
#include "sc_python_interp.hpp"
#include "sc_python_bridge.hpp"

#include "../sc_debug.hpp"
#include "../sc_event.hpp"
#include "../sc_memory.hpp"

#include "../utils/sc_boost.hpp"
#include "../utils/sc_cache.hpp"
#include "../utils/sc_lock.hpp"

namespace bp = boost::python;

extern "C"
{
#include <sc-memory/sc-store/sc_config.h>
}

namespace
{

std::unordered_set<std::string> gAddedModulePaths;

struct PyObjectWrap
{
  PyObjectWrap() : m_object(nullptr) {}
  explicit PyObjectWrap(PyObject * obj) : m_object(obj) {}
  ~PyObjectWrap() { Clear(); }

  void Clear()
  {
    if (m_object)
    {
      Py_XDECREF(m_object);
      m_object = nullptr;
    }
  }

  void Reset(PyObject * obj)
  {
    Clear();
    m_object = obj;
  }

  PyObject* operator * () const
  {
    return m_object;
  }

  bool IsValid() const
  {
    return (m_object != nullptr);
  }

  SC_DISALLOW_COPY_AND_MOVE(PyObjectWrap);

private:
  PyObject * m_object;
};

void PyLoadModulePathFromConfig(py::ScPythonInterpreter::ModulePathSet & outValues)
{
  // load search paths
  char const * pValue = sc_config_get_value_string("python", "modules_path");
  if (pValue == nullptr)
    return;

  StringVector values;
  std::string const paths = pValue;
  utils::StringUtils::SplitString(paths, ';', values);
  for (auto const & v : values)
    outValues.insert(utils::StringUtils::ReplaceAll(v, "\\", "/"));
}

void AddModuleSearchPaths(py::ScPythonInterpreter::ModulePathSet const & modulePath)
{
  PyObject* sysPath = PySys_GetObject("path");
  for (auto const & p : modulePath)
  {
    if (gAddedModulePaths.find(p) == gAddedModulePaths.end())
    {
      PyList_Insert(sysPath, 0, PyUnicode_FromString(p.c_str()));
      gAddedModulePaths.insert(p);
    }
  }
}

class PyBridgeWrap;

class PyScEvent
{
  friend class PyBridgeWrap;

public:
  using EventID = uint32_t;

protected:
  struct EmitParams
  {
    EventID m_id;
    ScAddr m_addr;
    ScAddr m_edgeAddr;
    ScAddr m_otherAddr;
  };

  explicit PyScEvent(ScEvent * evt, EventID id)
    : m_id(id)
  {
    SC_ASSERT(evt != nullptr, ("Should receive valid event pointer"));
    m_event.reset(evt);

    evt->SetDelegate(std::bind(&PyScEvent::OnEvent, this,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               std::placeholders::_3));
  }

public:

  ~PyScEvent()
  {
    Destroy();
  }

  void Destroy()
  {
    m_event.reset();

    if (m_onDestroy)
      m_onDestroy(m_id);
  }

  EventID GetID() const
  {
    return m_id;
  }

private:
  bool OnEvent(ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & otherAddr)
  {
    if (m_onEvent)
    {
      EmitParams params;
      params.m_id = m_id;
      params.m_addr = addr;
      params.m_edgeAddr = edgeAddr;
      params.m_otherAddr = otherAddr;

      m_onEvent(params);

      return true;
    }

    return false;
  }

protected:
  using OnEventFunc = std::function<void(EmitParams const &)>;
  using OnDestroyFunc = std::function<void(EventID)>;

  // just bridge wrapper can change it
  OnEventFunc m_onEvent;
  OnDestroyFunc m_onDestroy;

private:
  std::unique_ptr<ScEvent> m_event;
  EventID m_id;
};

class PyBridgeRequest
{
public:
  PyBridgeRequest() {}

  explicit PyBridgeRequest(py::ScPythonBridge::RequestPtr const & req)
    : m_impl(req)
  {
  }

  std::string GetName() const { return m_impl->GetName(); }
  std::string GetData() const { return m_impl->GetData(); }
  bool IsValid() const { return m_impl.get(); }

  void MakeResponse(py::ScPythonBridge::Response::Status status, std::string const & data)
  {
    m_impl->_OnMakeResponse(status, data);
  }

private:
  py::ScPythonBridge::RequestPtr m_impl;
};

class PyBridgeWrap
{
public:
  static boost::shared_ptr<PyBridgeWrap> Create()
  {
    return boost::shared_ptr<PyBridgeWrap>(new PyBridgeWrap());
  }

  PyBridgeWrap()
    : m_ctx(sc_access_lvl_make_max)
  {
  }

  ~PyBridgeWrap()
  {
    utils::ScLockScope scope(m_eventLock);  
    for (auto it : m_events)
      DestroyEvent(it.second);
    
    m_events.clear();
  }

  void DestroyEvent(boost::shared_ptr<PyScEvent> & evt)
  {
    evt->m_onDestroy = PyScEvent::OnDestroyFunc();
    evt->m_onEvent = PyScEvent::OnEventFunc();

    evt->Destroy();
  }

  void SetImpl(py::ScPythonBridgePtr impl) const
  {
    m_impl = impl;
  }

  void Initialize()
  {
    m_impl->Initialize();
  }

  bool IsExist() const
  {
    return m_impl->IsInitialized();
  }

  PyBridgeRequest GetRequest()
  {
    py::ScPythonBridge::RequestPtr request = m_impl->GetNextRequest();

    if (!request.get())
      return PyBridgeRequest();
    
    return PyBridgeRequest(request);
  }

  // calls from PyScEvent to request emit it in main thread
  void RequestEmit(PyScEvent::EmitParams const & params)
  {
    utils::ScLockScope scope(m_eventLock);
    
    std::string evtParams = "{";
    evtParams += "\"addr\":"  + std::to_string(params.m_addr.Hash());
    evtParams += ",\"edgeAddr\":" + std::to_string(params.m_edgeAddr.Hash());
    evtParams += ",\"otherAddr\":" + std::to_string(params.m_otherAddr.Hash());
    evtParams += ",\"id\":" + std::to_string(params.m_id);
    evtParams += "}";
    
    m_impl->DoRequest("ScEvent", evtParams);
  }

  void RequestDestroy(PyScEvent::EventID evtID)
  {
    utils::ScLockScope scope(m_eventLock);
    auto it = m_events.find(evtID);

    if (it == m_events.end())
    {
      SC_THROW_EXCEPTION(utils::ExceptionItemNotFound,
                         "Can't find event with ID: " + std::to_string(evtID));
    }

    DestroyEvent(it->second);
  }
  
  boost::shared_ptr<PyScEvent> SubscribeEvent(ScAddr const & elAddr, ScEvent::Type evtType)
  {
    utils::ScLockScope scope(m_eventLock);

    ScEvent * scEvt = new ScEvent(m_ctx, elAddr, evtType);
    boost::shared_ptr<PyScEvent> const evt(new PyScEvent(scEvt, ++ms_idCounter));
    
    SC_ASSERT(m_events.find(evt->GetID()) == m_events.end(), ());
    m_events[evt->GetID()] = evt;

    evt->m_onDestroy = std::bind(&PyBridgeWrap::RequestDestroy, this, std::placeholders::_1);
    evt->m_onEvent = std::bind(&PyBridgeWrap::RequestEmit, this, std::placeholders::_1);

    return evt;
  }
  
private:
  static PyScEvent::EventID ms_idCounter;
  mutable py::ScPythonBridgePtr m_impl;
  ScMemoryContext m_ctx;
  
  utils::ScLock m_eventLock;
  std::unordered_map<PyScEvent::EventID, boost::shared_ptr<PyScEvent>> m_events;
};

PyScEvent::EventID PyBridgeWrap::ms_idCounter = 0;

} // namespace 

  // small boost python module for bridge utils
BOOST_PYTHON_MODULE(scb)
{
  bp::register_ptr_to_python<boost::shared_ptr<PyScEvent>>();
  bp::register_ptr_to_python<boost::shared_ptr<PyBridgeWrap>>();

  bp::enum_<py::ScPythonBridge::Response::Status>("ResponseStatus")
    .value("Ok", py::ScPythonBridge::Response::Status::Ok)
    .value("Error", py::ScPythonBridge::Response::Status::Error)
    ;

  bp::enum_<ScEvent::Type>("ScPythonEventType")
    .value("AddInputEdge", ScEvent::Type::AddInputEdge)
    .value("AddOutputEdge", ScEvent::Type::AddOutputEdge)
    .value("ContentChanged", ScEvent::Type::ContentChanged)
    .value("EraseElement", ScEvent::Type::EraseElement)
    .value("RemoveInputEdge", ScEvent::Type::RemoveInputEdge)
    .value("RemoveOutputEdge", ScEvent::Type::RemoveOutputEdge)
    ;

  bp::class_<PyBridgeRequest>("ScPythonBridgeRequest", bp::no_init)
    .def("GetName", bp::make_function(&PyBridgeRequest::GetName, py::ReleaseGILPolicy()))
    .def("GetData", bp::make_function(&PyBridgeRequest::GetData, py::ReleaseGILPolicy()))
    .def("IsValid", bp::make_function(&PyBridgeRequest::IsValid, py::ReleaseGILPolicy()))
    .def("MakeResponse", bp::make_function(&PyBridgeRequest::MakeResponse, py::ReleaseGILPolicy()))
    ;

  bp::class_<PyBridgeWrap, boost::noncopyable>("ScPythonBridgeWrap", bp::no_init)
    .def("__init__", bp::make_constructor(&PyBridgeWrap::Create, py::ReleaseGILPolicy()))
    .def("Initialize", bp::make_function(&PyBridgeWrap::Initialize, py::ReleaseGILPolicy()))
    .def("Exist", bp::make_function(&PyBridgeWrap::IsExist, py::ReleaseGILPolicy()))
    .def("GetRequest", bp::make_function(&PyBridgeWrap::GetRequest, py::ReleaseGILPolicy()))
    .def("SubscribeEvent", bp::make_function(&PyBridgeWrap::SubscribeEvent, py::ReleaseGILPolicy()))
    ;

  bp::class_<PyScEvent, boost::noncopyable>("ScPythonEvent", bp::no_init)
    .def("Destroy", bp::make_function(&PyScEvent::Destroy, py::ReleaseGILPolicy()))
    .def("GetID", bp::make_function(&PyScEvent::GetID, py::ReleaseGILPolicy()))
    ;
}

namespace py
{

bool ScPythonInterpreter::ms_isInitialized = false;
std::wstring ScPythonInterpreter::ms_name;
utils::ScLock ScPythonInterpreter::m_lock;

ScPythonInterpreter::ModulesMap ScPythonInterpreter::ms_foundModules;
ScPythonInterpreter::ModulePathSet ScPythonInterpreter::ms_modulePaths;

ScPythonMainThread * gMainThread = nullptr;

bool ScPythonInterpreter::Initialize(std::string const & name)
{
  SC_ASSERT(!ms_isInitialized, ("You can't initialize this class twicely."));
  ms_name.assign(name.begin(), name.end());

  ScPythonMemoryModule::Initialize();
  PyImport_AppendInittab("scb", &PyInit_scb);

  SC_ASSERT(gMainThread == nullptr, ("ScPythonInterpreter already initialized"));
  gMainThread = new ScPythonMainThread();

  ModulePathSet modulePaths;
  PyLoadModulePathFromConfig(modulePaths);
  
  SC_LOG_INIT("Initialize python iterpreter version " << PY_VERSION);
  SC_LOG_INFO("Collect modules...");
  CollectModules(modulePaths);
  SC_LOG_INFO("Collected " << ms_foundModules.size() << " modules");

  ms_isInitialized = true;
  return true;
}

void ScPythonInterpreter::Shutdown()
{
  SC_ASSERT(gMainThread != nullptr, ());
  gMainThread = nullptr;

  ms_modulePaths.clear();
  
  ms_isInitialized = false;
}

void ScPythonInterpreter::RunScript(std::string const & scriptName, ScPythonBridgePtr bridge /* = nullptr */)
{
  utils::ScLockScope scope(m_lock);
  ScPythonSubThread subThreadScope;

  AddModuleSearchPaths(ms_modulePaths);

  auto const it = ms_foundModules.find(scriptName);
  if (it == ms_foundModules.end())
  {
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound,
                       "Can't find " << scriptName << " module");
  }

  boost::filesystem::path p(it->second);
  p /= it->first;
  std::string const filePath = p.string();

  //PyEvalLock lock;
  bp::object mainModule((bp::handle<>(bp::borrowed(PyImport_AddModule("__main__")))));
  bp::object mainNamespace = mainModule.attr("__dict__");
   
  try
  {
    bp::dict globalNamespace;
    globalNamespace["__builtins__"] = mainNamespace["__builtins__"];
    bp::exec("from scb import *\nfrom sc import *", globalNamespace, globalNamespace);
    
    if (bridge.get())
    {
      boost::shared_ptr<PyBridgeWrap> bridgeWrap(new PyBridgeWrap());
      bridgeWrap->SetImpl(bridge);
      globalNamespace["cpp_bridge"] = bp::object(bridgeWrap);
    }
    else
      globalNamespace["cpp_bridge"] = bp::object();

    //bp::exec("from scb import *\ncpp_bridge = ScPythonBridgeWrap()", globalNamespace, localNamespace);

    /*bp::object b = mainNamespace["cpp_bridge"];
    bp::extract<PyBridgeWrap> be(b);
    if (be.check())
    {
      PyBridgeWrap const & bImpl = be;
      bImpl.SetImpl(bridge);
    }*/

    bp::object resultObj(bp::exec_file(filePath.c_str(), globalNamespace, globalNamespace));  
    bp::exec("import gc\ngc.collect()", globalNamespace, globalNamespace);
  }
  catch (...)
  {
    PyErr_Print();
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState,
                       "Error during code run " << filePath);
  }
}

void ScPythonInterpreter::AddModulesPath(std::string const & modulesPath)
{
  utils::ScLockScope scope(m_lock);

  if (ms_modulePaths.insert(modulesPath).second)
    CollectModulesInPath(modulesPath);
}

void ScPythonInterpreter::CollectModules(ModulePathSet const & modulePath)
{
  for (auto const & p : modulePath)
    AddModulesPath(p);
}
void ScPythonInterpreter::CollectModulesInPath(std::string const & modulePath)
{
  boost::filesystem::path const root(modulePath);
  try
  {
    boost::filesystem::recursive_directory_iterator itEnd, itPath(modulePath);
  
    while (itPath != itEnd)
    {
      if (!boost::filesystem::is_directory(*itPath))
      {
        boost::filesystem::path const p = *itPath;
        std::string filename = utils::StringUtils::ReplaceAll(boost::filesystem::relativePath(root, p).string(), "\\", "/");
        std::string ext = utils::StringUtils::GetFileExtension(filename);
        utils::StringUtils::ToLowerCase(ext);

        if (ext == "py")
        {
          auto const itModule = ms_foundModules.find(filename);
          if (itModule != ms_foundModules.end())
          {
            SC_THROW_EXCEPTION(utils::ExceptionInvalidState,
                               "Module " << itModule->first << " already exist in " << itModule->second);
          }

          ms_foundModules.insert(std::make_pair(filename, modulePath));
        }        
      }

      try
      {
        ++itPath;
      }
      catch (std::exception & ex)
      {
        SC_LOG_ERROR(ex.what());
        itPath.no_push();
        try
        {
          ++itPath;
        }
        catch (...)
        {
          SC_LOG_ERROR(ex.what());
          return;
        }
      }
    }
  }
  catch (std::exception & ex)
  {
    SC_LOG_ERROR(ex.what());
  }
}

} // namesapce py
