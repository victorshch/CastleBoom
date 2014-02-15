/*
 * BlendedObject.h
 *
 *  Created on: 03.11.2008
 *      Author: Wictor
 */

#ifndef BLENDEDOBJECT_H_
#define BLENDEDOBJECT_H_

#include "ComplexObject.h"

///\details Класс - обертка для объекта, который нужно рисовать с использованием GL_BLEND
class BlendedObject: public ComplexObject
{
public:
	///\details Конструктор умолчания
	BlendedObject()
	{}

	///\details Функция для начала блендинга
	void applyBlending()
	{
		glEnable(GL_BLEND);
	}

	///\details Функция для завершения блендинга
	void stopBlending()
	{
		glDisable(GL_BLEND);
	}

	///\details Рисование объекта с учетом блендинга
	void paint()
	{
		applyBlending();

		glCullFace(GL_FRONT);
		ComplexObject::paint();

		glCullFace(GL_BACK);
		ComplexObject::paint();

		stopBlending();
	}
};

#endif /* BLENDEDOBJECT_H_ */
