#include "stdafx.h"
#include "camera_opengl_item.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QOpenGLFramebufferObject>
#include <QtMath>
#include <QDebug>
#include <QMutexLocker>

#include "app_model.h"
#include "size_grip_item.h"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <pcl/io/pcd_io.h>
#include <pcl/conversions.h>


//extern "C" {
//    _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
//}


CameraOpenGLItem::CameraOpenGLItem(Utility::e_StreamType streamType, const QRect& viewport, DisplayType e_type) :
    QGraphicsItem(), m_shaderName("commonShader"), m_displayType(e_type),
    m_factor(1.0), m_viewport(viewport)
{
    m_modelMatrix = glm::mat4(1.0f);

    m_inMove = false;

    switch (m_displayType)
    {
    case PairOne:
        m_camera = QSharedPointer<Camera>::create(glm::vec3(0.0f, 0.0f, 400.0f));
        break;
    case PairTwo:
        m_camera = QSharedPointer<Camera>::create(glm::vec3(0.0f, 0.0f, 400.0f));
        break;
    case Result:
        m_camera = QSharedPointer<Camera>::create(glm::vec3(0.0f, 0.0f, 400.0f));
        break;
    default:
        break;
    } 

    m_lastX = m_viewport.width() / 2.0f;
    m_lastY = m_viewport.height() / 2.0f;
    //setFlag(QGraphicsItem::ItemIgnoresTransformations);
}

