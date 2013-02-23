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
#include "sctpStatistic.h"

#include <QTimer>
#include <QDir>
#include <QDateTime>
#include <QDebug>

sctpStatistic::sctpStatistic(QObject *parent)
    : QObject(parent)
    , mStatUpdatePeriod(0)
    , mStatUpdateTimer(0)
{

}

sctpStatistic::~sctpStatistic()
{

}

bool sctpStatistic::initialize(const QString &statDirPath, quint32 updatePeriod)
{
    mStatPath = statDirPath;
    mStatUpdatePeriod = updatePeriod;

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

}


void sctpStatistic::update()
{
    if (!mStatInitUpdate)
    {
        //! @todo write startup statistics
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

    //! @todo collect information
    sStatItem item;

    // append new information
    if (stat.mItems == 0)
        stat.mItems = new sStatItem[stat.mCount + 1];
    stat.mItems[stat.mCount] = item;
    stat.mCount++;

    // save to file
    if (file.open(QFile::WriteOnly))
    {
        file.write((char*)&stat.mCount, sizeof(stat.mCount));
        file.write((char*)&stat.mItems, sizeof(sStatItem) * stat.mCount);

        file.close();
    }else
        qCritical() << "Can't write statistic file: " << statFilePath;


    mStatUpdateTimer->singleShot(mStatUpdatePeriod * 1000, this, SLOT(update()));
}
