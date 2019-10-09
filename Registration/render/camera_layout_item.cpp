#include "stdafx.h"
#include "camera_layout_item.h"
#include <QPainter>
#include <QStyle>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene>
#include "app_model.h"
#include "size_grip_item.h"

CameraLayoutItem::CameraLayoutItem(Utility::e_StreamType streamType, const QSizeF& size, QGraphicsItem *parent)
    : CameraItem(streamType,size, false, parent)
{
    if (AppModelSingleton->getDisplayMode())
    {
        setFlag(CameraItem::ItemIsMovable, true);
        setFlag(CameraItem::ItemIsSelectable, true);
        //setFlag(CameraItem::ItemIsFocusable, true);

        m_rectSizeGripItem = QSharedPointer<SizeGripItem>::create(this);
        QObject::connect(m_rectSizeGripItem->getResizer().data(), &RectResizer::sigResize, this, &CameraLayoutItem::onResize);
    }
}

void CameraLayoutItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    CameraItem::paint(painter, option, widget);
}

void CameraLayoutItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    CameraItem::mousePressEvent(event);
    if (event->button() == Qt::LeftButton) 
    {
        setCursor(QCursor(Qt::ClosedHandCursor));
        setSelected(true);
        update();
    }
}

void CameraLayoutItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    //if (event->modifiers() & Qt::ShiftModifier) 
    //{
    //    //stuff << event->pos();
    //    //update();
    //    return;
    //}
    CameraItem::mouseMoveEvent(event);
    setCursor(QCursor(Qt::OpenHandCursor));
    // value is the new position.
    //QRectF newRect = mapToScene(rect()).boundingRect();
    //QRectF rect = scene()->sceneRect();
    //if (rect.contains(newRect))
    {
        onResize(); //not good
    }

    update();
}

void CameraLayoutItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    CameraItem::mouseReleaseEvent(event);
    if (event->button() == Qt::LeftButton)
    {
        setCursor(QCursor(Qt::ArrowCursor));
        setSelected(false);
        update();
    }
}

QVariant CameraLayoutItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSelectedChange && scene())
    {
        if (value == true)
        {
            // do stuff if selected
        }
        else
        {
            // do stuff if not selected
        }
    }

    return CameraItem::itemChange(change, value);
}

void CameraLayoutItem::onResize()
{
    auto rectInScene = mapToScene(rect()).boundingRect();
    AppModelSingleton->setItemsRect(m_streamType, rectInScene.toAlignedRect());
}
