#include "stdafx.h"
#include "camera_item.h"
#include <QPainter>
#include <QStyle>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene>
#include <QEventLoop>
#include <QTimer>
#include "app_model.h"

CameraItem::CameraItem(Utility::e_StreamType streamType, const QSizeF& size, bool isCheckable, QGraphicsItem *parent)
    : QGraphicsRectItem(0,0, size.width(),size.height(), parent),
    m_streamType(streamType), m_size(size), m_checked(false), m_isCheckable(isCheckable)
{
}

QRectF CameraItem::boundingRect() const
{
    //return QRectF(QPointF(), m_size);
    return rect();
}

void CameraItem::setImage(const QImage& stream)
{
    m_stream = stream;
    update();

}

void CameraItem::setSize(const QSizeF &size)
{
    m_size = size;
    update();
}

void CameraItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QRectF rect = this->rect();

    bool drawIcon = false;
    if (!AppModelSingleton->getDownCamPreviewEnabled() && m_streamType == Utility::e_DownCam)
    {
        drawCamIcon(painter, "icon-downcam-disable");
        drawIcon = true;
    }

    if (!AppModelSingleton->getWebCamPreviewEnabled() && m_streamType == Utility::e_WebCam)
    {
        drawCamIcon(painter, "icon-webcam-disable");
        drawIcon = true;
    }

    if (!AppModelSingleton->getDesktopPreviewEnabled() && m_streamType == Utility::e_Desktop)
    {
        drawCamIcon(painter, "icon-monitor-press");
        drawIcon = true;
    }

    if (!AppModelSingleton->getMatPreviewEnabled() && m_streamType == Utility::e_Mat)
    {
        drawCamIcon(painter, "icon-touchmat-press");
        drawIcon = true;
    }

    if (!drawIcon && !m_stream.isNull())
        painter->drawImage(rect, m_stream);

    if (m_checked)
    {
        QImage selectedIcon("://icons/selected.png");
        QRectF rect_icon(0.7 * rect.width(), 0.5 * rect.height(), selectedIcon.width(), selectedIcon.height());
        painter->drawImage(rect_icon, selectedIcon);
    }
}

void CameraItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsRectItem::mousePressEvent(event);
    if (event->button() == Qt::LeftButton) 
    {
        if (m_isCheckable)
        {
            m_checked = !m_checked;
            update();

            if (AppModelSingleton->getPresentMode())
            {
                switch (m_streamType)
                {
                case Utility::e_DownCam:
                    AppModelSingleton->setDownCamPreviewEnabled(true);
                case Utility::e_WebCam:
                    AppModelSingleton->setWebCamPreviewEnabled(true);
                case Utility::e_Desktop:
                    AppModelSingleton->setDesktopPreviewEnabled(true);
                case Utility::e_Mat:
                    AppModelSingleton->setMatPreviewEnabled(true);
                default:
                    break;
                }
            }
        }
    }
}

void CameraItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    //if (event->modifiers() & Qt::ShiftModifier) 
    //{
    //    //stuff << event->pos();
    //    //update();
    //    return;
    //}
    QGraphicsRectItem::mouseMoveEvent(event);
}

void CameraItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsRectItem::mouseReleaseEvent(event);
}

QVariant CameraItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsRectItem::ItemSelectedChange)
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
    return QGraphicsRectItem::itemChange(change, value);
}

bool CameraItem::isChecked()
{
    return m_isCheckable && m_checked;
}

void CameraItem::setFreeze(bool freeze)
{
    switch (m_streamType)
    {
    case Utility::e_DownCam:
        AppModelSingleton->setDownCamFreezeMode(freeze);
        break;
    case Utility::e_WebCam:
        AppModelSingleton->setWebCamFreezeMode(freeze);
        break;
    case Utility::e_Desktop:
        AppModelSingleton->setDesktopFreezeMode(freeze);
        break;
    case Utility::e_Mat:
        AppModelSingleton->setMatFreezeMode(freeze);
        break;
    default:
        break;
    }
}

void CameraItem::drawCamIcon(QPainter* painter, const QString& iconName)
{
    QImage disabledIcon("://icons/"+ iconName +".png");
    QRectF rect_icon((rect().width() - disabledIcon.width()) / 2.0, (rect().height() - disabledIcon.height()) / 2.0,
        disabledIcon.width(), disabledIcon.height());
    painter->drawImage(rect_icon, disabledIcon);
    return;
}

void CameraItem::setChecked(bool checked)
{
    if (m_isCheckable)
        m_checked = checked;
}