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

#ifndef SCTP_COMMAND_H
#define SCTP_COMMAND_H

#include <QObject>

#include "sctpTypes.h"

class QIODevice;

/*! Base class for sctp commands.
 * It provide command packing/unpacking to binary data.
 * All types of command processed there in one function by using switch/case
 * statement to provide maximum speed.
 */
class sctpCommand : public QObject
{
    Q_OBJECT
public:
    explicit sctpCommand(QObject *parent = 0);
    virtual ~sctpCommand();

    /*! Read and process command from buffer
     * @param inDevice Pointer to device for input data reading
     * @param outDevice Pointer to device for output data writing
     */
    sctpErrorCode processCommand(QIODevice *inDevice, QIODevice *outDevice);

    /*! Wait while specified number of bytes will be available in specified data stream
     * @param stream Pointer to data stream to wait available bytes
     * @param bytesNum Number of waiting bytes
     * @returns If specified \b bytesNum available in \b stream, then return true; otherwise return false
     */
    bool waitAvailableBytes(QIODevice *stream, quint32 bytesNum);

    /*! Writes result header into output device
     * @param cmdCode Code of processed command
     * @param cmdId Processed command id
     * @param resCode Return result code
     * @param resSize Size of result data (in bytes)
     * @param outDevice Pointer to output device (use to write header)
     */
    void writeResultHeader(sctpCommandCode cmdCode, quint32 cmdId, sctpResultCode resCode, quint32 resSize, QIODevice *outDevice);

    //! Return size of command header in bytes
    static quint32 cmdHeaderSize();
    
protected:
    //! Type of command processing function
    typedef sctpErrorCode (*fProcessCommand)(quint8 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice);

private:
    // ------- processing functions ----------
    sctpErrorCode processCheckElement(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice);
    sctpErrorCode processGetElementType(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice);
    sctpErrorCode processElementErase(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice);
    sctpErrorCode processCreateNode(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice);
    sctpErrorCode processCreateLink(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice);
    sctpErrorCode processCreateArc(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice);


    sctpErrorCode processGetLinkContent(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice);
    sctpErrorCode processFindLinks(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice);
    sctpErrorCode processIterateElements(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice);

    sctpErrorCode processFindElementBySysIdtf(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice);

signals:
    
public slots:
    
};



#endif // SCTP_COMMAND_H
