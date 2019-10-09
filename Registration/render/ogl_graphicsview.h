#ifndef OGLGRAPHICSVIEW_H
#define OGLGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QVector>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include "def.h"

class Figure;
class OGLGraphicsView : public QGraphicsView
{
    Q_OBJECT
    ADD_FIELD(bool, RecordEnabled, false)
public:
    enum FigureType 
    {
        kFreehand,
        kRectangle,
        kOval
    };

    OGLGraphicsView(QGraphicsScene* scene, QWidget* parent = Q_NULLPTR);
    virtual ~OGLGraphicsView();

signals:
    void recordFrameReady(QImage frameImage);

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;

    virtual void drawForeground(QPainter *painter, const QRectF &rect);

private slots:
    void onAnnotationModeChanged(bool enabled);

private:
    void cancel();
    QPoint inverseY(const QPoint& pt);

private:
    bool m_enableDraw;
    bool m_drawStatus;
    QPoint m_startPoint;
    QPoint m_endPoint;
    QPainterPath* m_path;
    QVector<Figure*> m_figureArray;
    FigureType m_figureType;

    QSharedPointer<QOpenGLShaderProgram> m_program;
    QOpenGLBuffer m_color_vbo;
    QOpenGLBuffer m_mesh_vbo;
    QVector<QVector3D> m_vertices;
    QVector<QVector3D> m_vertColors;    
};

Q_DECLARE_METATYPE(OGLGraphicsView*)

#endif // OGLGRAPHICSVIEW_H
