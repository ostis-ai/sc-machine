#include "segmentitem.h"

#include <QBrush>
#include <QPen>

SegmentItem::SegmentItem() :
    QGraphicsRectItem(),
    mScType(0),
    mDeleteTimeStamp(0)
{
    setRect(0, 0, 10, 10);
    setScType(0);
}

SegmentItem::~SegmentItem()
{

}

void SegmentItem::setScType(const sc_type &type)
{
    mScType = type;

    // update brush color
    QColor brushColor(255, 255, 255);

    if (mScType & sc_type_link)
        brushColor = QColor(85, 97, 181);
    if (mScType & sc_type_node)
        brushColor = QColor(156, 172, 165);
    if (mScType & sc_type_edge_common)
        brushColor = QColor(255, 216, 77);
    if (mScType & sc_type_arc_common)
        brushColor = QColor(232, 155, 3);
    if (mScType & sc_type_arc_access)
        brushColor = QColor(210, 130, 30);


    setBrush(QBrush(brushColor));
    setOpacity(0.6f);
}

const sc_type& SegmentItem::scType() const
{
    return mScType;
}

void SegmentItem::setScAddr(const sc_addr &addr)
{
    mScAddr = addr;
}

const sc_addr& SegmentItem::scAddr() const
{
    return mScAddr;
}

void SegmentItem::setDeletionTimeStamp(quint32 value)
{
    mDeleteTimeStamp = value;

    QBrush brush(Qt::black);

    if (mDeleteTimeStamp > 0)
        brush.setColor(Qt::red);

    setPen(QPen(brush, 1.f));
}

quint32 SegmentItem::deletionTimeStamp() const
{
    return mDeleteTimeStamp;
}
