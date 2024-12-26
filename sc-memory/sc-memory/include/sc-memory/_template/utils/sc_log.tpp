/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/utils/sc_logger.hpp"

#include "sc-memory/utils/sc_message.hpp"

namespace utils
{

template <typename T, typename... ARGS>
void ScLogger::Error(T const & t, ARGS const &... others)
{
  Message(ScLogLevel::Level::Error, utils::impl::Message(t, others...), ScConsole::Color::Red);
}

template <typename T, typename... ARGS>
void ScLogger::Warning(T const & t, ARGS const &... others)
{
  Message(ScLogLevel::Level::Warning, utils::impl::Message(t, others...), ScConsole::Color::Yellow);
}

template <typename T, typename... ARGS>
void ScLogger::Info(T const & t, ARGS const &... others)
{
  Message(ScLogLevel::Level::Info, utils::impl::Message(t, others...), ScConsole::Color::Grey);
}

template <typename T, typename... ARGS>
void ScLogger::Debug(T const & t, ARGS const &... others)
{
  Message(ScLogLevel::Level::Debug, utils::impl::Message(t, others...), ScConsole::Color::LightBlue);
}

}  // namespace utils
