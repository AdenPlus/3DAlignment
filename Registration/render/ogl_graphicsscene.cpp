#include "stdafx.h"
#include "ogl_graphicsscene.h"
#include <QPaintEngine>
#include <Windows.h>
#include <qopengl.h>
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QGraphicsView>
#include <QGLWidget>


OGLGraphicsScene::OGLGraphicsScene(QWidget* parent)
    : QGraphicsScene(parent)
    , mTextItem(0)
{
    qRegisterMetaType<OGLGraphicsScene*>();
}

OGLGraphicsScene::~OGLGraphicsScene()
{

}

void OGLGraphicsScene::drawBackground(QPainter *painter, const QRectF& rect)
{
    if (painter->paintEngine()->type() != QPaintEngine::OpenGL2)
    {
        qWarning("OpenGLScene: drawBackground needs a "
                 "QGLWidget to be set as viewport on the "
                 "graphics view");
        return;
    }

    painter->beginNativePainting();

    auto f = QOpenGLContext::currentContext()->functions();

    f->glClearColor(0, 0, 0, 1);
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    painter->endNativePainting();
}

QImage OGLGraphicsScene::grabView()
{
    QGLWidget* pGLView = dynamic_cast<QGLWidget*>(views().first()->viewport());
    if(pGLView)
        return pGLView->grabFrameBuffer();
    return QImage();
}