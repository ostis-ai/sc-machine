
#include "sc_python_interp.hpp"
#include "sc_python_module.hpp"

#include "sc_python_includes.hpp"

#include "../sc_memory.hpp"
#include "../sc_debug.hpp"
#include "../sc_utils.hpp"

#include "../utils/sc_boost.hpp"
#include "../utils/sc_cache.hpp"

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

struct PyEvalLock
{
  PyEvalLock()
  {
    PyEval_AcquireLock();
  }

  ~PyEvalLock()
  {
    PyEval_ReleaseLock();
  }
};

void PyLoadModulePathFromConfig(StringVector & outValues)
{
  // load search paths
  char const * pValue = sc_config_get_value_string("python", "modules_path");
  if (pValue == nullptr)
    return;

  std::string const paths = pValue;
  utils::StringUtils::SplitString(paths, ';', outValues);
  for (auto & v : outValues)
    v = utils::StringUtils::ReplaceAll(v, "\\", "/");
}

void AddModuleSearchPaths(StringVector const & modulePath)
{
  PyObject* sysPath = PySys_GetObject("path");
  for (auto const & p : modulePath)
  {
    if (gAddedModulePaths.find(p) != gAddedModulePaths.end())
    {
      PyList_Insert(sysPath, 0, PyUnicode_DecodeLocale(p.c_str(), nullptr));
      gAddedModulePaths.insert(p);
    }
  }
}

} // namespace 

namespace py
{

bool ScPythonInterpreter::ms_isInitialized = false;
std::wstring ScPythonInterpreter::ms_name;
utils::ScLock ScPythonInterpreter::m_lock;

ScPythonInterpreter::ModulesMap ScPythonInterpreter::ms_foundModules;
ScPythonInterpreter::ModulePathSet ScPythonInterpreter::ms_modulePaths;

PyThreadState * gMainThreadState = nullptr;

bool ScPythonInterpreter::Initialize(std::string const & name)
{
  SC_ASSERT(!ms_isInitialized, ("You can't initialize this class twicely."));
  ms_name.assign(name.begin(), name.end());

  // TODO: more clear solution
  Py_SetProgramName((wchar_t*)ms_name.c_str());
  Py_Initialize();

  StringVector modulePath;
  PyLoadModulePathFromConfig(modulePath);
  AddModuleSearchPaths(modulePath);  
  
  PyEval_InitThreads();
  gMainThreadState = PyThreadState_Get();
  //PyEval_ReleaseLock();

  ScPythonMemoryModule::Initialize();

  SC_LOG_INIT("Initialize python iterpreter version " << PY_VERSION);
  SC_LOG_INFO("Collect modules...");
  CollectModules(modulePath);
  SC_LOG_INFO("Collected " << ms_foundModules.size() << " modules");

  ms_isInitialized = true;
  return true;
}

void ScPythonInterpreter::Shutdown()
{
  Py_Finalize();
  ms_isInitialized = false;
}

void ScPythonInterpreter::RunScript(std::string const & scriptName)
{
  utils::ScLockScope scope(m_lock);

  auto const it = ms_foundModules.find(scriptName);
  if (it == ms_foundModules.end())
  {
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound,
                       "Can't find " << scriptName << " module");
  }

  boost::filesystem::path p(it->second);
  p /= it->first;
  std::string const filePath = p.string();

  AddModuleSearchPaths({ it->second });
  
  std::ifstream inputfile(filePath);
  std::string fileContent((std::istreambuf_iterator<char>(inputfile)),
                           std::istreambuf_iterator<char>());

  //PyEval_RestoreThread(gMainThreadState);

  // now need to compile this file
  PyCompilerFlags flags;
  flags.cf_flags = 0;
  PyObjectWrap codeObj(Py_CompileStringFlags(fileContent.c_str(), filePath.c_str(), Py_file_input, &flags));
  if (*codeObj == nullptr)
  {
    PyErr_Print();
    SC_THROW_EXCEPTION(utils::ExceptionParseError,
                       "Can't parse file " << filePath);
  }

  //PyEvalLock lock;
  bp::object mainModule((bp::handle<>(bp::borrowed(PyImport_AddModule("__main__")))));
  bp::object mainNamespace = mainModule.attr("__dict__");

  PyObjectWrap resultObj(PyEval_EvalCode(*codeObj, mainNamespace.ptr(), mainNamespace.ptr()));
  if (!resultObj.IsValid())
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

void ScPythonInterpreter::CollectModules(StringVector const & modulePath)
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