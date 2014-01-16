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

#include "sctpCommand.h"
#include "sctpStatistic.h"

#include <QIODevice>
#include <QDataStream>
#include <QDebug>
#include <QBuffer>
#include <QCoreApplication>

extern "C"
{
#include "sc_memory_headers.h"
#include "sc_helper.h"
}

#define SCTP_READ_TIMEOUT   10000

#define READ_PARAM(val)  if (params->readRawData((char*)&val, sizeof(val)) != sizeof(val)) \
                            return SCTP_ERROR_CMD_READ_PARAMS;

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

    //qDebug() << "command code: " << cmdCode << "; params size: " << cmdParamSize << "\n";

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

    case SCTP_CMD_CREATE_NODE:
        return processCreateNode(cmdFlags, cmdId, &paramsStream, outDevice);

    case SCTP_CMD_CREAET_LINK:
        return processCreateLink(cmdFlags, cmdId, &paramsStream, outDevice);

    case SCTP_CMD_CREATE_ARC:
        return processCreateArc(cmdFlags, cmdId, &paramsStream, outDevice);

    case SCTP_CMD_GET_LINK_CONTENT:
        return processGetLinkContent(cmdFlags, cmdId, &paramsStream, outDevice);

    case SCTP_CMD_FIND_LINKS:
        return processFindLinks(cmdFlags, cmdId, &paramsStream, outDevice);

    case SCTP_CMD_SET_LINK_CONTENT:
        return processSetLinkContent(cmdFlags, cmdId, &paramsStream, outDevice);

    case SCTP_CMD_ITERATE_ELEMENTS:
        return processIterateElements(cmdFlags, cmdId, &paramsStream, outDevice);


    case SCTP_CMD_FIND_ELEMENT_BY_SYSITDF:
        return processFindElementBySysIdtf(cmdFlags, cmdId, &paramsStream, outDevice);

    case SCTP_CMD_SET_SYSIDTF:
        return processSetSysIdtf(cmdFlags, cmdId, &paramsStream, outDevice);

    case SCTP_CMD_STATISTICS:
        return processStatistics(cmdFlags, cmdId, &paramsStream, outDevice);

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
    READ_PARAM(addr);

    sctpResultCode resCode = sc_memory_is_element(addr) ? SCTP_RESULT_OK : SCTP_RESULT_FAIL;

    // send result
    writeResultHeader(SCTP_CMD_CHECK_ELEMENT, cmdId, resCode, 0, outDevice);

    return SCTP_NO_ERROR;
}

sctpErrorCode sctpCommand::processGetElementType(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    sc_addr addr;
    Q_UNUSED(cmdFlags);

    Q_ASSERT(params != 0);

    // read sc-addr of sc-element from parameters
    READ_PARAM(addr);

    sc_type type = 0;
    sctpResultCode resCode = (sc_memory_get_element_type(addr, &type) == SC_RESULT_OK) ? SCTP_RESULT_OK : SCTP_RESULT_FAIL;
    quint32 resSize = (resCode == SCTP_RESULT_OK) ? sizeof(type) : 0;

    // send result
    writeResultHeader(SCTP_CMD_GET_ELEMENT_TYPE, cmdId, resCode, resSize, outDevice);
    if (resCode == SCTP_RESULT_OK)
        outDevice->write((const char*)&type, sizeof(type));

    return SCTP_NO_ERROR;
}

sctpErrorCode sctpCommand::processElementErase(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    sc_addr addr;
    Q_UNUSED(cmdFlags);

    Q_ASSERT(params != 0);

    // read sc-addr of sc-element from parameters
    READ_PARAM(addr);

    sctpResultCode resCode = (sc_memory_element_free(addr) == SC_RESULT_OK) ? SCTP_RESULT_OK : SCTP_RESULT_FAIL;
    // send result
    writeResultHeader(SCTP_CMD_CHECK_ELEMENT, cmdId, resCode, 0, outDevice);

    return SCTP_NO_ERROR;
}

