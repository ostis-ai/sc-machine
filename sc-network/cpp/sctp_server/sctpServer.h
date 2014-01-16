/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2014 OSTIS

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

#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QList>

class sctpClient;
class sctpStatistic;

class QThreadPool;

class sctpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit sctpServer(QObject *parent = 0);
    virtual ~sctpServer();

    //! Starts server
    bool start(const QString &config);

protected:
    //! Parse configuration file
    void parseConfig(const QString &config_path);


protected:
    void incomingConnection(int socketDescriptor);

private:
    //! Port number
    quint16 mPort;
    //! Path to repository
    QString mRepoPath;
    //! Path to extensions directory
    QString mExtPath;

    QString mStatPath;
    quint32 mStatUpdatePeriod;
    sctpStatistic *mStatistic;

    //! Worker threads pool
    QThreadPool *mThreadPool;


signals:


public slots:
    void stop();

};

#endif // SERVER_H
