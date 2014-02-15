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

///\details Класс для ошибки при работе с текстурой
class TextureException {};

///\details Класс для инкапсуляции текстуры OpenGL
class Texture
{
	///\details Индекс текстуры
	GLuint index;
	///\details Объект класса QGLWidget, к контексту которого привязывается текстура
	QGLWidget *parent;
	///\details Множитель - используется в glTexGen()
	float factor;

	///\details Установка параметров текстуры
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
	///\details Конструктор, задающий имя файла с текстурой, множитель и параметр для GL_TEXTURE_ENV_MODE
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

	///\details Перегруженная версия, в которой вместо имени файла задается объект класса QImage
	Texture(QGLWidget *parent, const QImage &image, GLenum env_mode = GL_MODULATE)
	{
		index = parent->bindTexture(image);
		if(!index) throw TextureException();
		initTexture(env_mode);
	}

	///\details Включение данной текстуры
	void makeCurrent()
	{
		glBindTexture(GL_TEXTURE_2D, index);
	}

	///\details Возвращает множитель
	float getFactor() { return factor; }

	///\details Деструктор - в нем текстура освобождается
	~Texture()
	{
		parent->deleteTexture(index);
	}
};

typedef boost::shared_ptr<Texture> PTexture;

#endif /* TEXTURE_H_ */
