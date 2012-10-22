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

#ifndef SCTP_COMMAND_H
#define SCTP_COMMAND_H

#include "sctp_global.h"
#include <QObject>

class QIODevice;

/*! Base class for sctp commands.
 * It provide command packing/unpacking to binary data.
 * All types of command processed there in one function by using switch/case
 * statement to provide maximum speed.
 */
class SCTPSHARED_EXPORT sctpCommand : public QObject
{
    Q_OBJECT
public:
    explicit sctpCommand(QObject *parent = 0);
    virtual ~sctpCommand();

    typedef enum
    {
        SCTP_CMD_UNKNOWN            = 0x00, // unkown command
        SCTP_CMD_CHECK_ELEMENT      = 0x01, // check if specified sc-element exist
        SCTP_CMD_GET_ELEMENT_TYPE   = 0x02, // return sc-element type
        SCTP_CMD_ERASE_ELEMENT      = 0x03, // erase specified sc-element
        SCTP_CMD_CREATE_NODE        = 0x04, // create new sc-node
        SCTP_CMD_CREAET_LINK        = 0x05, // create new sc-link
        SCTP_CMD_CREATE_ARC         = 0x06, // create new sc-arc
        SCTP_CMD_GET_ARC_BEGIN      = 0x07, // return begin element of sc-arc
        SCTP_CMD_GET_ARC_END        = 0x08, // return end element of sc-arc

        SCTP_CMD_DISCONNECT         = 0xfe // disconnect client from server

    } eCommandType;

    //! Command processing result codes
    typedef enum
    {
        SCTP_RESULT_OK = 0,                     // No any errors, command processed
        SCTP_RESULT_ERROR,                      // There are any errors while processing command
        SCTP_RESULT_UNKNOWN_CMD,                // Unknown command code
        SCTP_RESULT_CMD_HEADER_READ_TIMEOUT,    // Timeout while waiting command header
        SCTP_RESULT_CMD_PARAM_READ_TIMEOUT      // Timeout while waiting command params

    } eResult;

    /*! Read and process command from buffer
     * @param inDevice Pointer to device for input data reading
     * @param outDevice Pointer to device for output data writing
     */
    eResult processCommand(QIODevice *inDevice, QIODevice *outDevice);

    /*! Wait while specified number of bytes will be available in specified data stream
     * @param stream Pointer to data stream to wait available bytes
     * @param bytesNum Number of waiting bytes
     * @returns If specified \b bytesNum available in \b stream, then return true; otherwise return false
     */
    bool waitAvailableBytes(QIODevice *stream, quint32 bytesNum);

    //! Return size of command header in bytes
    static quint32 cmdHeaderSize();
    
protected:
    //! Type of command processing function
    typedef bool (*fProcessCommand)(quint8 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice);

private:
    // ------- processing functions ----------
    bool processCheckElement(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice);

signals:
    
public slots:
    
};



#endif // SCTP_COMMAND_H
