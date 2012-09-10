#include "segmentview.h"
#include "segmentscene.h"
#include "segmentitem.h"

#include <QGraphicsTextItem>
#include <QFontMetrics>

extern "C"
{
#include "sc-store/sc_store.h"
#include "sc-store/sc_element.h"
}

SegmentView::SegmentView(QWidget *parent) :
    QGraphicsView(parent),
    mSegmentId(-1)
{
    setScene(new SegmentScene(this));

    // test --->
//    updateSegmentItems();
//    SegmentItem *item = 0;
//    foreach(item, mSegmentItems)
//    {
//        switch (qrand() % 6)
//        {
//        case 0:
//            item->setScType(sc_type_node);
//            break;
//        case 1:
//            item->setScType(sc_type_link);
//            break;

//        case 2:
//            item->setScType(sc_type_edge_common);
//            break;

//        case 3:
//            item->setScType(sc_type_arc_common);
//            break;

//        case 4:
//            item->setScType(sc_type_arc_access);
//            break;
//        };
//    }

    // < ---
}

SegmentView::~SegmentView()
{
}

void SegmentView::updateSegmentItems()
{
    if (mSegmentItems.size() == 0)
    {
        for (quint32 i = 0; i < SEGMENT_SIZE; ++i)
        {
            SegmentItem *item = new SegmentItem();

            item->setPos(i % SEGMENT_ITEMS_IN_ROW * (item->boundingRect().width() + SEGMENT_ITEMS_OFFSET),
                         i / SEGMENT_ITEMS_IN_ROW * (item->boundingRect().height() + SEGMENT_ITEMS_OFFSET));
            scene()->addItem(item);

            mSegmentItems.push_back(item);
        }

        // draw row numbers
        qreal dh = mSegmentItems.first()->boundingRect().height();
        qreal dw = mSegmentItems.first()->boundingRect().width();
        quint32 n = SEGMENT_SIZE / SEGMENT_ITEMS_IN_ROW;
        QString value;
        for (quint32 i = 0; i <= n; i++)
        {
            value = QString("%1").arg(i * SEGMENT_ITEMS_IN_ROW);
            QGraphicsTextItem *text = scene()->addText(value);
            QFontMetrics metrics(text->font());
            QRect rc = metrics.boundingRect(value);

            text->setPos(-SEGMENT_ITEMS_OFFSET - dw / 2.f - rc.width(),
                         (dh + SEGMENT_ITEMS_OFFSET) * i - rc.height() / 2.f + 1);
        }
    }

    //! @todo update segment items info
    SegmentItem *item = 0;
    quint32 i = 0;
    foreach (item, mSegmentItems)
    {
        sc_addr addr;
        addr.seg = mSegmentId;
        addr.offset = i++;

        sc_element *el = sc_storage_get_element(addr, SC_TRUE);

        item->setScType(el->type);
        item->setDeletionTimeStamp(el->delete_time_stamp);
        item->setScAddr(addr);
    }
}

void SegmentView::setSegmentId(quint32 segId)
{
    mSegmentId = segId;
    updateSegmentItems();
}

quint32 SegmentView::segmentId() const
{
    return mSegmentId;
}

void SegmentView::reset()
{

}
