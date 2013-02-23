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

#include "sctpClient.h"
#include "sctpCommand.h"

#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>

sctpClient::sctpClient(QObject *parent)
    : QObject(parent)
    , mSocket(0)
{
    mCommand = new sctpCommand(this);
}

sctpClient::~sctpClient()
{
    delete mCommand;
}

void sctpClient::setSocketDescriptor(int socketDescriptor)
{
    mSocket = new QTcpSocket(this);

    connect(mSocket, SIGNAL(connected()), this, SLOT(connected()));
    connect(mSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(mSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));

    mSocket->setSocketDescriptor(socketDescriptor);

    qDebug() << "Connected client from address: " << mSocket->peerAddress().toString();
}

void sctpClient::connected()
{
}

void sctpClient::disconnected()
{
    qDebug() << "Disconnected client with adress: " << mSocket->peerAddress().toString();
}

void sctpClient::readyRead()
{
    while (mSocket->bytesAvailable() >= mCommand->cmdHeaderSize())
    {
        sctpErrorCode errCode = mCommand->processCommand(mSocket, mSocket);
        if (errCode != SCTP_ERROR_NO)
        {
            qDebug() << "Error: " << errCode << "; while process request from clien " << mSocket->peerAddress().toString();
//            mSocket->close();
        }

        mSocket->flush();
    }
}
