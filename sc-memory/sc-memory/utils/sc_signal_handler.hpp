/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "../sc_defines.hpp"

#include <functional>

namespace utils
{
/* This class used to handle system signals such as
 * Ctrl + C, Ctrl + Break and etc.
 */
class ScSignalHandler
{
public:
  using HandlerDelegate = std::function<void()>;

  _SC_EXTERN static void Initialize();

  // handler for Ctcl+C and Ctrl + Break
  _SC_EXTERN static HandlerDelegate m_onTerminate;
};

}  // namespace utils