// Paint the batch using a custom implementation.
void CameraOpenGLItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setPointSize(20);
    font.setLetterSpacing(QFont::AbsoluteSpacing, 5);
    painter->setFont(font);

    QString strLable;
    switch (m_displayType)
    {
    case PairOne:
        painter->setPen(QPen(Qt::red, 3, Qt::DotLine));
        strLable = "Source Point Cloud";
        break;
    case PairTwo:
        painter->setPen(QPen(Qt::green, 3, Qt::DotLine));
        strLable = "Target Point Cloud";
        break;
    case Result:
        painter->setPen(QPen(Qt::yellow, 3, Qt::DotLine));
        strLable = "Aligned Point Cloud";
        break;
    default:
        break;
    }    
    
    painter->drawText(QPoint(100, 100), strLable);
    painter->drawRect(3, 3, m_viewport.width()-10, m_viewport.height()-10);

    painter->save();
    painter->beginNativePainting();

    auto f = QOpenGLContext::currentContext()->functions();

    f->glViewport(m_viewport.x(), m_viewport.y(), m_viewport.width(), m_viewport.height());

    //f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glm::vec2 Rotate;
    // Projection matrix : 45?Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)m_viewport.width() / (float)m_viewport.height(), 0.1f, 1000.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(Projection));
    
    m_View = m_camera->GetViewMatrix();

    // Model matrix : an identity matrix (model will be at the origin)
    glm::mat4 Model = m_modelMatrix;

    // Our ModelViewProjection : multiplication of our 3 matrices
    //glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around

    glm::mat4 mvp = m_View * Model;

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(mvp));

    f->glEnable(GL_POINT_SMOOTH);
    glColor3f(1.0, 1.0, 1.0);
    glPointSize(1);

    ////draw lines 
    //glBegin(GL_LINES);
    //glColor3f(1.0, 0.0, 0.0);

    //glm::vec3 topRight(0.25, -0.5 , 1);
    //glm::vec3 bottomRight(0.25, 0.5, 1);

    //topRight = topRight * 200.f;
    //bottomRight = bottomRight * 200.f;

    //glVertex3f(topRight.x, topRight.y, topRight.z);
    //glVertex3f(bottomRight.x, bottomRight.y, bottomRight.z);
    ////glVertex3f(0.3, -0.5, 400);
    ////glVertex3f(0.3, 0.5, 400);
    //glEnd();


    if (m_displayType != Result)
    {
        glBegin(GL_POINTS);

        if(m_displayType == PairOne)
            glColor3f(1.0, 0.0, 0.0);
        else
            glColor3f(0.0, 1.0, 0.0);
        for (size_t i = 0; i < m_arrPoints.size(); ++i)
        {
            float _x = m_arrPoints.at(i).x;
            float _y = m_arrPoints.at(i).y;
            float _z = m_arrPoints.at(i).z;
            //glColor3ub(1, 0, 0);
            glVertex3f(_x, _y, _z);
        }

        glEnd();
    }
    else
    {
        if (m_bDrawMesh)
        {
            //glLineWidth(1.0f);

            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);

            glColor3f(1.0f, 1.0f, 1.0f);

            //glEnableClientState(GL_VERTEX_ARRAY);
            //glEnableClientState(GL_COLOR_ARRAY);
            for (size_t i = 0; i < m_polygons.size(); i++)
            {
                float _x1 = m_polygons[i].vertices_[0];
                float _y1 = m_polygons[i].vertices_[1];
                float _z1 = m_polygons[i].vertices_[2];

                float _x2 = m_polygons[i].vertices_[3];
                float _y2 = m_polygons[i].vertices_[4];
                float _z2 = m_polygons[i].vertices_[5];

                float _x3 = m_polygons[i].vertices_[6];
                float _y3 = m_polygons[i].vertices_[7];
                float _z3 = m_polygons[i].vertices_[8];

                //glVertex3f(_x1, _y1, _z1);
                //glVertex3f(_x2, _y2, _z2);
                //glVertex3f(_x3, _y3, _z3);

                glBegin(GL_LINES);
                glVertex3f(_x1, _y1, _z1);
                glVertex3f(_x2, _y2, _z2);
                glEnd();

                glBegin(GL_LINES);
                glVertex3f(_x2, _y2, _z2);
                glVertex3f(_x3, _y3, _z3);
                glEnd();

                glBegin(GL_LINES);
                glVertex3f(_x1, _y1, _z1);
                glVertex3f(_x3, _y3, _z3);
                glEnd();
                
                //
                //glVertexPointer(3, GL_FLOAT, 0, m_polygons[i].vertices_);
                //glColorPointer(4, GL_FLOAT, 0, m_polygons[i].colors_);
                //glColor4f(0.8f, 0.8f, 0.8f, 1.0f);
                //f->glDrawArrays(GL_TRIANGLES, 0, m_polygons[i].nvertices_);
                //
            }



            //glDisableClientState(GL_COLOR_ARRAY);
            //glDisableClientState(GL_VERTEX_ARRAY);            
        }
        else
        {
            glBegin(GL_POINTS);

            glColor3f(1.0, 0.0, 0.0);
            for (size_t i = 0; i < m_cp1.size(); ++i)
            {
                float _x = m_cp1.at(i).x;
                float _y = m_cp1.at(i).y;
                float _z = m_cp1.at(i).z;
                //glColor3ub(1, 0, 0);
                glVertex3f(_x, _y, _z);
            }

            glColor3f(0.0, 1.0, 0.0);
            for (size_t i = 0; i < m_cp2.size(); ++i)
            {
                float _x = m_cp2.at(i).x;
                float _y = m_cp2.at(i).y;
                float _z = m_cp2.at(i).z;
                //glColor3ub(1, 0, 0);
                glVertex3f(_x, _y, _z);
            }

            glEnd();
        }
    }


    painter->endNativePainting();
    painter->restore();
}

QSharedPointer<CameraOpenGLItem::ShaderData> CameraOpenGLItem::createShader()
{
    return QSharedPointer<ShaderData>::create();
}

