/*
 * Texture.h
 *
 *  Created on: 28.10.2008
 *      Author: Wictor
 */

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <GL/gl.h>
#include <QImage>
#include <QGLWidget>
#include <boost/shared_ptr.hpp>

///\details ����� ��� ������ ��� ������ � ���������
class TextureException {};

///\details ����� ��� ������������ �������� OpenGL
class Texture
{
	///\details ������ ��������
	GLuint index;
	///\details ������ ������ QGLWidget, � ��������� �������� ������������� ��������
	QGLWidget *parent;
	///\details ��������� - ������������ � glTexGen()
	float factor;

	///\details ��������� ���������� ��������
	void initTexture(GLenum env_mode)
	{
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,
		(float)GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		(float)GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
		(float)GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
		(float)GL_REPEAT);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
		(float)env_mode);
	}
public:
	///\details �����������, �������� ��� ����� � ���������, ��������� � �������� ��� GL_TEXTURE_ENV_MODE
	Texture(QGLWidget *parent_, const QString &fileName, float factor_ = 1.0, GLenum env_mode = GL_MODULATE):
		parent(parent_), factor(factor_)
	{
		parent->makeCurrent();
		QImage image;
		image.load(fileName);
		index = parent->bindTexture(image);
		if(!index) throw TextureException();
		initTexture(env_mode);
	}

	///\details ������������� ������, � ������� ������ ����� ����� �������� ������ ������ QImage
	Texture(QGLWidget *parent, const QImage &image, GLenum env_mode = GL_MODULATE)
	{
		index = parent->bindTexture(image);
		if(!index) throw TextureException();
		initTexture(env_mode);
	}

	///\details ��������� ������ ��������
	void makeCurrent()
	{
		glBindTexture(GL_TEXTURE_2D, index);
	}

	///\details ���������� ���������
	float getFactor() { return factor; }

	///\details ���������� - � ��� �������� �������������
	~Texture()
	{
		parent->deleteTexture(index);
	}
};

typedef boost::shared_ptr<Texture> PTexture;

#endif /* TEXTURE_H_ */