sctpErrorCode sctpCommand::processCreateNode(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    Q_UNUSED(cmdFlags);

    Q_ASSERT(params != 0);

    sc_type type;
    // read type of node
    READ_PARAM(type);

    sc_addr addr = sc_memory_node_new(type);

    // send result
    sctpErrorCode result;
    if (SC_ADDR_IS_NOT_EMPTY(addr))
    {
        writeResultHeader(SCTP_CMD_CREATE_NODE, cmdId, SCTP_RESULT_OK, sizeof(addr), outDevice);
        outDevice->write((const char*)&addr, sizeof(addr));

        result = SCTP_NO_ERROR;
    }else
    {
        writeResultHeader(SCTP_CMD_CREATE_NODE, cmdId, SCTP_RESULT_FAIL, 0, outDevice);
        result = SCTP_ERROR;
    }

    return result;
}

sctpErrorCode sctpCommand::processCreateLink(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    Q_UNUSED(cmdFlags);

    sc_addr addr = sc_memory_link_new();

    // send result
    sctpErrorCode result;
    if (SC_ADDR_IS_NOT_EMPTY(addr))
    {
        writeResultHeader(SCTP_CMD_CREATE_NODE, cmdId, SCTP_RESULT_OK, sizeof(addr), outDevice);
        outDevice->write((const char*)&addr, sizeof(addr));

        result = SCTP_NO_ERROR;
    }else
    {
        writeResultHeader(SCTP_CMD_CREATE_NODE, cmdId, SCTP_RESULT_FAIL, 0, outDevice);
        result = SCTP_ERROR;
    }

    return result;
}

sctpErrorCode sctpCommand::processCreateArc(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    Q_UNUSED(cmdFlags);

    Q_ASSERT(params != 0);

    sc_type type;
    sc_addr begin_addr, end_addr;

    // read type of sc-arc
    READ_PARAM(type);

    // read sc-addr of begin and end elements
    READ_PARAM(begin_addr);
    READ_PARAM(end_addr);

    sc_addr addr = sc_memory_arc_new(type, begin_addr, end_addr);
    sctpErrorCode result;
    if (SC_ADDR_IS_NOT_EMPTY(addr))
    {
        writeResultHeader(SCTP_CMD_CREATE_ARC, cmdId, SCTP_RESULT_OK, 0, outDevice);
        outDevice->write((const char*)&addr, sizeof(addr));

        result = SCTP_NO_ERROR;
    }else
    {
        writeResultHeader(SCTP_CMD_CREAET_LINK, cmdId, SCTP_RESULT_FAIL, 0, outDevice);
        result = SCTP_ERROR;
    }

    return result;
}


sctpErrorCode sctpCommand::processGetLinkContent(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    sc_addr addr;
    sc_stream *stream = (sc_stream*)nullptr;
    sc_char data_buffer[512];
    sc_uint32 data_len = 0;
    sc_uint32 data_written = 0;
    sc_uint32 data_read = 0;

    Q_UNUSED(cmdFlags);
    Q_ASSERT(params != 0);

    // read sc-addr of sc-element from parameters
    if (params->readRawData((char*)&addr, sizeof(addr)) != sizeof(addr))
        return SCTP_ERROR_CMD_READ_PARAMS;

    sctpResultCode resCode = (sc_memory_get_link_content(addr, &stream) == SC_RESULT_OK) ? SCTP_RESULT_OK : SCTP_RESULT_FAIL;


    if (resCode == SCTP_RESULT_OK)
    {
        if (sc_stream_get_length(stream, &data_len) != SC_RESULT_OK)
        {
            resCode = SCTP_RESULT_FAIL;
            sc_stream_free(stream);
            stream = (sc_stream*)nullptr;
        }
    }
    // send result
    writeResultHeader(SCTP_CMD_GET_LINK_CONTENT, cmdId, resCode, data_len, outDevice);

    if (resCode == SCTP_RESULT_FAIL)
    {
        if (stream != nullptr)
            sc_stream_free(stream);

        return SCTP_ERROR;
    }

    // write content data
    while (sc_stream_eof(stream) != SC_TRUE)
    {
        // if there are any error to read data, then
        // write null into output
        if (sc_stream_read_data(stream, data_buffer, 512, &data_read) != SC_RESULT_OK)
        {
            if (data_written < data_len)
            {
                quint32 len = data_len - data_written;
                sc_char *data = new sc_char[len];
                memset(data, 0, len);
                outDevice->write(data, len);
                delete []data;

                sc_stream_free(stream);
                return SCTP_ERROR;
            }
        }

        outDevice->write(data_buffer, data_read);
        data_written += data_read;
    }

    Q_ASSERT(data_written == data_len);

    if (resCode == SCTP_RESULT_OK)
        sc_stream_free(stream);


    return SCTP_NO_ERROR;
}

