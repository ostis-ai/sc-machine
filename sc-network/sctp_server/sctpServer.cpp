/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sctpServer.h"
#include "sctpClient.h"
#include "sctpStatistic.h"
#include "sctpEventManager.h"

#include <QSettings>
#include <QDebug>
#include <QThreadPool>
#include <QTimer>

#include <iostream>
#include "sc-memory/sc_memory.hpp"
#include "utils/parser.hpp"
#include <map>

#include <QNetworkInterface>


sctpServer::sctpServer(QObject * parent)
  : QTcpServer(parent)
  , mPort(0)
  , mStatistic(0)
  , mSavePeriod(0)
  , mEventManager(0)
{
}

sctpServer::~sctpServer()
{
  if (mStatistic)
    delete mStatistic;
}

bool sctpServer::start(const QString & config)
{
  parseConfig(config);

  if (!listen(QHostAddress::Any, mPort))
  {
    qCritical() << QObject::tr("Unable to start the server: %1").arg(errorString());
    return false;
  }

  QString ipAddress;
  QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
  // use the first non-localhost IPv4 address
  for (int i = 0; i < ipAddressesList.size(); ++i)
  {
    if (ipAddressesList.at(i) != QHostAddress::LocalHost && ipAddressesList.at(i).toIPv4Address())
    {
      ipAddress = ipAddressesList.at(i).toString();
      break;
    }
  }

  // if we did not find one, use IPv4 localhost
  if (ipAddress.isEmpty())
    ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
  QString message = QObject::tr("The server is running on\nIP: %1\tport: %2\n").arg(ipAddress).arg(serverPort());
  qDebug() << message.toUtf8().constData();

  // initialize sc-memory
  qDebug() << "Initialize sc-memory\n";
  sc_memory_params params;
  sc_memory_params_clear(&params);

  std::string config_path = config.toStdString();
  std::string repo_path = mRepoPath.toStdString();
  std::string ext_path = mExtPath.toStdString();

  params.clear = SC_FALSE;
  params.config_file = config_path.c_str();
  params.repo_path = repo_path.c_str();
  params.ext_path = ext_path.c_str();

  ScMemory::Initialize(params);

  mContext.reset(new ScMemoryContext(sc_access_lvl_make_max, "SctpServer"));
  if (mContext.get() == nullptr)
    return false;

  mEventManager = new sctpEventManager();
  mEventManager->initialize();

  if (mStatUpdatePeriod > 0)
  {
    mStatistic = new sctpStatistic(this);
    mStatistic->initialize(mStatPath, mStatUpdatePeriod, mContext->GetRealContext());
  }

  QTimer::singleShot(mSavePeriod * 1000, this, SLOT(onSave()));

  return true;
}

void sctpServer::parseConfig(const QString & config_path)
{
  std::map<std::string, std::string> conf_file = parse_config(config_path.toStdString());

  bool result = false;
  qDebug() << QString::fromStdString(conf_file["port"]);
  mPort = QString::fromStdString(conf_file["port"]).toUInt(&result);

  if (!result)
  {
    qDebug() << "Can't parse port number from configuration file\n";
    exit(0);
  }

  mRepoPath = QString::fromStdString(conf_file["path"]);
  if (mRepoPath.isEmpty())
  {
    qDebug() << "Path to repo is empty\n";
    exit(0);
  }

  mSavePeriod = QString::fromStdString(conf_file["save_period"]).toUInt(&result);
  if (!result)
  {
    qWarning() << "Can't parse save period. Use default value: 3600 (1h)";
    mSavePeriod = 3600;
  }

  mExtPath = QString::fromStdString(conf_file["ext"]);

  mStatUpdatePeriod = QString::fromStdString(conf_file["update_period"]).toUInt(&result);
  if (!result)
    qWarning() << "Can't parse period statistic from configuration file\n";
  if (mStatUpdatePeriod > 0 && mStatUpdatePeriod < 60)
    qWarning() << "Statistics update period is very short, it would be take much processor time. Recomend to make it "
                  "more long";

  mStatPath = QString::fromStdString(conf_file["stat_path"]);
  if (mStatPath.isEmpty() && mStatUpdatePeriod > 0)
  {
    qDebug() << "Path to store statistics is empty\n";
    exit(0);
  }
}

void sctpServer::incomingConnection(qintptr socketDescriptor)
{
  sctpClient * client = new sctpClient(this, socketDescriptor);
  connect(client, SIGNAL(finished()), client, SLOT(deleteLater()));
  connect(client, SIGNAL(destroyed(QObject *)), this, SLOT(clientDestroyed(QObject *)));
  mClients.insert(client);
  client->start();
}

void sctpServer::stop()
{
  mContext.reset();

  mEventManager->shutdown();
  delete mEventManager;
  mEventManager = 0;

  ScMemory::Shutdown(true);

  exit(0);
}

void sctpServer::clientDestroyed(QObject * client)
{
  QSet<sctpClient *>::iterator it = mClients.find(static_cast<sctpClient *>(client));
  if (it == mClients.end())
    qWarning("Recieve event from non existing client");

  mClients.erase(it);
}

void sctpServer::onSave()
{
  sc_memory_save(mContext->GetRealContext());

  QTimer::singleShot(mSavePeriod * 1000, this, SLOT(onSave()));
}
