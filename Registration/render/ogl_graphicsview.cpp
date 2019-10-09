#include "stdafx.h"
#include "camera_opengl_item.h"
#include "ogl_graphicsview.h"
#include <QGLWidget>
#include <QtMath>
#include <QMouseEvent>
#include "figure.h"
#include "app_model.h"


OGLGraphicsView::OGLGraphicsView(QGraphicsScene* scene, QWidget* parent) : 
    QGraphicsView(scene,parent),
    m_enableDraw(false),
    m_drawStatus(false),
    m_path(nullptr),
    m_figureType(kFreehand)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //setViewport(new QGLWidget(QGLFormat(QGL::NoDepthBuffer | QGL::NoStencilBuffer | QGL::NoSampleBuffers)));//QGL::SampleBuffers 
    setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    //QMatrix matrix;
    //matrix.scale(1, -1);
    //setMatrix(matrix);
}

OGLGraphicsView::~OGLGraphicsView()
{
    for (auto figure : m_figureArray)
    {
        if (figure)
        {
            delete figure;
            figure = nullptr;
        }
    }
}

void OGLGraphicsView::mousePressEvent(QMouseEvent* event)
{
    if(!m_enableDraw)
        return QGraphicsView::mousePressEvent(event);

    if (Qt::LeftButton == event->button())
    {
        if (m_drawStatus == false)
        {
            m_drawStatus = true;
            m_startPoint = m_endPoint = inverseY(event->pos());
            if (m_figureType == kFreehand)
            {
                m_path = new QPainterPath;
                m_path->moveTo(inverseY(event->pos()));
            }
        }
    }
}

void OGLGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_enableDraw)
        return QGraphicsView::mouseMoveEvent(event);

    if (m_drawStatus)
    {
        m_endPoint = inverseY(event->pos());
        if (m_figureType == kFreehand)
        {
            m_path->lineTo(inverseY(event->pos()));
        }
    }
}

void OGLGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    if (!m_enableDraw)
        return QGraphicsView::mouseReleaseEvent(event);

    Figure* currentFigure = nullptr;
    switch (m_figureType)
    {
    case kOval:
        currentFigure = new Oval(m_startPoint.rx(), m_startPoint.ry(), m_endPoint.rx() - m_startPoint.rx(), m_endPoint.ry() - m_startPoint.ry());
        m_figureArray.push_back(currentFigure);
        m_drawStatus = false;
        break;
    case kRectangle:
        currentFigure = new RectangleShape(m_startPoint.rx(), m_startPoint.ry(), m_endPoint.rx() - m_startPoint.rx(), m_endPoint.ry() - m_startPoint.ry());
        m_figureArray.push_back(currentFigure);
        m_drawStatus = false;
        break;
    case kFreehand:
        if (m_path)
        {
            currentFigure = new Freehand(*m_path);
            m_figureArray.push_back(currentFigure);
            delete m_path;
            m_path = nullptr;
        }
        break;
    default:
        break;
    }
    currentFigure = nullptr;
    m_drawStatus = false;
}

void OGLGraphicsView::drawForeground(QPainter *painter, const QRectF &rect)
{
    QGraphicsView::drawForeground(painter, rect);
    if (m_enableDraw)
    {
        painter->save();

        //painter->resetMatrix();
        //painter->resetTransform();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setRenderHint(QPainter::HighQualityAntialiasing);
        painter->setPen(QPen(Qt::red, 5));

        for (auto figure : m_figureArray)
        {
            figure->draw(*painter);
        }
        if (m_drawStatus)
        {
            switch (m_figureType)
            {
            case kOval:
                painter->drawEllipse(m_startPoint.rx(), m_startPoint.ry(), m_endPoint.rx() - m_startPoint.rx(), m_endPoint.ry() - m_startPoint.ry());
                break;
            case kRectangle:
                painter->drawRect(m_startPoint.rx(), m_startPoint.ry(), m_endPoint.rx() - m_startPoint.rx(), m_endPoint.ry() - m_startPoint.ry());
                break;
            case kFreehand:
                if (m_path != nullptr)
                {
                    painter->drawPath(*m_path);
                }
                break;
            default:
                break;
            }
        }

        painter->restore();
    }

    if (m_RecordEnabled)
    {
        auto pView = dynamic_cast<QGLWidget*>(viewport());
        if (pView)
        {
            QImage frameImage = pView->grabFrameBuffer();
            emit recordFrameReady(frameImage);
        }
    }
}

void OGLGraphicsView::cancel()
{
    if (!m_figureArray.empty())
    {
        m_figureArray.pop_back();
    }
}

QPoint OGLGraphicsView::inverseY(const QPoint& pt)
{
    QPointF pt_scene = mapToScene(pt);
    return QPoint(pt_scene.x(), pt_scene.y());
    //return pt;
}

void OGLGraphicsView::onAnnotationModeChanged(bool enabled)
{
    m_enableDraw = enabled;
}

void OGLGraphicsView::wheelEvent(QWheelEvent* pWheelEvent)
{
    //if (pWheelEvent->modifiers() & Qt::ControlModifier)
    //{
    //    //if (pWheelEvent->delta() > 0)
    //    //{
    //    //    emit mouseWheelZoom(true);
    //    //}
    //    //else
    //    //{
    //    //    emit mouseWheelZoom(false);
    //    //}

    //    // Do a wheel-based zoom about the cursor position
    //    double angle = pWheelEvent->angleDelta().y();
    //    double factor = qPow(1.2, pWheelEvent->delta() / 240.0);

    //    auto targetViewportPos = pWheelEvent->pos();
    //    auto targetScenePos = mapToScene(pWheelEvent->pos());

    //    //QMatrix matrix = this->matrix();
    //    //matrix.scale(factor, factor);
    //    //setMatrix(matrix);

    //    scale(factor, factor);
    //    centerOn(targetScenePos);
    //    QPointF deltaViewportPos = targetViewportPos - QPointF(viewport()->width() / 2.0, viewport()->height() / 2.0);
    //    QPointF viewportCenter = mapFromScene(targetScenePos) - deltaViewportPos;
    //    centerOn(mapToScene(viewportCenter.toPoint()));

    //    for(auto item : items()) 
    //    {
    //        auto openGlItem = dynamic_cast<CameraOpenGLItem*>(item);
    //        if (openGlItem)
    //        {
    //            auto center = openGlItem->getViewport().center();
    //            auto scalechange = factor - 1.0;
    //            auto offsetX = -(center.x() * scalechange) / openGlItem->getViewport().width();
    //            auto offsetY = -(center.y() * scalechange) / openGlItem->getViewport().height();
    //            openGlItem->setViewport(openGlItem->getViewport());
    //        }
    //    }

    //    return;
    //}

    QGraphicsView::wheelEvent(pWheelEvent);
}
