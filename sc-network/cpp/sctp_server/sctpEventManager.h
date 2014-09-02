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

#ifndef _sctpEventManager_h_
#define _sctpEventManager_h_

#include <QObject>
#include <QMutex>
#include "sctpTypes.h"

#include <map>

class sctpCommand;

struct sEventData
{
    sc_event *event;
    sctpCommand *cmd;
    tEventId id;
};


struct sEventDataCompare
{
    bool operator() (const sEventData& lhs, const sEventData& rhs) const
    {
       return lhs.id < rhs.id;
    }
};


class sctpEventManager
{

public:
    explicit sctpEventManager();
    virtual ~sctpEventManager();

    static sctpEventManager* getSingleton();

private:
    static sctpEventManager *msInstance;

public:
    //! Initialize event manager
    bool initialize();

    //! Shutdonw event manager
    void shutdown();

    /*! Creates new event
     * @param type Event type
     * @param addr sc-addr of sc-element to create event
     * @param cmd Pointer to sctpCommand class instance that works with client. It will notified on event fires
     * @param eventId Reference to eventId container
     * @returns If event created, then function returns true and id of a created event stores in \p eventId; otherwise
     * it returns false
     */
    bool createEvent(sc_memory_context *ctx, sc_event_type type, sc_addr addr, sctpCommand *cmd, tEventId &event);

    /*! Destroys event
     * @param eventId Id of event, that need to be destroyed.
     * @see sctpCommand::createEvent
     * @returns If event destroyed, then returns true; otherwise returns false
     */
    bool destroyEvent(tEventId event);


    /*! Function to find unused event id.
     * @param eventId Reference to variable that will contains found event id
     * @returns If unused event id found, then it stored in \p eventId and function returns true;
     * otherwise function returns false.
     * @note This function is not thread safe, so you need to use it just when access to
     * mEvents dictionary synchronized
     */
    bool _getAvailableEventId(tEventId &eventId);

private:
    /*! Callback function for all events
     */
    static sc_result _eventsCallback(const sc_event *event, sc_addr arg);

protected:
    //! Array of events
    typedef std::map<tEventId, sEventData*> tScEventsMap;
    tScEventsMap mEvents;
    tEventId mLastEventId;

    QMutex mEventsMutex;
};

#endif
