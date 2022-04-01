#pragma once

#include "../sc_utils.hpp"
#include "../utils/sc_lock.hpp"

#include "sc_python_bridge.hpp"

#include <string>
#include <memory>

namespace py
{
class ScPythonInterpreter
{
public:
  using ModulesMap = std::unordered_map<std::string, std::string>;
  using ModulePathSet = std::set<std::string>;

  _SC_EXTERN static bool Initialize(std::string const & name);
  _SC_EXTERN static void Shutdown();

  /* This function runs specified script. Path to script should be relative to
   * python.modules_path config value.
   * This function is a thread safe
   */
  _SC_EXTERN static void RunScript(
      std::string const & scriptName,
      ScMemoryContext const & ctx,
      ScPythonBridgePtr bridge = ScPythonBridgePtr());

  _SC_EXTERN static void AddModulesPath(std::string const & modulesPath);

  static ModulesMap GetFoundModules();

protected:
  static void CollectModules(ModulePathSet const & modulePath);
  static void CollectModulesInPath(std::string const & path);

private:
  static bool ms_isInitialized;
  static std::wstring ms_name;
  static utils::ScLock ms_lock;

  static ModulesMap ms_foundModules;
  static ModulePathSet ms_modulePaths;
};

}  // namespace py
