#pragma once

#include <QGraphicsSceneMouseEvent>
#include <QSharedPointer>
#include "utility.h"
#include "camera_item.h"

class SizeGripItem;
class CameraLayoutItem : public CameraItem
{
Q_OBJECT
public:
    CameraLayoutItem(Utility::e_StreamType streamType, const QSizeF& size, QGraphicsItem *parent = Q_NULLPTR);
    CameraLayoutItem(QObject* object, const QSizeF& size, QGraphicsItem *parent = Q_NULLPTR);

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* item, QWidget* widget) Q_DECL_OVERRIDE;

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

protected:
    QSharedPointer<SizeGripItem> m_rectSizeGripItem;

private slots:
    void onResize();

};
