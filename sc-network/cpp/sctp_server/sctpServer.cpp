/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010 OSTIS

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

#include <QSettings>
#include <QDebug>
#include <QTimer>
#include <QDir>

extern "C"
{
#include "sc_memory.h"
#include "sc_helper.h"
}

#include <QNetworkInterface>

sctpServer::sctpServer(QObject *parent)
  : QTcpServer(parent)
  , mPort(0)
  , mStatUpdatePeriod(0) // one hour
  , mStatUpdateTimer(0)
  , mStatInitUpdate(false)
{
}

sctpServer::~sctpServer()
{
}

bool sctpServer::start(const QString &config)
{
    parseConfig(config);

    if (!listen(QHostAddress::Any, mPort))
    {
        qCritical() << QObject::tr("Unable to start the server: %1").arg(errorString());
        return false;
    }

    // create statistics directory
    if (mStatUpdatePeriod > 0)
    {
        // create directory, that would contain statistics
        QDir dir(mStatPath);
        if (!dir.isAbsolute())
        {
            qCritical() << "Path to statistics directory must to be an absolute\n";
            return false;
        }

        if (!dir.exists())
        {
            if (!dir.mkpath(mStatPath))
            {
                qCritical() << QString("Can't create statistics path: '%1'").arg(mStatPath);
                return false;
            }
        }

        mStatUpdateTimer = new QTimer(this);
        statUpdate();
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

    mClientsList.append(client);
}

void sctpServer::stop()
{
    sc_memory_shutdown_ext();
    sc_helper_shutdown();
    sc_memory_shutdown();
    close();
}

void sctpServer::statUpdate()
{
    if (!mStatInitUpdate)
    {
        //! @todo write startup statistics
    }

    mStatUpdateTimer->singleShot(mStatUpdatePeriod * 1000, this, SLOT(statUpdate()));
}