QSharedPointer<CameraOpenGLItem::ShaderData> CameraOpenGLItem::initShader(const QString& shaderName)
{
    auto data = createShader();
    if (m_renderSourceType == Utility::e_OpenCVMat)
    {
        data->pixelFormat = QOpenGLTexture::PixelFormat::BGR;
    }
    else if (m_renderSourceType == Utility::e_QImage)
    {
        switch (QImage::Format_RGB32)
        {
        case QImage::Format_RGB32:
            data->pixelFormat = QOpenGLTexture::PixelFormat::BGRA;
            break;
        case QImage::Format_RGB888:
            data->pixelFormat = QOpenGLTexture::PixelFormat::RGB;
            break;
        default:
            data->pixelFormat = QOpenGLTexture::PixelFormat::RGB;
        }
    }
    data->interpolationType = ShaderData::InterpolationType::Linear;

    data->filtersProgram = createAndCompileShaderProgram(":/shader/" + shaderName);
    if (!data->filtersProgram)
        return QSharedPointer<ShaderData>();

    //buildVertexBuffer(data);

    data->projectionMatrix.ortho(0, 1, 1, 0, -1, 1);

    data->contrast = AppModelSingleton->getContrast()/100.0;
    data->hsl = { 0.0, 0.0, AppModelSingleton->getBrightness()/100.0f };

    return data;
}

void CameraOpenGLItem::drawTexture(QSharedPointer<ShaderData> &data,
    const void* inputBuffer)
{
    const auto functions = QOpenGLContext::currentContext()->functions();

    if(!data->inputTexture)
        data->inputTexture = createTexture(data->imageSize, data->interpolationType);

    data->inputTexture->setData(data->pixelFormat, QOpenGLTexture::UInt8, inputBuffer);
    data->inputTexture->bind();

    data->vertexBuffer->bind();
    data->filtersProgram->bind();
    data->filtersProgram->setUniformValue("projectionMatrix", data->projectionMatrix);
    data->filtersProgram->setUniformValue("originalImageSize", 
        QVector2D(data->imageSize.width(), data->imageSize.height()));
    data->filtersProgram->setUniformValue("tex", 0);

    data->filtersProgram->setUniformValue("contrast", data->contrast);
    data->filtersProgram->setUniformValue("hsl", data->hsl);
    data->filtersProgram->setUniformValue("keystoneEnabled", false);

    data->filtersProgram->enableAttributeArray(0);

    drawCustomize();

    functions->glDrawArrays(GL_QUADS, 0, 4);
    functions->glFlush();

    data->filtersProgram->disableAttributeArray(0);
    data->filtersProgram->release();
    data->vertexBuffer->release();
    data->inputTexture->release();
}

QSharedPointer<QOpenGLShaderProgram> CameraOpenGLItem::createAndCompileShaderProgram(QString programName)
{
    QSharedPointer<QOpenGLShaderProgram> result(new QOpenGLShaderProgram);

    QOpenGLShader *vertexShader = new QOpenGLShader(QOpenGLShader::Vertex, result.data());
    readAndCompileShaderFile(vertexShader, programName + ".vert");

    QOpenGLShader *fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment, result.data());
    readAndCompileShaderFile(fragmentShader, programName + ".frag");

    result->addShader(vertexShader);
    result->addShader(fragmentShader);
    bool bSuccess = result->link();

    return bSuccess ? result : QSharedPointer<QOpenGLShaderProgram>();
}

void CameraOpenGLItem::buildVertexBuffer(QSharedPointer<ShaderData> data)
{
    static const float coords[12] = { 
        0, 0, 0 ,
        1, 0, 0,
        1, 1, 0 ,
        0, 1, 0 };

    data->vertexBuffer = QSharedPointer<QOpenGLBuffer>::create(QOpenGLBuffer::VertexBuffer);
    data->vertexBuffer->create();
    data->vertexBuffer->bind();
    data->vertexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    data->vertexBuffer->allocate(coords, 12 * sizeof(float));

    data->filtersProgram->enableAttributeArray(0);
    data->filtersProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3);
    data->vertexBuffer->release();
    data->filtersProgram->disableAttributeArray(0);    
}

