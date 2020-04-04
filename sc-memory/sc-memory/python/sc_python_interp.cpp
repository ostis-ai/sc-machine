
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
#include "sc-core/sc-store/sc_config.h"
}

namespace
{

std::unordered_set<std::string> gAddedModulePaths;

template<typename Func, typename... Args>
void CallPythonFunctionNoGIL(Func & f, Args... args)
{
  f(args...);
}

template<typename Func, typename... Args>
void CallPythonFunction(Func & f, Args... args)
{
  py::WithGIL gil;
  CallPythonFunctionNoGIL(f, args...);
}

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

  std::vector<std::string> values;
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


class PyBridgeWrap
{
  using EventsMap = std::unordered_map<PyScEvent::EventID, boost::shared_ptr<PyScEvent>>;

public:
  static boost::shared_ptr<PyBridgeWrap> Create()
  {
    return boost::shared_ptr<PyBridgeWrap>(new PyBridgeWrap());
  }

  PyBridgeWrap()
    : m_ctx(sc_access_lvl_make_max, "PyBridgeWrap")
  {
  }

  ~PyBridgeWrap()
  {
    for (auto it : m_events)
      ClearEvent(it.second);

    m_events.clear();
    if (m_impl.get())
      m_impl->m_closeDelegate = py::ScPythonBridge::CloseFunc();
  }

  void SetImpl(py::ScPythonBridgePtr impl)
  {
    m_impl = impl;

    m_impl->m_closeDelegate = std::bind(&PyBridgeWrap::OnCloseRequest, this);
  }

  void ClearEvent(boost::shared_ptr<PyScEvent> & evt)
  {
    evt->m_onDestroy = PyScEvent::OnDestroyFunc();
    evt->m_onEvent = PyScEvent::OnEventFunc();

    evt->Destroy();
  }

  void SetImpl(py::ScPythonBridgePtr impl) const
  {
    m_impl = impl;
  }

  void Ready()
  {
    m_impl->PythonReady();
  }

  void Finish()
  {
    m_impl->PythonFinish();
  }

  bool IsExist() const
  {
    return m_impl->IsInitialized();
  }

  // calls from PyScEvent to request emit it in main thread
  void EmitEvent(PyScEvent::EmitParams const & params)
  {
    if (m_eventDelegate)
    {
      py::WithGIL gil;
      CallPythonFunctionNoGIL(
        m_eventDelegate,
        bp::object(params.m_id),
        bp::object(params.m_addr),
        bp::object(params.m_edgeAddr),
        bp::object(params.m_otherAddr));
    }
  }

  void DestroyEvent(PyScEvent::EventID evtID)
  {
    utils::ScLockScope scope(m_eventsLock);
    auto it = m_events.find(evtID);

    if (it == m_events.end())
    {
      SC_THROW_EXCEPTION(utils::ExceptionItemNotFound,
                         "Can't find event with ID: " + std::to_string(evtID));
    }

    ClearEvent(it->second);
    m_events.erase(it);
  }

  boost::shared_ptr<PyScEvent> SubscribeEvent(ScAddr const & elAddr, ScEvent::Type evtType)
  {
    utils::ScLockScope scope(m_eventsLock);

    ScEvent * scEvt = new ScEvent(m_ctx, elAddr, evtType);
    boost::shared_ptr<PyScEvent> const evt(new PyScEvent(scEvt, ++ms_idCounter));

    SC_ASSERT(m_events.find(evt->GetID()) == m_events.end(), ());
    m_events[evt->GetID()] = evt;

    evt->m_onDestroy = std::bind(&PyBridgeWrap::DestroyEvent, this, std::placeholders::_1);
    evt->m_onEvent = std::bind(&PyBridgeWrap::EmitEvent, this, std::placeholders::_1);

    return evt;
  }

  std::string GetInitParams() const
  {
    return m_impl->GetInitParams();
  }

protected:
  // calls from c++
  void OnCloseRequest()
  {
    py::WithGIL gil;
    if (!m_closeDelegate.is_none())
      CallPythonFunction(m_closeDelegate);
  }

private:
  static PyScEvent::EventID ms_idCounter;
  mutable py::ScPythonBridgePtr m_impl;
  ScMemoryContext m_ctx;

  utils::ScLock m_eventsLock;
  EventsMap m_events;

public:
  // delegates that will be used in python module
  bp::object m_closeDelegate;
  bp::object m_eventDelegate;
};

PyScEvent::EventID PyBridgeWrap::ms_idCounter = 0;

// Logging
class PythonLog
{
public:
  void Write(std::string const & str)
  {
    // python sends \n after each print call, so just skip it
    if (str == "\n")
      return;

    SC_LOG_PYTHON(str);
  }

  void Flush() {}
};

class PythonLogError
{
public:
  void Write(std::string const & str)
  {
    // python sends \n after each print call, so just skip it
    if (str == "\n")
      return;

    SC_LOG_PYTHON_ERROR(str);
  }

  void Flush() {}
};

} // namespace

  // small boost python module for bridge utils
