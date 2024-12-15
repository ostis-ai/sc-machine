#include "sc-memory/utils/sc_log.hpp"

#include "sc-memory/utils/sc_message.hpp"

namespace utils
{

template <typename T, typename... ARGS>
void ScLog::Error(T const & t, ARGS const &... others)
{
  Message(ScLogLevel::Level::Error, utils::impl::Message(t, others...), ScConsole::Color::Red);
}

template <typename T, typename... ARGS>
void ScLog::Warning(T const & t, ARGS const &... others)
{
  Message(ScLogLevel::Level::Warning, utils::impl::Message(t, others...), ScConsole::Color::Yellow);
}

template <typename T, typename... ARGS>
void ScLog::Info(T const & t, ARGS const &... others)
{
  Message(ScLogLevel::Level::Info, utils::impl::Message(t, others...), ScConsole::Color::Grey);
}

template <typename T, typename... ARGS>
void ScLog::Debug(T const & t, ARGS const &... others)
{
  Message(ScLogLevel::Level::Info, utils::impl::Message(t, others...), ScConsole::Color::LightBlue);
}

}  // namespace utils
