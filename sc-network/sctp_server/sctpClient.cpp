/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sctpClient.h"
#include "sctpCommand.h"
#include "sctpStatistic.h"

#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>

sctpClient::sctpClient(QObject *parent, int socketDescriptor)
    : QThread(parent)
    , mSocket(0)
    , mCommand(0)
    , mSocketDescriptor(socketDescriptor)
{
}

sctpClient::~sctpClient()
{
}

void sctpClient::run()
{
    mSocket = new QTcpSocket();
    if (!mSocket->setSocketDescriptor(mSocketDescriptor))
    {
        qDebug() << "Can't process socket descriptor " << mSocketDescriptor;
        delete mSocket;
        return;
    }

    qDebug() << "Client connected: " << mSocket->peerAddress();

    sctpStatistic::getInstance()->clientConnected();

    mCommand = new sctpCommand();
    mCommand->init();

    while (mSocket->waitForReadyRead())
    {
        processCommands();
    }

    mCommand->shutdown();

    if (mSocket->state() == QTcpSocket::ConnectedState)
    {
        mSocket->waitForBytesWritten();
        mSocket->waitForDisconnected();
    }
    mSocket->close();

    delete mSocket;
    mSocket = 0;

    delete mCommand;
    mCommand = 0;

    //deleteLater(); // shedule destroy in main thread
}

void sctpClient::processCommands()
{
    while (mSocket->bytesAvailable() >= mCommand->cmdHeaderSize())
    {
        eSctpErrorCode errCode = mCommand->processCommand(mSocket, mSocket);
        if (errCode != SCTP_NO_ERROR)
        {
            qDebug() << "Error: " << errCode << "; while process request from client " << mSocket->peerAddress().toString();
            sctpStatistic::getInstance()->commandProcessed(true);
        }else
        {
            sctpStatistic::getInstance()->commandProcessed(false);
        }

        mSocket->flush();
    }
}