BOOST_PYTHON_MODULE(scb)
{
  bp::register_ptr_to_python<boost::shared_ptr<PyScEvent>>();
  bp::register_ptr_to_python<boost::shared_ptr<PyBridgeWrap>>();

  bp::class_<PythonLog>("CppLog", bp::init<>())
    .def("write", bp::make_function(&PythonLog::Write))
    .def("flush", bp::make_function(&PythonLog::Flush));

  bp::class_<PythonLogError>("CppLogError", bp::init<>())
    .def("write", bp::make_function(&PythonLogError::Write))
    .def("flush", bp::make_function(&PythonLogError::Flush));

  bp::enum_<ScEvent::Type>("ScPythonEventType")
    .value("AddInputEdge", ScEvent::Type::AddInputEdge)
    .value("AddOutputEdge", ScEvent::Type::AddOutputEdge)
    .value("ContentChanged", ScEvent::Type::ContentChanged)
    .value("EraseElement", ScEvent::Type::EraseElement)
    .value("RemoveInputEdge", ScEvent::Type::RemoveInputEdge)
    .value("RemoveOutputEdge", ScEvent::Type::RemoveOutputEdge)
    ;

  bp::class_<PyScEvent, boost::noncopyable>("ScPythonEvent", bp::no_init)
    .def("Destroy", bp::make_function(&PyScEvent::Destroy))
    .def("GetID", bp::make_function(&PyScEvent::GetID))
    ;

  bp::class_<PyBridgeWrap, boost::noncopyable>("ScPythonBridge", bp::no_init)
    .def("Ready", bp::make_function(&PyBridgeWrap::Ready))
    .def("Finish", bp::make_function(&PyBridgeWrap::Finish))
    .def("SubscribeEvent", bp::make_function(&PyBridgeWrap::SubscribeEvent))
    .def("DestroyEvent", bp::make_function(&PyBridgeWrap::DestroyEvent))
    .def_readwrite("onClose", &PyBridgeWrap::m_closeDelegate)
    .def_readwrite("onEvent", &PyBridgeWrap::m_eventDelegate)
    .def("InitParams", bp::make_function(&PyBridgeWrap::GetInitParams))
    ;
}

namespace py
{

bool ScPythonInterpreter::ms_isInitialized = false;
std::wstring ScPythonInterpreter::ms_name;
utils::ScLock ScPythonInterpreter::ms_lock;

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
  SC_LOG_INFO("Collect python modules...");
  CollectModules(modulePaths);
  SC_LOG_INFO("Collected " << ms_foundModules.size() << " python modules");

  ms_isInitialized = true;
  return true;
}

void ScPythonInterpreter::Shutdown()
{
  SC_ASSERT(gMainThread != nullptr, ());
  gMainThread = nullptr;

  ms_modulePaths.clear();
  ms_foundModules.clear();

  ms_isInitialized = false;
}

void ScPythonInterpreter::RunScript(std::string const & scriptName, ScMemoryContext const & ctx, ScPythonBridgePtr bridge /* = nullptr */)
{
  ScPythonSubThread subThreadScope;

  std::string modulePath, moduleName;
  {
    utils::ScLockScope scope(ms_lock);

    AddModuleSearchPaths(ms_modulePaths);

    auto const it = ms_foundModules.find(scriptName);
    if (it == ms_foundModules.end())
    {
      SC_THROW_EXCEPTION(utils::ExceptionItemNotFound,
                         "Can't find " << scriptName << " module");
    }

    moduleName = it->first;
    modulePath = it->second;
  }

  boost::filesystem::path p(modulePath);
  p /= moduleName;
  std::string const filePath = p.string();

  //PyEvalLock lock;
  bp::object mainModule((bp::handle<>(bp::borrowed(PyImport_AddModule("__main__")))));
  bp::object mainNamespace = mainModule.attr("__dict__");
  try
  {
    bp::dict globalNamespace;
    globalNamespace["__builtins__"] = mainNamespace["__builtins__"];
    std::stringstream initCode;
    initCode
      << "from scb import *" << std::endl
      << "from sc import *" << std::endl
      << "import sys" << std::endl
      << "sys.path.append('" << p.parent_path().string() << "')" << std::endl
      << "sys.stdout = CppLog()" << std::endl
      << "sys.stderr = CppLogError()" << std::endl;
    bp::exec(initCode.str().c_str(), globalNamespace, globalNamespace);

    std::unique_ptr<PyBridgeWrap> bridgeWrap;
    if (bridge.get())
    {
      bridgeWrap.reset(new PyBridgeWrap());
      bridgeWrap->SetImpl(bridge);
      globalNamespace["__cpp_bridge__"] = bp::ptr(bridgeWrap.get());
    }
    else
      globalNamespace["__cpp_bridge__"] = bp::object();

    globalNamespace["__ctx__"] = bp::ptr(&ctx);
    globalNamespace["__file__"] = filePath;

    // prepare module name
    std::string name = moduleName;
    if (utils::StringUtils::EndsWith(moduleName, ".py"))
      name = moduleName.substr(0, moduleName.size() - 3);

    globalNamespace["__name__"] = utils::StringUtils::ReplaceAll(name, "/", ".");

    bp::object resultObj(bp::exec_file(filePath.c_str(), globalNamespace, globalNamespace));
    bp::exec("import gc\ngc.collect()", globalNamespace, globalNamespace);

    globalNamespace.clear();
    SC_LOG_DEBUG("Clear python module run " + filePath);
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
  utils::ScLockScope scope(ms_lock);

  if (ms_modulePaths.insert(modulesPath).second)
    CollectModulesInPath(modulesPath);
}

ScPythonInterpreter::ModulesMap ScPythonInterpreter::GetFoundModules()
{
  utils::ScLockScope scope(ms_lock);
  return ms_foundModules;
}

void ScPythonInterpreter::CollectModules(ModulePathSet const & modulePath)
{
  for (auto const & p : modulePath)
    AddModulesPath(p);
}
void ScPythonInterpreter::CollectModulesInPath(std::string const & modulePath)
{
  boost::filesystem::path const root(modulePath);

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

} // namesapce py
