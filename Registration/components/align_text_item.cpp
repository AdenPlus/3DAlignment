#include "stdafx.h"
#include "align_text_item.h"

AlignTextItem::AlignTextItem(QGraphicsItem *parent)
    : QGraphicsTextItem(parent), m_doc(document())
{
}

QRectF AlignTextItem::boundingRect() const
{
    return m_rect;
}

void AlignTextItem::setBoundingRect(const QRectF &newRect)
{
    m_rect = newRect;
}

void AlignTextItem::setAlignment(Qt::Alignment alignment)
{
    QTextOption option = m_doc->defaultTextOption();
    option.setAlignment(alignment);
    m_doc->setDefaultTextOption(option);
}

void AlignTextItem::setWrapMode(QTextOption::WrapMode wrap)
{
    QTextOption option = m_doc->defaultTextOption();
    option.setWrapMode(wrap);
    m_doc->setDefaultTextOption(option);
}

void	AlignTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    QPen pen(Qt::white);
    painter->setPen(pen);
    QFont font("Segoe UI", 12);
    painter->setFont(font);
    painter->drawText(m_rect, Qt::AlignCenter, m_doc->toPlainText());
    painter->restore();
}