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
#include <QCoreApplication>

extern "C"
{
#include "sc_memory.h"
}

#define SCTP_READ_TIMEOUT   10000

sctpCommand::sctpCommand(QObject *parent) :
    QObject(parent)
{
}

sctpCommand::~sctpCommand()
{
}

sctpErrorCode sctpCommand::processCommand(QIODevice *inDevice, QIODevice *outDevice)
{
    quint8 cmdCode = SCTP_CMD_UNKNOWN;
    quint8 cmdFlags = 0;
    quint32 cmdId = 0;
    quint32 cmdParamSize = 0;

    // read command header
    if (!waitAvailableBytes(inDevice, cmdHeaderSize()))
        return SCTP_ERROR_CMD_HEADER_READ_TIMEOUT;

    inDevice->read((char*)&cmdCode, sizeof(cmdCode));
    inDevice->read((char*)&cmdFlags, sizeof(cmdFlags));
    inDevice->read((char*)&cmdId, sizeof(cmdId));
    inDevice->read((char*)&cmdParamSize, sizeof(cmdParamSize));

    qDebug() << "command code: " << cmdCode << "; params size: " << cmdParamSize << "\n";

    // read params data
    QByteArray paramsData(cmdParamSize, 0);
    if (!waitAvailableBytes(inDevice, cmdParamSize))
        return SCTP_ERROR_CMD_PARAM_READ_TIMEOUT;

    inDevice->read((char*)paramsData.data(), paramsData.size());
    QDataStream paramsStream(paramsData);


    switch (cmdCode)
    {
    case SCTP_CMD_CHECK_ELEMENT:
        return processCheckElement(cmdFlags, cmdId, &paramsStream, outDevice);

    case SCTP_CMD_GET_ELEMENT_TYPE:
        return processGetElementType(cmdFlags, cmdId, &paramsStream, outDevice);

    case SCTP_CMD_ERASE_ELEMENT:
        return processElementErase(cmdFlags, cmdId, &paramsStream, outDevice);

    case SCTP_CMD_SHUTDOWN:
        QCoreApplication::quit();
        break;

    default:
        return SCTP_ERROR_UNKNOWN_CMD;
    }

    return SCTP_ERROR;
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

void sctpCommand::writeResultHeader(sctpCommandCode cmdCode, quint32 cmdId, sctpResultCode resCode, quint32 resSize, QIODevice *outDevice)
{
    Q_ASSERT(outDevice != 0);
    quint8 code = cmdCode;

    outDevice->write((const char*)&code, sizeof(code));
    outDevice->write((const char*)&cmdId, sizeof(cmdId));

    code = resCode;
    outDevice->write((const char*)&code, sizeof(code));
    outDevice->write((const char*)&resSize, sizeof(resSize));
}

quint32 sctpCommand::cmdHeaderSize()
{
    return 2 * sizeof(quint8) + 2 * sizeof(quint32);
}


// ----------- process commands -------------
sctpErrorCode sctpCommand::processCheckElement(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    sc_addr addr;
    Q_UNUSED(cmdFlags);

    Q_ASSERT(params != 0);

    // read sc-add from parameters
    if (params->readRawData((char*)&addr, sizeof(addr)) != sizeof (addr))
        return SCTP_ERROR_CMD_READ_PARAMS;

    sctpResultCode resCode = sc_memory_is_element(addr) ? SCTP_RESULT_TRUE : SCTP_RESULT_FALSE;

    // send result
    writeResultHeader(SCTP_CMD_CHECK_ELEMENT, cmdId, resCode, 0, outDevice);

    return SCTP_ERROR_NO;
}

sctpErrorCode sctpCommand::processGetElementType(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    sc_addr addr;
    Q_UNUSED(cmdFlags);

    Q_ASSERT(params != 0);

    // read sc-addr of sc-element from parameters
    if (params->readRawData((char*)&addr, sizeof(addr)) != sizeof(addr))
        return SCTP_ERROR_CMD_READ_PARAMS;

    sc_type type = 0;
    sctpResultCode resCode = (sc_memory_get_element_type(addr, &type) == SC_OK) ? SCTP_RESULT_TRUE : SCTP_RESULT_FALSE;
    quint32 resSize = (resCode == SCTP_RESULT_TRUE) ? sizeof(type) : 0;

    // send result
    writeResultHeader(SCTP_CMD_GET_ELEMENT_TYPE, cmdId, resCode, resSize, outDevice);
    if (resCode == SCTP_RESULT_TRUE)
        outDevice->write((const char*)&type, sizeof(type));

    return SCTP_ERROR_NO;
}

sctpErrorCode sctpCommand::processElementErase(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    sc_addr addr;
    Q_UNUSED(cmdFlags);

    Q_ASSERT(params != 0);

    // read sc-addr of sc-element from parameters
    if (params->readRawData((char*)&addr, sizeof(addr)) != sizeof(addr))
        return SCTP_ERROR_CMD_READ_PARAMS;

    sctpResultCode resCode = (sc_memory_element_free(addr) == SC_OK) ? SCTP_RESULT_TRUE : SCTP_RESULT_FALSE;
    // send result
    writeResultHeader(SCTP_CMD_CHECK_ELEMENT, cmdId, resCode, 0, outDevice);

    return SCTP_ERROR_NO;
}
