#pragma once

#include <QGraphicsTextItem>
#include <QTextOption>
#include <QTextDocument>
#include <QPainter>

class AlignTextItem : public QGraphicsTextItem
{
public:
    explicit AlignTextItem(QGraphicsItem *parent = 0);

    virtual QRectF boundingRect() const;

    void setBoundingRect(const QRectF &newRect);
    void setAlignment(Qt::Alignment alignment);
    void setWrapMode(QTextOption::WrapMode wrap);

    virtual void	paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QRectF m_rect;
    QTextDocument* m_doc;
};
