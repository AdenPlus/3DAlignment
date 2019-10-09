#pragma once

//#include <GL/glew.h>
#include "camera_item.h"
#include "camera_layout_item.h"
#include "def.h"

#include <QTouchEvent>
#include <QWidget>
#include <QSharedPointer>
#include <QGestureEvent>
#include <QOpenGLFramebufferObject>
#include <QTransform>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QMutex>
#include <QTimer>
#include <pcl/common/common.h>
#include <pcl/PolygonMesh.h>

#include <glm/mat4x4.hpp> // glm::mat4

#include "camera.h"

typedef struct _SinglePoly
{
    float* vertices_;
    float* colors_;
    GLenum mode_;
    GLuint nvertices_;
} SinglePoly;

class CameraOpenGLItem : public QGraphicsItem, protected QOpenGLFunctions
{
//Q_OBJECT

protected:
    struct ShaderData : public QObject
    {
        enum InterpolationType
        {
            Nearest = 1,
            Linear = 2
        };

        QString shaderName;
        QSharedPointer<QOpenGLShaderProgram> filtersProgram;
        QSharedPointer<QOpenGLBuffer> vertexBuffer;
        QSharedPointer<QOpenGLTexture> inputTexture;
        QSize imageSize;
        QMatrix4x4 projectionMatrix;
        QOpenGLTexture::PixelFormat pixelFormat;
        InterpolationType interpolationType;
        float contrast;
        QVector3D hsl;
    };
public:
    enum DisplayType
    {
        PairOne = 1,
        PairTwo = 2,
        Result = 3
    };

    CameraOpenGLItem(Utility::e_StreamType streamType, const QRect& viewport, DisplayType e_type);

    // Paint the batch using a custom implementation.
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void setViewport(const QRect viewport) { m_viewport = viewport; };
    const QRect& getViewport() { return m_viewport; };

    QRectF boundingRect() const Q_DECL_OVERRIDE;

    void setPoints(const std::vector<pcl::PointXYZ, Eigen::aligned_allocator<pcl::PointXYZ>>& arrPoints);

    void setAllPoints(const std::vector<pcl::PointXYZ, Eigen::aligned_allocator<pcl::PointXYZ>>& cp1,
        const std::vector<pcl::PointXYZ, Eigen::aligned_allocator<pcl::PointXYZ>>& cp2);

    void setDisplayType(DisplayType type) { m_displayType = type; };

    void setTransform(QMatrix4x4 matrix);

    void setMesh(pcl::PolygonMesh::Ptr pMesh, bool bDrawMesh);

protected:
    virtual QSharedPointer<ShaderData> createShader();

    virtual void drawCustomize() {};

    QSharedPointer<ShaderData> getShaderData() { return m_contextData; };

    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

    virtual void wheelEvent(QGraphicsSceneWheelEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;

private slots:

    void onTimeoutMonitor();

private:
    QSharedPointer<ShaderData> initShader(const QString& shaderName);
    void buildVertexBuffer(QSharedPointer<ShaderData> data);
    QSharedPointer<QOpenGLShaderProgram> createAndCompileShaderProgram(QString programName);
    void readAndCompileShaderFile(QOpenGLShader *shader, QString shaderFileName);
    QSharedPointer<QOpenGLTexture> createTexture(QSize size, 
        ShaderData::InterpolationType interpolationType, 
        QOpenGLTexture::TextureFormat format = QOpenGLTexture::RGB8_UNorm);

    void drawTexture(QSharedPointer<ShaderData> &data, const void* inputBuffer);

private:
    QSharedPointer<ShaderData> m_contextData;
    QString m_shaderName;
    Utility::RenderSourceType m_renderSourceType;
    QPointF mouse_pos_parent_coords;
    qreal m_factor;
    QRect m_viewport;

    GLuint VBO;

    std::vector<pcl::PointXYZ, Eigen::aligned_allocator<pcl::PointXYZ>> m_arrPoints;
    std::vector<float> m_arrVertex;    

    QPointF m_initPos;
    bool m_inMove;

    glm::mat4 m_modelMatrix;
    DisplayType m_displayType;

    std::vector<pcl::PointXYZ, Eigen::aligned_allocator<pcl::PointXYZ>> m_cp1;
    std::vector<pcl::PointXYZ, Eigen::aligned_allocator<pcl::PointXYZ>> m_cp2;

    QTimer* m_timer_monitor;

    glm::mat4 m_View;
    QSharedPointer<Camera> m_camera;

    bool m_firstMouse = true;

    float m_lastX;
    float m_lastY;

    bool m_bDrawMesh;

    std::vector<SinglePoly> m_polygons;
};

