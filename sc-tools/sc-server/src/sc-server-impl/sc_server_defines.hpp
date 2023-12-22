/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <websocketpp/server.hpp>
#include <websocketpp/config/core.hpp>
#include <websocketpp/config/asio_no_tls.hpp>

#include <set>

#include "sc-memory/utils/sc_console.hpp"
#include "sc-memory/sc_addr.hpp"

using ScServerPort = size_t;
using ScServerUserProcessId = websocketpp::connection_hdl;
using ScServerUserProcesses = std::map<ScServerUserProcessId, ScAddr, std::owner_less<ScServerUserProcessId>>;

using ScServerLogLevel = websocketpp::log::level;
using ScServerDefaultErrorLogLevel = websocketpp::log::elevel;

struct ScServerErrorLevel : ScServerDefaultErrorLogLevel
{
  /// Special aggregate value representing "no levels"
  static ScServerLogLevel const none = 0x0;
  /// Low level debugging information (warning: very chatty)
  static ScServerLogLevel const debug = 0x1;
  /// Information about unusual system states or other minor internal library
  /// problems, less chatty than devel.
  static ScServerLogLevel const library = 0x2;
  /// Information about minor configuration problems or additional information
  /// about other warnings.
  static ScServerLogLevel const info = 0x4;
  /// Information about important problems not severe enough to terminate
  /// connections.
  static ScServerLogLevel const warning = 0x8;
  /// Recoverable error. Recovery may mean cleanly closing the connection with
  /// an appropriate error code to the remote endpoint.
  static ScServerLogLevel const error = 0x10;
  /// Unrecoverable error. This error will trigger immediate unclean
  /// termination of the connection or endpoint.
  static ScServerLogLevel const fatal = 0x20;
  /// Special aggregate value representing "all levels"
  static ScServerLogLevel const all = 0xffffffff;

  static char const * channel_name(ScServerLogLevel channel)
  {
    switch (channel)
    {
    case debug:
      return "Debug";
    case library:
      return "Library";
    case info:
      return "Info";
    case warning:
      return "Warning";
    case error:
      return "Error";
    case fatal:
      return "Fatal";
    default:
      return "Unknown";
    }
  }
};

namespace websocketpp::log
{
template <typename concurrency, typename names>
class ScServerDefaultLogger : public basic<concurrency, names>
{
public:
  typedef basic<concurrency, names> base;

  ScServerDefaultLogger<concurrency, names>(channel_type_hint::value hint = channel_type_hint::access)
    : basic<concurrency, names>(hint)
    , m_out(&std::cout)
  {
  }

  ScServerDefaultLogger<concurrency, names>(level channels, channel_type_hint::value hint = channel_type_hint::access)
    : basic<concurrency, names>(0xffffffff, hint)
    , m_out(&std::cout)
  {
  }

  void set_ostream(std::ostream * out = &std::cout)
  {
    m_out = out;
  }

  void write(ScServerLogLevel channel, std::string const & msg)
  {
    write(channel, msg.c_str());
  }

  void write(ScServerLogLevel channel, char const * msg)
  {
    scoped_lock_type lock(m_lock);
    if (!this->dynamic_test(channel))
      return;

    ScConsole::SetColor(ScConsole::Color::Reset);
    ScConsole::Color const color = get_color(channel);
    ScConsole::SetColor(color);
    *m_out << "\x1b[1m[" << timestamp << "]"
           << "[" << names::channel_name(channel) << "]:\x1b[0m ";
    ScConsole::SetColor(color);
    *m_out << "[sc-server] " << msg << "\n";
    ScConsole::SetColor(ScConsole::Color::Reset);

    m_out->flush();
  }

  ScConsole::Color get_color(ScServerLogLevel channel)
  {
    switch (channel)
    {
    case ScServerErrorLevel::debug:
      return ScConsole::Color::LightBlue;
    case ScServerErrorLevel::library:
    case ScServerErrorLevel::info:
      return ScConsole::Color::Grey;
    case ScServerErrorLevel::warning:
      return ScConsole::Color::Yellow;
    case ScServerErrorLevel::error:
    case ScServerErrorLevel::fatal:
      return ScConsole::Color::Red;
    default:
      return ScConsole::Color::White;
    }
  }

private:
  typedef typename base::scoped_lock_type scoped_lock_type;
  typedef typename concurrency::mutex_type mutex_type;
  mutex_type m_lock;

  static std::ostream & timestamp(std::ostream & os)
  {
    std::time_t t = std::time(nullptr);
    std::tm lt = lib::localtime(t);
    char buffer[20];
    size_t result = std::strftime(buffer, sizeof(buffer), "%H:%M:%S", &lt);
    return os << (result == 0 ? "Unknown" : buffer);
  }

  std::ostream * m_out;
};
}  // namespace websocketpp::log

struct ScServerConfig : public websocketpp::config::asio
{
  typedef websocketpp::log::ScServerDefaultLogger<concurrency_type, ScServerErrorLevel> elog_type;
  typedef websocketpp::log::ScServerDefaultLogger<concurrency_type, ScServerErrorLevel> alog_type;

  struct ScServerTransportConfig : public websocketpp::config::asio::transport_config
  {
    typedef ScServerConfig::elog_type elog_type;
    typedef ScServerConfig::alog_type alog_type;
  };

  typedef websocketpp::transport::asio::endpoint<ScServerTransportConfig> transport_type;
};

using ScServerCore = websocketpp::server<ScServerConfig>;
using ScServerMessage = ScServerCore::message_ptr;

using ScServerMessageType = websocketpp::frame::opcode::value;
using ScServerCloseCode = websocketpp::close::status::value;

using ScServerException = websocketpp::exception;

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::placeholders::_3;

#define SC_SERVER_DEBUG_LEVEL "Debug"
#define SC_SERVER_INFO_LEVEL "Info"
#define SC_SERVER_ERROR_LEVEL "Error"

#define SC_SERVER_CONSOLE_TYPE "Console"
#define SC_SERVER_FILE_TYPE "File"
