#pragma once

#include<QPainter>

class Figure
{
public:
    Figure() {};
    virtual ~Figure() {};
    virtual void draw(QPainter& paint) { Q_UNUSED(paint); };
};

class Freehand : public Figure
{
public:
    Freehand(QPainterPath paths);
    virtual void draw(QPainter& paint);

private:
    QPainterPath paths;
};

class RectangleShape : public Figure
{
public:
    RectangleShape(int spx, int spy, int epx, int epy);
    virtual void draw(QPainter& paint);

protected:
    int m_startPointX;
    int m_startPointY;
    int m_endPointX;
    int m_endPointY;
};

class Oval : public RectangleShape
{
public:
    Oval(int spx, int spy, int epx, int epy);
    void draw(QPainter &paint);
};