sctpErrorCode sctpCommand::processFindLinks(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    sc_addr addr;
    sc_uint32 data_len = 0;
    sc_char *data = 0;

    Q_UNUSED(cmdFlags);

    Q_ASSERT(params != 0);

    // read length of content data
    READ_PARAM(data_len);
    Q_ASSERT(data_len > 0);

    data = new sc_char[data_len];
    if (params->readRawData(data, data_len) != data_len)
    {
        delete[] data;
        return SCTP_ERROR_CMD_READ_PARAMS;
    }

    sc_stream *stream = sc_stream_memory_new(data, data_len, SC_STREAM_READ, SC_FALSE);
    sc_uint32 result_count = 0;
    sc_addr *result = 0;

    if (sc_memory_find_links_with_content(stream, &result, &result_count) != SC_RESULT_OK)
        writeResultHeader(SCTP_CMD_FIND_LINKS, cmdId, SCTP_RESULT_FAIL, 0, outDevice);
    else
    {
        writeResultHeader(SCTP_CMD_FIND_LINKS, cmdId, SCTP_RESULT_OK, result_count * sizeof(sc_addr) + sizeof(result_count), outDevice);
        outDevice->write((const char*)&result_count, sizeof(result_count));
        outDevice->write((const char*)result, sizeof(sc_addr) * result_count);
    }
    delete[] data;
    sc_stream_free(stream);

    return SCTP_NO_ERROR;
}

sctpErrorCode sctpCommand::processSetLinkContent(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    sc_addr addr;
    sc_uint32 data_len = 0;
    sc_char *data = 0;

    Q_UNUSED(cmdFlags);

    Q_ASSERT(params != 0);

    // read length of content data
    READ_PARAM(addr);
    READ_PARAM(data_len);
    Q_ASSERT(data_len > 0);

    data = new sc_char[data_len];
    if (params->readRawData(data, data_len) != data_len)
    {
        delete[] data;
        return SCTP_ERROR_CMD_READ_PARAMS;
    }

    sc_stream *stream = sc_stream_memory_new(data, data_len, SC_STREAM_READ, SC_FALSE);

    sc_result result = sc_memory_set_link_content(addr, stream);
    writeResultHeader(SCTP_CMD_SET_LINK_CONTENT, cmdId, result == SC_RESULT_OK ? SCTP_RESULT_OK : SCTP_RESULT_FAIL, 0, outDevice);
    sc_stream_free(stream);
    delete[] data;

    return SCTP_NO_ERROR;
}

