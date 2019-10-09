#ifndef SIZEGRIPITEM_H
#define SIZEGRIPITEM_H

#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QList>
#include <QSharedPointer>

class Resizer : public QObject
{
    Q_OBJECT
public:
    virtual void operator()(QGraphicsItem*, const QRectF&) {};
signals:
    void sigResize();
};

class SizeGripItem : public QGraphicsItem
{
    enum e_Corner
    {
        e_Top = 0x1,
        e_Bottom = 0x2,
        e_Left = 0x4,
        e_TopLeft = e_Top | e_Left,
        e_BottomLeft = e_Bottom | e_Left,
        e_Right = 0x8,
        e_TopRight = e_Top | e_Right,
        e_BottomRight = e_Bottom | e_Right
    };

    class HandleItem : public QGraphicsRectItem
    {
    public:
        HandleItem(int positionFlags, SizeGripItem* parent);
        int positionFlags() const;

    protected:
        virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    private:
        QPointF restrictPosition(const QPointF& newPos);

        int m_positionFlags;
        SizeGripItem* m_parent;
    };

public:
    SizeGripItem(QGraphicsItem* parent = 0);
    virtual ~SizeGripItem();
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    void setTopLeft(const QPointF& pos);
    void setTop(qreal y);
    void setTopRight(const QPointF& pos);
    void setRight(qreal x);
    void setBottomRight(const QPointF& pos);
    void setBottom(qreal y);
    void setBottomLeft(const QPointF& pos);
    void setLeft(qreal x);

    QSharedPointer<Resizer> getResizer() { return m_resizer;};

private:
    void doResize();
    void updateHandleItemPositions();

    QList<HandleItem*> m_handleItems;
    QRectF m_rect;
    QSharedPointer<Resizer> m_resizer;
};

class RectResizer : public Resizer
{
public:
    virtual void operator()(QGraphicsItem* item, const QRectF& rect)
    {
        QGraphicsRectItem* rectItem = dynamic_cast<QGraphicsRectItem*>(item);
        if (rectItem)
        {
            rectItem->setRect(rect);
            emit sigResize();
        }
    }
};

#endif // SIZEGRIPITEM_H
