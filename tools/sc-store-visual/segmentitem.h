#ifndef SEGMENTITEM_H
#define SEGMENTITEM_H

extern "C"
{
#include "sc-store/sc_types.h"
}
#include <QGraphicsRectItem>

class SegmentItem : public QGraphicsRectItem
{

public:
    explicit SegmentItem();
    virtual ~SegmentItem();

    //! Setup sc-type of showed sc-element
    void setScType(const sc_type &type);
    //! Returns sc-type of showed sc-element
    const sc_type& scType() const;

    //! Setup sc-addr of showed sc-element
    void setScAddr(const sc_addr &addr);
    //! Return sc-add of showed sc-element
    const sc_addr& scAddr() const;

    void setDeletionTimeStamp(quint32 value);
    quint32 deletionTimeStamp() const;

private:
    //! Sc-type of element showed with item
    sc_type mScType;
    //! Sc-addr of element showed with this item
    sc_addr mScAddr;
    //! Deletion time stamp
    quint32 mDeleteTimeStamp;
    
signals:
    
public slots:
    
};

#endif // SEGMENTITEM_H
