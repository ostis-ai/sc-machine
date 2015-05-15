/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sctpClient_h_
#define _sctpClient_h_

#include <QObject>
#include <QThread>


class QTcpSocket;
class sctpCommand;

class sctpClient : public QThread
{
    Q_OBJECT
public:
    explicit sctpClient(QObject *parent, int socketDescriptor);
    virtual ~sctpClient();

    void run();

protected:
    void processCommands();

private:
    //! Pointer to client socket
    QTcpSocket *mSocket;
    //! Pointer to command processing class
    sctpCommand *mCommand;

    int mSocketDescriptor;
    
signals:
    void done(sctpClient *client);
};

#endif // CLIENTTHREAD_H
