#pragma once

#include <QColor>
#include <QObject>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include "utility.h"
#include "def.h"

class CameraItem : public QObject, public QGraphicsRectItem
{
Q_OBJECT
public:
    explicit CameraItem(Utility::e_StreamType streamType, const QSizeF& size, bool isCheckable=false, QGraphicsItem *parent = Q_NULLPTR);

    QRectF boundingRect() const Q_DECL_OVERRIDE;

    void setImage(const QImage& stream);    
    void setSize(const QSizeF &size);
    void setFreeze(bool freeze);

    bool isChecked();
    void setChecked(bool checked);

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* item, QWidget* widget) Q_DECL_OVERRIDE;

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    void drawCamIcon(QPainter* painter, const QString& iconName);

protected:
    QImage m_stream;
    Utility::e_StreamType m_streamType;
    QSizeF m_size;
    bool m_checked;
    bool m_isCheckable;
};
