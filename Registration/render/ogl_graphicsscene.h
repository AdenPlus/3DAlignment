#ifndef OGLGRAPHICSSCENE_H
#define OGLGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QWidget>
#include <QPainter>
#include <QRectF>
#include  <QGraphicsTextItem>

class OGLGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    OGLGraphicsScene(QWidget* parent = 0);
    virtual ~OGLGraphicsScene();

    Q_INVOKABLE QImage grabView();

protected:
    void drawBackground(QPainter *painter, const QRectF& rect);
private:
    QGraphicsTextItem* mTextItem;
};

Q_DECLARE_METATYPE(OGLGraphicsScene*)

#endif // OGLGRAPHICSSCENE_H