QSharedPointer<QOpenGLTexture> CameraOpenGLItem::createTexture(QSize size,
    ShaderData::InterpolationType interpolationType,
    QOpenGLTexture::TextureFormat format)
{
    auto result = QSharedPointer<QOpenGLTexture>::create(QOpenGLTexture::Target2D);
    result->setSize(size.width(), size.height());
    result->setFormat(format);

    result->setMagnificationFilter(interpolationType == ShaderData::InterpolationType::Nearest ? QOpenGLTexture::Nearest : QOpenGLTexture::Linear);
    result->setMinificationFilter(interpolationType == ShaderData::InterpolationType::Nearest ? QOpenGLTexture::Nearest : QOpenGLTexture::Linear);

    if (!result->create())
        throw new std::exception("Failed to create texture");

    result->allocateStorage();

    return result;
}

void CameraOpenGLItem::readAndCompileShaderFile(QOpenGLShader* shader, QString shaderFileName)
{
    QFile shaderFile(shaderFileName);
    if (!shaderFile.open(QIODevice::ReadOnly))
    {
        //qCritical() << this << "Cannot open shader file";
        return;
    }

    if (!shader->compileSourceFile(shaderFileName))
    {
        //qCritical() << this << "Failed to compile shader" << shaderFileName << "with following error:";
        //qCritical() << this << shader->log();
        return;
    }
}

QVariant CameraOpenGLItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    return QGraphicsItem::itemChange(change, value);
}

QRectF CameraOpenGLItem::boundingRect() const
{
    return QRectF(0,0, m_viewport.width(), m_viewport.height());
}

void CameraOpenGLItem::setPoints(const std::vector<pcl::PointXYZ, Eigen::aligned_allocator<pcl::PointXYZ>>& arrPoints)
{
    m_arrPoints = arrPoints;

    m_arrVertex.resize(m_arrPoints.size() * 3);

    for (size_t i = 0; i < m_arrPoints.size(); ++i)
    {
        m_arrVertex[3 * i + 0] = m_arrPoints.at(i).x;
        m_arrVertex[3 * i + 1] = m_arrPoints.at(i).y;
        m_arrVertex[3 * i + 2] = m_arrPoints.at(i).z;
    }
}
void CameraOpenGLItem::setAllPoints(const std::vector<pcl::PointXYZ, Eigen::aligned_allocator<pcl::PointXYZ>>& cp1,
    const std::vector<pcl::PointXYZ, Eigen::aligned_allocator<pcl::PointXYZ>>& cp2)
{
    m_cp1 = cp1;
    m_cp2 = cp2;

    //m_timer_monitor = new QTimer(this);
    //m_timer_monitor->setTimerType(Qt::PreciseTimer);
    //m_timer_monitor->setInterval(500);

    //QObject::connect(m_timer_monitor, SIGNAL(timeout()), this, SLOT(onTimeoutMonitor()));

    //m_timer_monitor->start(500);
}


void CameraOpenGLItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_initPos = event->pos();
    m_inMove = true;
}

void CameraOpenGLItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    m_inMove = false;

}

void CameraOpenGLItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!m_inMove)
        return;

    int xpos = event->pos().x();
    int ypos = event->pos().y();

    auto dx = event->pos().x() - m_initPos.x();
    auto dy = event->pos().y() - m_initPos.y();

    if (m_firstMouse)
    {
        m_lastX = xpos;
        m_lastY = ypos;
        m_firstMouse = false;
    }

    float xoffset = xpos - m_lastX;
    float yoffset = m_lastY - ypos; // reversed since y-coordinates go from bottom to top

    m_lastX = xpos;
    m_lastY = ypos;
    
    if (event->buttons() & Qt::MidButton)
    {
        m_camera->ProcessMouseMovement(xoffset, yoffset);
        update();
        return;
    }
    
    if ( abs(xoffset) > 3 && abs(xoffset) > abs(yoffset))
    {
        if (xoffset > 0)
            m_camera->rotatePos(Camera_Movement::RIGHT);
        else
            m_camera->rotatePos(Camera_Movement::LEFT);
    }
    else if (abs(yoffset) > 3)
    {
        if (yoffset > 0)
            m_camera->rotatePos(Camera_Movement::FORWARD);
        else
            m_camera->rotatePos(Camera_Movement::BACKWARD);
    }

    update();
}

