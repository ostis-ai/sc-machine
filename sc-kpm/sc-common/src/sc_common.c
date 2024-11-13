/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_common.h"

// --------------------- Module ------------------------

sc_result sc_module_initialize()
{
  // TODO(NikitaZotov): All test targets for agents in the machine directly depend on the targets of the modules where
  // these agents are implemented. That is, they are not loaded as extensions. This is because it is more convenient to
  // test agents in tests: when loading an extension, the whole module with all agents is loaded, and when linking it,
  // only the ones to be tested are loaded. In sc-kpm, all targets for tests depend on extensions with tests, and these
  // extensions depend on sc-kpm-common, which has been made as an extension. If it's not made as an extension, the
  // tests don't link completely on ubuntu. We'll have to think about how to make it so that we don't have to make such
  // dependencies in the future. Perhaps we should think about how to load not all agents from modules for tests.
  return SC_RESULT_OK;
}

sc_result sc_module_shutdown()
{
  return SC_RESULT_OK;
}
