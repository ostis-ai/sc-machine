/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <QtCore/QCoreApplication>
#include "sctpServer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString config("config.ini");
    if (argc > 1)
        config = argv[1];

    sctpServer server;
    if (!server.start(config))
        exit(0);

    QObject::connect(&a, SIGNAL(aboutToQuit()), &server, SLOT(stop()));
    
    return a.exec();
}