void CameraOpenGLItem::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    float yoffset = m_lastY - event->pos().x(); // reversed since y-coordinates range from bottom to top
    float sensitivity = 0.05f;
    yoffset *= sensitivity;

    if (event->delta() >= 0)
        yoffset = 1.001;
    else
        yoffset = -1.001;

    m_camera->ProcessMouseScroll(yoffset);

 
    update();

    //QGraphicsItem::wheelEvent(event);
}

void CameraOpenGLItem::onTimeoutMonitor()
{
    for (size_t i = 0; i < m_cp2.size(); ++i)
    {
        QVector3D vertext(m_cp2.at(i).x, m_cp2.at(i).y, m_cp2.at(i).z);
        QVector3D a(0.0, 0.0, 10.0);
        QMatrix4x4 m;
        vertext -= a;
        m.rotate(5, 0.0, 1.0, 0.0);
        vertext = m * vertext;
        vertext += a;

        m.translate(1, 1, 0);
        m.translate(-1, -1, 0);

        vertext = m * vertext;
        vertext += a;

        m_cp2.at(i).x = vertext.x();
        m_cp2.at(i).y = vertext.y();
        m_cp2.at(i).z = vertext.z();
    }


    update();
}

void CameraOpenGLItem::setTransform(QMatrix4x4 matrix)
{
    for (size_t i = 0; i < m_cp2.size(); ++i)
    {
        QVector3D vertext(m_cp2.at(i).x, m_cp2.at(i).y, m_cp2.at(i).z);

        QVector3D alignedVertext = matrix * vertext;

        m_cp2.at(i).x = alignedVertext.x();
        m_cp2.at(i).y = alignedVertext.y();
        m_cp2.at(i).z = alignedVertext.z();
    }

    update();
}


void CameraOpenGLItem::setMesh(pcl::PolygonMesh::Ptr pMesh, bool bDrawMesh)
{
    m_bDrawMesh = bDrawMesh;

    m_polygons.clear();
    pcl::PointCloud<pcl::PointXYZ> newcloud;
    pcl::fromPCLPointCloud2(pMesh->cloud, newcloud);
    Eigen::Vector4f tmp;
    for (size_t i = 0; i < pMesh->polygons.size(); i++)
    { // each triangle/polygon
        pcl::Vertices apoly_in = pMesh->polygons[i];
        SinglePoly apoly;
        apoly.nvertices_ = apoly_in.vertices.size();
        apoly.vertices_ = new float[3 * apoly_in.vertices.size()];
        apoly.colors_ = new float[4 * apoly_in.vertices.size()];

        for (size_t j = 0; j < apoly_in.vertices.size(); j++)
        { // each point
            uint32_t pt = apoly_in.vertices[j];
            tmp = newcloud.points[pt].getVector4fMap();
            // x,y,z
            apoly.vertices_[3 * j + 0] = tmp(0);
            apoly.vertices_[3 * j + 1] = tmp(1);
            apoly.vertices_[3 * j + 2] = tmp(2);

            apoly.vertices_[3 * j + 0] = newcloud.points[pt].x;                ;
            apoly.vertices_[3 * j + 1] = newcloud.points[pt].y;
            apoly.vertices_[3 * j + 2] = newcloud.points[pt].z;

            // r,g,b: input is ints 0->255, opengl wants floats 0->1
            apoly.colors_[4 * j + 0] = 1.0f; // Red
            apoly.colors_[4 * j + 1] = 0.0f; // Green
            apoly.colors_[4 * j + 2] = 0.0f; // Blue
            apoly.colors_[4 * j + 3] = 1.0;
        }
        m_polygons.push_back(apoly);
    }

    update();
}