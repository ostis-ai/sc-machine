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

#include <QtCore/QCoreApplication>
#include <QNetworkInterface>
#include "sctpserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    sctpServer server;
    if (!server.listen(QHostAddress::Any, 55770))
    {
        qCritical(QObject::tr("Unable to start the server: %1").arg(server.errorString()).toUtf8().constData());
        return 0;
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
    QString message = QObject::tr("The server is running on\nIP: %1\tport: %2\n"
                                  "Run the Fortune Client example now.")
                                  .arg(ipAddress).arg(server.serverPort());
    printf("%s", message.toUtf8().constData());
    
    return a.exec();
}
