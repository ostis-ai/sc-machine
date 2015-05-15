/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sctpStatistic.h"

#include <QTimer>
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include <QMutex>

extern "C"
{
#include "sc_memory.h"
}


sctpStatistic* sctpStatistic::mInstance = 0;

sctpStatistic* sctpStatistic::getInstance()
{
    Q_ASSERT(mInstance != 0);
    return mInstance;
}

sctpStatistic::sctpStatistic(QObject *parent)
    : QObject(parent)
    , mStatUpdatePeriod(0)
    , mStatUpdateTimer(0)
    , mStatInitUpdate(true)
    , mContext(0)
{
    Q_ASSERT(mInstance == 0);
    mInstance = this;
}

sctpStatistic::~sctpStatistic()
{
    mInstance = 0;
}

bool sctpStatistic::initialize(const QString &statDirPath, quint32 updatePeriod, sc_memory_context *context)
{
    mStatPath = statDirPath;
    mStatUpdatePeriod = updatePeriod;
    mContext = context;

    memset(&mCurrentStat, 0, sizeof(mCurrentStat));

    mDataMutex = new QMutex(QMutex::Recursive);
    mFsMutex = new QMutex(QMutex::Recursive);

    // create statistics directory
    if (mStatUpdatePeriod > 0)
    {
        // create directory, that would contain statistics
        QDir dir(mStatPath);
        if (!dir.isAbsolute())
        {
            qCritical() << "Path to statistics directory must to be an absolute\n";
            return false;
        }

        if (!dir.exists())
        {
            if (!dir.mkpath(mStatPath))
            {
                qCritical() << QString("Can't create statistics path: '%1'").arg(mStatPath);
                return false;
            }
        }

        mStatUpdateTimer = new QTimer(this);
        update();
    }

    return true;
}

void sctpStatistic::shutdown()
{
    mContext = 0;

    delete mDataMutex;
    mDataMutex = 0;

    delete mFsMutex;
    mFsMutex = 0;
}


void sctpStatistic::update()
{
    QMutexLocker dataLocker(mDataMutex);
    QMutexLocker fsLocker(mFsMutex);

    if (mStatInitUpdate)
    {
        //! @todo write startup statistics
        mStatInitUpdate = false;
    }

    // determine date
    QDateTime dateTime(QDateTime::currentDateTime());
    QString statFileName = QString("%1_%2_%3").arg(dateTime.date().day()).arg(dateTime.date().month()).arg(dateTime.date().year());
    QDir statDir(mStatPath);
    QString statFilePath = statDir.filePath(statFileName);

    sStat stat;

    // check if stat file exist
    QFile file(statFilePath);
    QFileInfo fileInfo(statFilePath);
    if (fileInfo.exists())
    {
        // read exist information in file
        if (file.open(QFile::ReadOnly))
        {
            if (file.read((char*)&stat.mCount, sizeof(stat.mCount)) != sizeof(stat.mCount))
                stat.mCount = 0;

            if (stat.mCount > 0)
            {
                int bytesToRead = sizeof(sStatItem) * stat.mCount;
                stat.mItems = new sStatItem[stat.mCount + 1];
                if (file.read((char*)&stat.mItems[0], bytesToRead) != bytesToRead)
                    stat.mCount = 0;
            }

            file.close();
        }else
            qCritical() << "Can't open statistic file: " <<  statFilePath;
    }

    // collect information
    mCurrentStat.mTime = dateTime.toMSecsSinceEpoch();
    mCurrentStat.mIsInitStat = mStatInitUpdate ? 1 : 0;

    sc_stat mem_stat;
    if (sc_memory_stat(mContext, &mem_stat) == SC_RESULT_OK)
    {
        mCurrentStat.mArcCount = mem_stat.arc_count;
        mCurrentStat.mNodeCount = mem_stat.node_count;
        mCurrentStat.mLinksCount = mem_stat.link_count;

        mCurrentStat.mEmptyCount = mem_stat.empty_count;
    }

    // append new information
    if (stat.mItems == 0)
        stat.mItems = new sStatItem[stat.mCount + 1];
    stat.mItems[stat.mCount] = mCurrentStat;
    stat.mCount++;

    // save to file
    if (file.open(QFile::WriteOnly))
    {
        file.write((char*)&stat.mCount, sizeof(stat.mCount));
        file.write((char*)stat.mItems, sizeof(sStatItem) * stat.mCount);

        file.close();
    }else
        qCritical() << "Can't write statistic file: " << statFilePath;

    memset(&mCurrentStat, 0, sizeof(mCurrentStat));

    mStatUpdateTimer->singleShot(mStatUpdatePeriod * 1000, this, SLOT(update()));
}

void sctpStatistic::getStatisticsInTimeRange(quint64 beg_time, quint64 end_time, tStatItemVector &result)
{
    QMutexLocker fsLocker(mFsMutex);

    QDateTime begDate, endDate;
    begDate.setMSecsSinceEpoch(beg_time);
    endDate.setMSecsSinceEpoch(end_time);

    // collect all statistics files
    QDir statDir(mStatPath);
    QStringList statFileNames = statDir.entryList(QDir::Files);

    QDateTime fileDate;
    QString fileName, statFilePath;
    foreach (fileName, statFileNames)
    {
        // build date from file name
        QStringList values = fileName.split("_");
        if (values.size() != 3) continue; //! TODO: error reports

        fileDate.setDate(QDate(values[2].toInt(), values[1].toInt(), values[0].toInt()));

        // check if time in range
        if (fileDate >= begDate && fileDate <= endDate)
        {
            sStat stat;
            statFilePath = statDir.absoluteFilePath(fileName);
            // open file and read data from it
            QFile file(statFilePath);
            // read exist information in file
            if (file.open(QFile::ReadOnly))
            {
                if (file.read((char*)&stat.mCount, sizeof(stat.mCount)) != sizeof(stat.mCount))
                    stat.mCount = 0;

                if (stat.mCount > 0)
                {
                    int bytesToRead = sizeof(sStatItem) * stat.mCount;
                    stat.mItems = new sStatItem[stat.mCount];
                    if (file.read((char*)stat.mItems, bytesToRead) == bytesToRead)
                    {
                        // iterate internal data
                        for (quint32 idx = 0; idx < stat.mCount; idx++)
                        {
                            if (stat.mItems[idx].mTime >= beg_time && stat.mItems[idx].mTime <= end_time)
                            {
                                result.push_back(stat.mItems[idx]);
                            }
                            else
                            {
                                if (stat.mItems[idx].mTime > end_time)
                                    break; // do not process file, because we gone out of range
                            }
                        }
                    }//! TODO report error
                }

                file.close();
            }else
                qCritical() << "Can't open statistic file: " <<  statFilePath;
        }
    }

    qSort(result);
}

void sctpStatistic::clientConnected()
{
    QMutexLocker locker(mDataMutex);
    mCurrentStat.mConnectionsCount++;
}

void sctpStatistic::commandProcessed(bool error)
{
    QMutexLocker locker(mDataMutex);
    mCurrentStat.mCommandsCount++;
    if (error)
        mCurrentStat.mCommandErrorsCount++;
}
