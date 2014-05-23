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

#include "sctpClient.h"
#include "sctpCommand.h"
#include "sctpStatistic.h"

#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>

sctpClient::sctpClient(int socketDescriptor)
    : mSocket(0)
    , mSocketDescriptor(socketDescriptor)
    , mCommand(0)
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