sctpErrorCode sctpCommand::processIterateElements(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    sc_uchar iterator_type = 0;
    sc_type type1, type2, type3, type4;
    sc_addr addr1, addr2, addr3;

    Q_UNUSED(cmdFlags);

    Q_ASSERT(params != nullptr);

    // read iterator type
    READ_PARAM(iterator_type);

    Q_ASSERT(iterator_type < SCTP_ITERATOR_COUNT);

    // 3-elements iterators
    if (iterator_type <= SCTP_ITERATOR_3F_A_F)
    {
        sc_iterator3 *it = (sc_iterator3*)nullptr;

        switch (iterator_type)
        {
        case SCTP_ITERATOR_3A_A_F:
            READ_PARAM(type1);
            READ_PARAM(type2);
            READ_PARAM(addr1);
            it = sc_iterator3_a_a_f_new(type1, type2, addr1);
            break;

        case SCTP_ITERATOR_3F_A_A:
            READ_PARAM(addr1);
            READ_PARAM(type1);
            READ_PARAM(type2);
            it = sc_iterator3_f_a_a_new(addr1, type1, type2);
            break;

        case SCTP_ITERATOR_3F_A_F:
            READ_PARAM(addr1);
            READ_PARAM(type1);
            READ_PARAM(addr2);
            it = sc_iterator3_f_a_f_new(addr1, type1, addr2);
            break;

        default:
            return SCTP_ERROR;
        }

        Q_ASSERT(it != nullptr);

        // create results data
        QByteArray results;
        QBuffer buffer(&results);
        sc_uint32 results_count = 0;
        sc_addr addr;

        buffer.open(QBuffer::WriteOnly);
        while (sc_iterator3_next(it) == SC_TRUE)
        {
            results_count++;
            for (sc_uint i = 0; i < 3; i++)
            {
                addr = sc_iterator3_value(it, i);
                buffer.write((const char*)&addr, sizeof(addr));
            }
        }
        buffer.close();

        // write result
        writeResultHeader(SCTP_CMD_ITERATE_ELEMENTS, cmdId, SCTP_RESULT_OK, results.size() + sizeof(results_count), outDevice);
        outDevice->write((const char*)&results_count, sizeof(results_count));
        if (results_count > 0)
            outDevice->write((const char*)results.constData(), results.size());

        sc_iterator3_free(it);

    }else
    {
        // 5-elements iterators
        sc_iterator5 *it = (sc_iterator5*)nullptr;

        switch (iterator_type)
        {
        case SCTP_ITERATOR_5F_A_A_A_F:
            READ_PARAM(addr1);
            READ_PARAM(type1);
            READ_PARAM(type2);
            READ_PARAM(type3);
            READ_PARAM(addr2);
            it = sc_iterator5_f_a_a_a_f_new(addr1, type1, type2, type3, addr2);
            break;

        case SCTP_ITERATOR_5_A_A_F_A_A:
            READ_PARAM(type1);
            READ_PARAM(type2);
            READ_PARAM(addr1);
            READ_PARAM(type3);
            READ_PARAM(type4);
            it = sc_iterator5_a_a_f_a_a_new(type1, type2, addr1, type3, type4);
            break;

        case SCTP_ITERATOR_5_A_A_F_A_F:
            READ_PARAM(type1);
            READ_PARAM(type2);
            READ_PARAM(addr1);
            READ_PARAM(type3);
            READ_PARAM(addr2);
            it = sc_iterator5_a_a_f_a_f_new(type1, type2, addr1, type3, addr2);
            break;

        case SCTP_ITERATOR_5_F_A_A_A_A:
            READ_PARAM(addr1);
            READ_PARAM(type1);
            READ_PARAM(type2);
            READ_PARAM(type3);
            READ_PARAM(type4);
            it = sc_iterator5_f_a_a_a_a_new(addr1, type1, type2, type3, type4);
            break;

        case SCTP_ITERATOR_5_F_A_F_A_A:
            READ_PARAM(addr1);
            READ_PARAM(type1);
            READ_PARAM(addr2);
            READ_PARAM(type2);
            READ_PARAM(type3);
            it = sc_iterator5_f_a_f_a_a_new(addr1, type1, addr2, type2, type3);
            break;

        case SCTP_ITERATOR_5_F_A_F_A_F:
            READ_PARAM(addr1);
            READ_PARAM(type1);
            READ_PARAM(addr2);
            READ_PARAM(type2);
            READ_PARAM(addr3);
            it = sc_iterator5_f_a_f_a_f_new(addr1, type1, addr2, type2, addr3);
            break;

        default:
            return SCTP_ERROR;
        }

        Q_ASSERT(it != nullptr);

        // create results data
        QByteArray results;
        QBuffer buffer(&results);
        sc_uint32 results_count = 0;
        sc_addr addr;

        buffer.open(QBuffer::WriteOnly);
        while (sc_iterator5_next(it) == SC_TRUE)
        {
            results_count++;
            for (sc_uint i = 0; i < 5; i++)
            {
                addr = sc_iterator5_value(it, i);
                buffer.write((const char*)&addr, sizeof(addr));
            }
        }
        buffer.close();

        // write result
        writeResultHeader(SCTP_CMD_ITERATE_ELEMENTS, cmdId, SCTP_RESULT_OK, results.size() + sizeof(results_count), outDevice);
        outDevice->write((const char*)&results_count, sizeof(results_count));
        if (results_count > 0)
            outDevice->write((const char*)results.constData(), results.size());

        sc_iterator5_free(it);
    }

    return SCTP_NO_ERROR;
}


