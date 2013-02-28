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

#ifndef _sctpStat_h_
#define _sctpStat_h_

#include <QObject>


class QTimer;

//! Structure to store statistic information for on time value
struct sStatItem
{
    quint64 mTime; // unix time
    quint64 mNodeCount; // number of all nodes
    quint64 mArcCount; // number of all arcs
    quint64 mLiksCount; // number of all links
    quint64 mLiveNodeCount; // number of live nodes
    quint64 mLiveArcCount; // number of live arcs
    quint64 mLiveLinkCount; // number of live links
    quint64 mClientsCount;  // number of collected clients
    quint64 mCommandsCount; // number of processed commands
    bool mIsInitStat;   // flag on initial stat save
};

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
            delete mItems;
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
    bool initialize(const QString &statDirPath, quint32 updatePeriod);
    //! Shutdown statistics
    void shutdown();

    //! Returns pointer to singleton instance
    static sctpStatistic* getInstance();

protected:
    //! Path to directory to store statistics information
    QString mStatPath;
    //! Time period to collect statistics (seconds)
    quint32 mStatUpdatePeriod;
    //! Statistics update timer
    QTimer *mStatUpdateTimer;
    //! Flag that determine, if statistics was updated on start
    bool mStatInitUpdate;

private:
    static sctpStatistic *mInstance;

protected slots:
    void update();
};

#endif // _sctpStat_h_
