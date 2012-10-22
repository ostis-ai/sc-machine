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

#include "sctpCommand.h"

#include <QIODevice>
#include <QDataStream>
#include <QDebug>

#define SCTP_READ_TIMEOUT   10000

sctpCommand::sctpCommand(QObject *parent) :
    QObject(parent)
{
}

sctpCommand::~sctpCommand()
{
}

sctpCommand::eResult sctpCommand::processCommand(QIODevice *inDevice, QIODevice *outDevice)
{
    quint8 cmdCode = SCTP_CMD_UNKNOWN;
    quint8 cmdFlags = 0;
    quint32 cmdId = 0;
    quint32 cmdParamSize = 0;

    // read command header
    if (!waitAvailableBytes(inDevice, cmdHeaderSize()))
        return SCTP_RESULT_CMD_HEADER_READ_TIMEOUT;

    inDevice->read((char*)&cmdCode, sizeof(cmdCode));
    inDevice->read((char*)&cmdFlags, sizeof(cmdFlags));
    inDevice->read((char*)&cmdId, sizeof(cmdId));
    inDevice->read((char*)&cmdParamSize, sizeof(cmdParamSize));

    // read params data
    QByteArray paramsData(cmdParamSize, 0);
    if (!waitAvailableBytes(inDevice, cmdParamSize))
        return SCTP_RESULT_CMD_PARAM_READ_TIMEOUT;

    inDevice->read((char*)paramsData.data(), paramsData.size());
    QDataStream paramsStream(paramsData);

    qDebug() << "command code: " << cmdCode << "\n";

    switch (cmdCode)
    {
    case SCTP_CMD_CHECK_ELEMENT:
        if (processCheckElement(cmdFlags, cmdId, &paramsStream, outDevice))
            return SCTP_RESULT_OK;
        break;

    default:
        return SCTP_RESULT_UNKNOWN_CMD;
    }

    return SCTP_RESULT_ERROR;
}

bool sctpCommand::waitAvailableBytes(QIODevice *stream, quint32 bytesNum)
{
    while (stream->bytesAvailable() < bytesNum)
    {
        if (!stream->waitForReadyRead(SCTP_READ_TIMEOUT))
        {
            return false;
        }
    }

    return true;
}

quint32 sctpCommand::cmdHeaderSize()
{
    return 2 * sizeof(quint8) + 2 * sizeof(quint32);
}


// ----------- process commands -------------
bool sctpCommand::processCheckElement(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    qDebug() << "process check";
    return false;
}
