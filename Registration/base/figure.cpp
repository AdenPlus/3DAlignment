#include "stdafx.h"
#include "Figure.h"

Freehand::Freehand(QPainterPath p)
{
    paths = p;
}

void Freehand::draw(QPainter &paint)
{
    paint.drawPath(paths);
}

RectangleShape::RectangleShape(int spx, int spy, int epx, int epy)
{
    m_startPointX = spx;
    m_startPointY = spy;
    m_endPointX = epx;
    m_endPointY = epy;
}

void RectangleShape::draw(QPainter &paint)
{
    paint.drawRect(m_startPointX, m_startPointY, m_endPointX, m_endPointY);
}

Oval::Oval(int spx, int spy, int epx, int epy) : RectangleShape(spx, spy, epx, epy)
{
}

void Oval::draw(QPainter &paint)
{
    paint.drawEllipse(m_startPointX, m_startPointY, m_endPointX, m_endPointY);
}
