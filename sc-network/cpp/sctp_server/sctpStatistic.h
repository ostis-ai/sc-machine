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

#ifndef _sctpStatistic_h_
#define _sctpStatistic_h_

#include <QObject>

#include "sctpTypes.h"


class QTimer;
class QMutex;

//! Structure to store statistic information for on time value
struct sStatItem
{
    quint64 mTime; // unix time
    quint64 mNodeCount; // amount of all nodes
    quint64 mArcCount; // amount of all arcs
    quint64 mLinksCount; // amount of all links
    quint64 mEmptyCount; // amount of empty sc-elements
    quint64 mConnectionsCount;  // amount of collected clients
    quint64 mCommandsCount; // amount of processed commands (it includes commands with errors)
    quint64 mCommandErrorsCount; // amount of command, that was processed with error
    quint8 mIsInitStat;   // flag on initial stat save

    bool operator < (const sStatItem &other) const
    {
        return mTime < other.mTime;
    }

    static quint32 realSize()
    {
        return sizeof(quint64) * 11 + sizeof(mIsInitStat);
    }
};

typedef QVector<sStatItem> tStatItemVector;

/*! Structure to store statistics
 */
struct sStat
{
    quint32 mCount; // number of stat items
    sStatItem *mItems; // array of stat items

    sStat()
        : mCount(0)
        , mItems(0)
    {
    }

    ~sStat()
    {
        if (mItems)
            delete []mItems;
    }
};


/*!
 * \brief Class to work with statistics information
 */
class sctpStatistic : public QObject
{
    Q_OBJECT

public:
    explicit sctpStatistic(QObject *parent = 0);
    virtual ~sctpStatistic();

    //! Initialize statistics
    bool initialize(const QString &statDirPath, quint32 updatePeriod, sc_memory_context *context);
    //! Shutdown statistics
    void shutdown();

    //! Returns pointer to singleton instance
    static sctpStatistic* getInstance();


    /*! Collect statistics information in specified time range
     * @param beg_time Begin range time
     * @param end_time End range time
     * @param result vector, that will be filled with statistics
     */
    void getStatisticsInTimeRange(quint64 beg_time, quint64 end_time, tStatItemVector &result);



protected:
    //! Path to directory to store statistics information
    QString mStatPath;
    //! Time period to collect statistics (seconds)
    quint32 mStatUpdatePeriod;
    //! Statistics update timer
    QTimer *mStatUpdateTimer;
    //! Flag that determine, if statistics was updated on start
    bool mStatInitUpdate;
    //! Current statistic info
    sStatItem mCurrentStat;
    //! Pointer to mutex, that used to synchronize data
    QMutex *mDataMutex;
    //! Pointer to mutex, that used to synchronize filesystem routines
    QMutex *mFsMutex;

    //! Pointer to default memory context
    sc_memory_context *mContext;

public:
    void clientConnected();
    void commandProcessed(bool error);

private:
    static sctpStatistic *mInstance;

protected slots:
    void update();
};

#endif // _sctpStat_h_
