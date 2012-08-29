#include "segmentview.h"

SegmentView::SegmentView(QWidget *parent) :
    QGraphicsView(parent),
    mSegmentId(-1)
{
    setScene(new QGraphicsScene());
}

SegmentView::~SegmentView()
{
}

void SegmentView::setSegmentId(quint32 segId)
{
    mSegmentId = segId;
}

quint32 SegmentView::segmentId() const
{
    return mSegmentId;
}
