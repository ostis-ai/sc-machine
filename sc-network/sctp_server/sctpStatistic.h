/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sctpStatistic_h_
#define _sctpStatistic_h_

#include <QObject>

#include "../sctp_client/sctpTypes.hpp"


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
        return sizeof(sStatItem);
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
