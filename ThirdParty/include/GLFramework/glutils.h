#ifndef GLUTILS_H
#define GLUTILS_H

#include <QRect>
#include <QColor>
#include <QVector2D>
#include <QVector3D>
#include "GLFramework/engine/TextureManager.h"


class QOpenGLTexture;

class GLUtils
{
public:
	GLUtils();
	virtual ~GLUtils();

	void drawRect(const QRect& selRect, float frameWidth, QColor color = QColor(0, 0, 255, 255)) const;

	void orthogonalStart(int w, int h);

	void orthogonalEnd(void);

	void drawQuad(A3D::Texture::Ref texture, float x, float y, float w, float h) const;
	void drawQuad(A3D::Texture::Ref texture, float x, float y, float w, float h, const QVector2D& uv0, const QVector2D& uv1) const;
	void drawQuad(const QRect& rect, const QColor& color) const;

	void glCheckError() const;
};



#endif // GLUTILS_H
