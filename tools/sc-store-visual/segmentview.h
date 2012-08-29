#ifndef SEGMENTVIEW_H
#define SEGMENTVIEW_H

#include <QGraphicsView>

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
    
 private:
    //! Id of visualized segment
    quint32 mSegmentId;

signals:
    
public slots:
    
};

#endif // SEGMENTVIEW_H
