#ifndef SEGMENTVIEW_H
#define SEGMENTVIEW_H

#include <QGraphicsView>
#include <QVector>

class SegmentItem;

// Number of segment items in one row
#define SEGMENT_ITEMS_IN_ROW    64
#define SEGMENT_ITEMS_OFFSET    2

class SegmentView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit SegmentView(QWidget *parent = 0);
    virtual ~SegmentView();

    //! Setup id of segment to visualize
    void setSegmentId(quint32 segId);
    //! Return currently visualized segment id
    quint32 segmentId() const;

    //! Reset view
    void reset();

protected:
    //! Updates segment items
    void updateSegmentItems();
    
 private:
    //! Id of visualized segment
    quint32 mSegmentId;
    //! Vector of segment items
    typedef QVector<SegmentItem*> tSegmentItemsVector;
    tSegmentItemsVector mSegmentItems;

signals:
    
public slots:
    
};

#endif // SEGMENTVIEW_H
