/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sctpEventManager.h"
#include "sctpCommand.h"

sctpEventManager* sctpEventManager::msInstance = 0;

sctpEventManager::sctpEventManager()
    : mLastEventId(1)
{
    Q_ASSERT(msInstance == 0);
    msInstance = this;
}

sctpEventManager::~sctpEventManager()
{
    msInstance = 0;
}

sctpEventManager* sctpEventManager::getSingleton()
{
    return msInstance;
}

bool sctpEventManager::initialize()
{
    QMutexLocker locker(&mEventsMutex);

    return true;
}

void sctpEventManager::shutdown()
{
    QMutexLocker locker(&mEventsMutex);

    tScEventsMap::iterator it, itEnd = mEvents.end();
    for (it = mEvents.begin(); it != itEnd; ++it)
    {
        sEventData *evt = it->second;
        sc_event_destroy(evt->event);
        delete evt;
    }

    mEvents.clear();
}

bool sctpEventManager::createEvent(sc_memory_context *ctx, sc_event_type type, sc_addr addr, sctpCommand *cmd, tEventId &event)
{
    QMutexLocker locker(&mEventsMutex);


    if (!_getAvailableEventId(event))
        return false;

    sEventData *evt = new sEventData();

    evt->cmd = cmd;
    evt->id = event;
    evt->event = sc_event_new(ctx, addr, type, UINT_TO_POINTER(event), &sctpEventManager::_eventsCallback, 0);

    Q_ASSERT(mEvents.find(evt->id) == mEvents.end());

    mEvents[evt->id] = evt;

    return true;
}

bool sctpEventManager::destroyEvent(tEventId event)
{
    QMutexLocker locker(&mEventsMutex);

    tScEventsMap::iterator it = mEvents.find(event);

    if (it == mEvents.end())
        return false;

    Q_ASSERT(it->second->id == event);

    if (sc_event_destroy(it->second->event) != SC_RESULT_OK)
        return false;

    mEvents.erase(it);

    return true;
}

bool sctpEventManager::_getAvailableEventId(tEventId &eventId)
{
    tEventId start = mLastEventId;
    eventId = start + 1;

    while (eventId != start && (mEvents.find(eventId) != mEvents.end()) && eventId != 0)
        eventId = eventId + 1;

    if (eventId != start)
    {
        mLastEventId = eventId;
        return true;
    }

    return false;
}


sc_result sctpEventManager::_eventsCallback(const sc_event *event, sc_addr arg)
{
    QMutexLocker locker(&sctpEventManager::msInstance->mEventsMutex);

    Q_ASSERT(event != 0);

    tScEventsMap::iterator it = sctpEventManager::msInstance->mEvents.find(POINTER_TO_UINT(sc_event_get_data(event)));
    if (it == sctpEventManager::msInstance->mEvents.end())
        return SC_RESULT_ERROR_INVALID_STATE;

    sEventData *evt = it->second;
    Q_ASSERT(evt && evt->cmd);
    Q_ASSERT(event == evt->event);

    evt->cmd->processEventEmit(evt->id, sc_event_get_element(event), arg);

    return SC_RESULT_OK;
}
