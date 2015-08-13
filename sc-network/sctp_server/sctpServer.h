/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sctpServer_h_
#define _sctpServer_h_

#include "../sctp_client/sctpTypes.hpp"

#include <QTcpServer>
#include <QList>

class sctpClient;
class sctpStatistic;
class sctpEventManager;

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
    void incomingConnection(qintptr socketDescriptor);

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

    quint32 mSavePeriod;

    QSet<sctpClient*> mClients;

    //! Event manager instance
    sctpEventManager *mEventManager;
    //! Pointer to default memory context
    sc_memory_context *mContext;

signals:


public slots:
    void stop();

    void onSave();

    void clientDestroyed(QObject *client);

};

#endif // SERVER_H
