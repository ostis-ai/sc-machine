/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2013 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

#include "sctpServer.h"
#include "sctpClient.h"
#include "sctpStatistic.h"

#include <QSettings>
#include <QDebug>

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
    printf("%s", message.toUtf8().constData());

    // initialize sc-memory
    qDebug() << "Initialize sc-memory\n";
    if (sc_memory_initialize(mRepoPath.toStdString().c_str(), config.toStdString().c_str()) != SC_TRUE)
        return false;
    if (sc_helper_init() != SC_RESULT_OK)
        return false;
    if (sc_memory_initialize_ext(mExtPath.toStdString().c_str()) != SC_TRUE)
        return false;

    if (mStatUpdatePeriod > 0)
    {
        mStatistic = new sctpStatistic(this);
        mStatistic->initialize(mStatPath, mStatUpdatePeriod);
    }

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

void sctpServer::incomingConnection(int socketDescriptor)
{
    // store client in clients list
    sctpClient *client = new sctpClient(this);
    client->setSocketDescriptor(socketDescriptor);
}

void sctpServer::stop()
{
    sc_memory_shutdown_ext();
    sc_helper_shutdown();
    sc_memory_shutdown();
    close();
}

