/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <QtCore/QCoreApplication>
#include "sctpServer.h"

#include <sc-memory/utils/sc_signal_handler.hpp>

int main(int argc, char *argv[]) try
{
  QCoreApplication a(argc, argv);

  QString config("config.ini");
  if (argc > 1)
    config = argv[1];

  sctpServer server;
  utils::ScSignalHandler::Initialize();
  utils::ScSignalHandler::m_onTerminate = [&server]()
  {
    server.stop();
  };

  if (!server.start(config))
    exit(1);

  //QObject::connect(&a, SIGNAL(aboutToQuit()), &server, SLOT(stop()));

  return a.exec();
}
catch (utils::ScException const & ex)
{
  SC_LOG_ERROR(ex.Message());
}
catch (std::exception const & ex)
{
  SC_LOG_ERROR(ex.what());
}
