/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_signal_handler.hpp"

#include "../sc_platform.hpp"
#include "../sc_debug.hpp"

#if SC_IS_PLATFORM_WIN32
#  include <windows.h>
#elif SC_IS_PLATFORM_LINUX || SC_IS_PLATFORM_MAC
#  include <csignal>
#else
#  error "Not supported platform"
#endif

namespace handlers
{
#if SC_IS_PLATFORM_WIN32
BOOL CtrlHandler(DWORD fdwCtrlType)
{
  switch (fdwCtrlType)
  {
  // Handle the CTRL-C signal.
  case CTRL_C_EVENT:
  // CTRL-CLOSE: confirm that the user wants to exit.
  case CTRL_CLOSE_EVENT:
  case CTRL_BREAK_EVENT:
  {
    if (utils::ScSignalHandler::m_onTerminate)
      utils::ScSignalHandler::m_onTerminate();
    return (TRUE);
  }

  case CTRL_LOGOFF_EVENT:
  case CTRL_SHUTDOWN_EVENT:
    return FALSE;

  default:
    return FALSE;
  }
}
#elif SC_IS_PLATFORM_LINUX || SC_IS_PLATFORM_MAC
void sc_signal_handler(int s)
{
  if (utils::ScSignalHandler::m_onTerminate)
    utils::ScSignalHandler::m_onTerminate();
}
#else
#  error "Not supported platform"
#endif
}  // namespace handlers

namespace utils
{
ScSignalHandler::HandlerDelegate ScSignalHandler::m_onTerminate;

void ScSignalHandler::Initialize()
{
  SC_LOG_INIT("Signal handler");
#if SC_IS_PLATFORM_WIN32
  if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)handlers::CtrlHandler, TRUE))
  {
    SC_LOG_INFO("Signal handler initialized");
  }
  else
  {
    SC_LOG_ERROR("Can't initialize signal handler");
  }
#elif SC_IS_PLATFORM_LINUX || SC_IS_PLATFORM_MAC
  struct sigaction sigIntHandler
  {
  };

  sigIntHandler.sa_handler = handlers::sc_signal_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, nullptr);
#else
#  error "Unsupported platform"
#endif
}

}  // namespace utils
