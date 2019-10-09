#include "stdafx.h"
#include "size_grip_item.h"
#include <QBrush>

#define IMPL_SET_FN(TYPE, POS)                  \
        void SizeGripItem::set ## POS (TYPE v)      \
        {                                           \
            m_rect.set ## POS (v);                   \
            doResize();                             \
        }

    IMPL_SET_FN(qreal, Top)
    IMPL_SET_FN(qreal, Right)
    IMPL_SET_FN(qreal, Bottom)
    IMPL_SET_FN(qreal, Left)
    IMPL_SET_FN(const QPointF&, TopLeft)
    IMPL_SET_FN(const QPointF&, TopRight)
    IMPL_SET_FN(const QPointF&, BottomRight)
    IMPL_SET_FN(const QPointF&, BottomLeft)


SizeGripItem::HandleItem::HandleItem(int positionFlags, SizeGripItem* parent)
    : QGraphicsRectItem(-4, -4, 8, 8, parent),
      m_positionFlags(positionFlags),
      m_parent(parent)
{
    setBrush(QBrush(Qt::green));
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
}

int SizeGripItem::HandleItem::positionFlags() const
{
    return m_positionFlags;
}

QVariant SizeGripItem::HandleItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    QVariant retVal = value;

    if (change == ItemPositionChange)
    {
        retVal = restrictPosition(value.toPointF());
    }
    else if (change == ItemPositionHasChanged)
    {
        QPointF pos = value.toPointF();

        switch (m_positionFlags)
        {
            case e_TopLeft:
                m_parent->setTopLeft(pos);
                break;
            case e_Top:
                m_parent->setTop(pos.y());
                break;
            case e_TopRight:
                m_parent->setTopRight(pos);
                break;
            case e_Right:
                m_parent->setRight(pos.x());
                break;
            case e_BottomRight:
                m_parent->setBottomRight(pos);
                break;
            case e_Bottom:
                m_parent->setBottom(pos.y());
                break;
            case e_BottomLeft:
                m_parent->setBottomLeft(pos);
                break;
            case e_Left:
                m_parent->setLeft(pos.x());
                break;
        }
    }

    return retVal;
}

QPointF SizeGripItem::HandleItem::restrictPosition(const QPointF& newPos)
{
    QPointF retVal = pos();

    if (m_positionFlags & e_Top || m_positionFlags & e_Bottom)
        retVal.setY(newPos.y());

    if (m_positionFlags & e_Left || m_positionFlags & e_Right)
        retVal.setX(newPos.x());

    if (m_positionFlags & e_Top && retVal.y() > m_parent->m_rect.bottom())
        retVal.setY(m_parent->m_rect.bottom());
    else if (m_positionFlags & e_Bottom && retVal.y() < m_parent->m_rect.top())
        retVal.setY(m_parent->m_rect.top());

    if (m_positionFlags & e_Left && retVal.x() > m_parent->m_rect.right())
        retVal.setX(m_parent->m_rect.right());
    else if (m_positionFlags & e_Right && retVal.x() < m_parent->m_rect.left())
        retVal.setX(m_parent->m_rect.left());

    return retVal;
}

SizeGripItem::SizeGripItem( QGraphicsItem* parent)
    : QGraphicsItem(parent),
      m_resizer(new RectResizer)
{
    if (parentItem())
        m_rect = parentItem()->boundingRect();

    m_handleItems.append(new HandleItem(e_TopLeft, this));
    m_handleItems.append(new HandleItem(e_Top, this));
    m_handleItems.append(new HandleItem(e_TopRight, this));
    m_handleItems.append(new HandleItem(e_Right, this));
    m_handleItems.append(new HandleItem(e_BottomRight, this));
    m_handleItems.append(new HandleItem(e_Bottom, this));
    m_handleItems.append(new HandleItem(e_BottomLeft, this));
    m_handleItems.append(new HandleItem(e_Left, this));
    updateHandleItemPositions();
}

SizeGripItem::~SizeGripItem()
{
}

QRectF SizeGripItem::boundingRect() const
{
    return m_rect;
}

void SizeGripItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(painter)
    Q_UNUSED(option)
    Q_UNUSED(widget)
}

void SizeGripItem::doResize()
{
    if (m_resizer)
    {
        (*m_resizer)(parentItem(), m_rect);
        updateHandleItemPositions();
    }
}

void SizeGripItem::updateHandleItemPositions()
{
    for(auto item : m_handleItems)
    {
        item->setFlag(ItemSendsGeometryChanges, false);

        switch (item->positionFlags())
        {
            case e_TopLeft:
                item->setPos(m_rect.topLeft());
                break;
            case e_Top:
                item->setPos(m_rect.left() + m_rect.width() / 2 - 1, m_rect.top());
                break;
            case e_TopRight:
                item->setPos(m_rect.topRight());
                break;
            case e_Right:
                item->setPos(m_rect.right(), m_rect.top() + m_rect.height() / 2 - 1);
                break;
            case e_BottomRight:
                item->setPos(m_rect.bottomRight());
                break;
            case e_Bottom:
                item->setPos(m_rect.left() + m_rect.width() / 2 - 1, m_rect.bottom());
                break;
            case e_BottomLeft:
                item->setPos(m_rect.bottomLeft());
                break;
            case e_Left:
                item->setPos(m_rect.left(), m_rect.top() + m_rect.height() / 2 - 1);
                break;
        }

        item->setFlag(ItemSendsGeometryChanges, true);
    }
}
