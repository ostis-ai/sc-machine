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

extern "C"
{
#include "sc_memory.h"
#include "sc_helper.h"
}

#include <QNetworkInterface>

sctpServer::sctpServer(QObject *parent)
  : QTcpServer(parent)
  , mPort(0)
  , mStatistic(0)
  , mSavePeriod(0)
  , mEventManager(0)
  , mContext(0)
{
}

sctpServer::~sctpServer()
{
    if (mStatistic)
        delete mStatistic;

}

bool sctpServer::start(const QString &config)
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
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
                ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }

    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    QString message = QObject::tr("The server is running on\nIP: %1\tport: %2\n")
                                  .arg(ipAddress).arg(serverPort());
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

    mContext = sc_memory_initialize(&params);
    if (mContext == 0)
        return false;

    mEventManager = new sctpEventManager();
    mEventManager->initialize();

    if (mStatUpdatePeriod > 0)
    {
        mStatistic = new sctpStatistic(this);
        mStatistic->initialize(mStatPath, mStatUpdatePeriod, mContext);
    }

    QTimer::singleShot(mSavePeriod * 1000, this, SLOT(onSave()));

    return true;
}

void sctpServer::parseConfig(const QString &config_path)
{
    QSettings settings(config_path, QSettings::IniFormat);

    bool result = false;
    qDebug() << settings.value("Network/Port").toString();
    mPort = settings.value("Network/Port").toUInt(&result);

    if (!result)
    {
        qDebug() << "Can't parse port number from configuration file\n";
        exit(0);
    }

    mRepoPath = settings.value("Repo/Path").toString();
    if (mRepoPath.isEmpty())
    {
        qDebug() << "Path to repo is empty\n";
        exit(0);
    }

    mSavePeriod = settings.value("Repo/SavePeriod").toUInt(&result);
    if (!result)
    {
        qWarning() << "Can't parse save period. Use default value: 3600 (1h)";
        mSavePeriod = 3600;
    }

    mExtPath = settings.value("Extensions/Directory").toString();

    mStatUpdatePeriod = settings.value("Stat/UpdatePeriod").toUInt(&result);
    if (!result)
        qWarning() << "Can't parse period statistic from configuration file\n";
    if (mStatUpdatePeriod > 0 && mStatUpdatePeriod < 60)
        qWarning() << "Statistics update period is very short, it would be take much processor time. Recomend to make it more long";

    mStatPath = settings.value("Stat/Path").toString();
    if (mStatPath.isEmpty() && mStatUpdatePeriod > 0)
    {
        qDebug() << "Path to store statistics is empty\n";
        exit(0);
    }
}

void sctpServer::incomingConnection(qintptr socketDescriptor)
{
    sctpClient *client = new sctpClient(this, socketDescriptor);
    connect(client, SIGNAL(finished()), client, SLOT(deleteLater()));
    connect(client, SIGNAL(destroyed(QObject*)), this, SLOT(clientDestroyed(QObject*)));
    mClients.insert(client);
    client->start();
}

void sctpServer::stop()
{
    sc_memory_shutdown(SC_TRUE);
    mContext = 0;

    mEventManager->shutdown();
    delete mEventManager;
    mEventManager = 0;

    close();
}

void sctpServer::clientDestroyed(QObject *client)
{
    QSet<sctpClient*>::iterator it = mClients.find(static_cast<sctpClient*>(client));
    if (it == mClients.end())
        qWarning("Recieve event from non existing client");

    mClients.erase(it);
}

void sctpServer::onSave()
{
    sc_memory_save(mContext);

    QTimer::singleShot(mSavePeriod * 1000, this, SLOT(onSave()));
}
