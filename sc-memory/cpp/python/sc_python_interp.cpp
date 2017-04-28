
#include "sc_python_interp.hpp"
#include "sc_python_module.hpp"

#include "sc_python_includes.hpp"

#include "../sc_debug.hpp"
#include "../sc_utils.hpp"

#include "../utils/sc_cache.hpp"

#include <boost/filesystem.hpp>



namespace bp = boost::python;


extern "C"
{
#include <sc-memory/sc-store/sc_config.h>
}

namespace
{

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

} // namespace 

namespace py
{

bool ScPythonInterpreter::ms_isInitialized = false;
std::wstring ScPythonInterpreter::ms_name;
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
  
  PyEval_InitThreads();
  gMainThreadState = PyThreadState_Get();
  PyEval_ReleaseLock();

  ScPythonMemoryModule::Initialize();

  SC_LOG_INIT("Initialize python iterpreter version " << PY_VERSION);
  SC_LOG_INFO("Collect modules...");
  CollectModules();
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
  auto const it = ms_foundModules.find(scriptName);
  if (it == ms_foundModules.end())
  {
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound,
                       "Can't find " << scriptName << " module");
  }

  boost::filesystem::path p(it->second);
  p /= it->first;
  std::string const filePath = p.string();

  std::ifstream inputfile(filePath);
  std::string fileContent((std::istreambuf_iterator<char>(inputfile)),
                           std::istreambuf_iterator<char>());

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

  PyEvalLock lock;

  /*PyObjectWrap globals(PyDict_New());
  PyDict_SetItemString(*globals, "__builtins__", PyEval_GetBuiltins());*/
  //PyObjectWrap globalsDict(PyDict_New());
  //PyDict_SetItemString(*globals, "__dict__", *globalsDict);

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
  if (ms_modulePaths.insert(modulesPath).second)
    CollectModulesInPath(modulesPath);
}

void ScPythonInterpreter::CollectModules()
{
  // load search paths
  char const * pValue = sc_config_get_value_string("python", "modules_path");
  if (pValue == nullptr)
    return;

  std::string const paths = pValue;

  StringVector values;
  utils::StringUtils::SplitString(paths, ';', values);
  for (auto const & p : values)
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
        std::string const filename = boost::filesystem::relative(p, root).string();
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