sctpErrorCode sctpCommand::processFindElementBySysIdtf(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    sc_addr addr;
    sc_uint32 data_len = 0;
    sc_char *data = 0;

    Q_UNUSED(cmdFlags);

    Q_ASSERT(params != 0);

    // read length of content data
    READ_PARAM(data_len);
    Q_ASSERT(data_len > 0);

    data = new sc_char[data_len];
    if (params->readRawData(data, data_len) != data_len)
    {
        delete[] data;
        return SCTP_ERROR_CMD_READ_PARAMS;
    }

    sc_addr result;
    if (sc_helper_find_element_by_system_identifier(data, data_len, &result) != SC_RESULT_OK)
        writeResultHeader(SCTP_CMD_FIND_ELEMENT_BY_SYSITDF, cmdId, SCTP_RESULT_FAIL, 0, outDevice);
    else
    {
        writeResultHeader(SCTP_CMD_FIND_ELEMENT_BY_SYSITDF, cmdId, SCTP_RESULT_OK, sizeof(sc_addr), outDevice);
        outDevice->write((const char*)&result, sizeof(sc_addr));
    }
    delete[] data;

    return SCTP_NO_ERROR;
}

sctpErrorCode sctpCommand::processSetSysIdtf(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    sc_addr addr;
    sc_uint32 data_len = 0;
    sc_char *data = 0;

    Q_UNUSED(cmdFlags);

    Q_ASSERT(params != 0);

    // read length of content data
    READ_PARAM(addr);
    READ_PARAM(data_len);
    Q_ASSERT(data_len > 0);

    data = new sc_char[data_len];
    if (params->readRawData(data, data_len) != data_len)
    {
        delete[] data;
        return SCTP_ERROR_CMD_READ_PARAMS;
    }

    writeResultHeader(SCTP_CMD_FIND_ELEMENT_BY_SYSITDF, cmdId, sc_helper_set_system_identifier(addr, data, data_len) != SC_RESULT_OK ? SCTP_RESULT_FAIL : SCTP_RESULT_OK, 0, outDevice);

    delete[] data;

    return SCTP_NO_ERROR;
}

sctpErrorCode sctpCommand::processStatistics(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    quint64 begin_time;
    quint64 end_time;

    Q_UNUSED(cmdFlags);

    Q_ASSERT(params != 0);
    READ_PARAM(begin_time);
    READ_PARAM(end_time);

    tStatItemVector stat;
    sctpStatistic::getInstance()->getStatisticsInTimeRange(begin_time, end_time, stat);

    writeResultHeader(SCTP_CMD_STATISTICS, cmdId, SCTP_RESULT_OK, sizeof(quint32) + sStatItem::realSize() * stat.size(), outDevice);
    // write result
    quint32 res_count = stat.size();
    outDevice->write((const char*)&res_count, sizeof(res_count));
    for (quint32 idx = 0; idx < res_count; ++idx)
        outDevice->write((const char*)&(stat[idx]), sStatItem::realSize());

    return SCTP_NO_ERROR;
}
