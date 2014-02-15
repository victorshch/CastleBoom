/*
 * TexturedObject.h
 *
 *  Created on: 29.10.2008
 *      Author: Wictor
 */

#ifndef TEXTUREDOBJECT_H_
#define TEXTUREDOBJECT_H_

#include <GL/gl.h>
#include <boost/shared_ptr.hpp>

#include "MovableObject.h"
#include "Texture.h"

///\details Класс для объекта, на который накладывается текстура
class TexturedObject: public virtual MovableObject
{
	///\details Указатель на текстуру
	PTexture texture;
	///\details Статическая переменная, указывающая, проводится ли в данный момент текстурирование
	static bool isTexturizing;
	///\details Параметры для GL_TEXTURE_GEN_S
	GLfloat s_params[4];
	///\details Параметры для GL_TEXTURE_GEN_T
	GLfloat t_params[4];
	///\details Флаг, указывающий, была ли вызвана setTexGenParams() для этого объекта
	bool texGenParamsSet;

protected:
	///\details Применение текстуры.
	///\details Эту функцию надо вызывать внутри метода paint() дочернего класса
	void applyTexture()
	{
		if(texture!=PTexture()) texture->makeCurrent();
	}

	///\details Начало наложения текстуры
	///\details Эту функцию надо вызывать внутри метода paint() дочернего класса
	void startTexturization()
	{
//		if(!isTexturizing && texture!=PTexture())
//		{
			glEnable(GL_TEXTURE_2D);
			isTexturizing=true;
//		}
	}

	///\details Окончание наложения текстуры
	///\details Эту функцию надо вызывать внутри метода paint() дочернего класса
	static void stopTexturization()
	{
//		if(isTexturizing)
//		{
			glDisable(GL_TEXTURE_2D);
			isTexturizing=false;
//		}
	}

public:
	///\details Конструктор, инициализирующий указатель на текстуру
	TexturedObject(PTexture texture_): texture(texture_), texGenParamsSet(false)
	{}

	///\details Установка параметров автоматического определения текстурных координат (glTexGen())
	void setTexGenParams(GLfloat s_p1, GLfloat s_p2, GLfloat s_p3, GLfloat s_p4,
			GLfloat t_p1, GLfloat t_p2, GLfloat t_p3, GLfloat t_p4)
	{
		if(texture==PTexture()) return;

		float factor = texture->getFactor();
		s_params[0]=s_p1*factor;
		s_params[1]=s_p2*factor;
		s_params[2]=s_p3*factor;
		s_params[3]=s_p4*factor;

		t_params[0]=t_p1*factor;
		t_params[1]=t_p2*factor;
		t_params[2]=t_p3*factor;
		t_params[3]=t_p4*factor;

		texGenParamsSet=true;
	}
	///\details Инициализация автоматического определения текстурных координат (glTexGen())
	void initTexGen()
	{
		if(texture==PTexture() || !texGenParamsSet) return;

		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

		glTexGenfv(GL_S, GL_EYE_PLANE, s_params);
		glTexGenfv(GL_T, GL_EYE_PLANE, t_params);
	}

	///\details Начало автоматического определения текстурных координат (glTexGen())
	void startTexGen()
	{
	if(texture==PTexture() || !texGenParamsSet) return;
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
	}

	///\details Окончание автоматического определения текстурных координат (glTexGen())
	void stopTexGen()
	{
		if(texture==PTexture() || !texGenParamsSet) return;
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
	}

	///\details Возвращает, были ли установлены параметры автоматической генерации текстурных координат
	bool areParamsSet() { return texGenParamsSet; }
};

#endif /* TEXTUREDOBJECT_H_ */
