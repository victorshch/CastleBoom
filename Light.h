/*
 * Light.h
 *
 *  Created on: 28.10.2008
 *      Author: Wictor
 */

#ifndef LIGHT_H_
#define LIGHT_H_

#include <boost/shared_ptr.hpp>

#include "MovableObject.h"

///\details Класс, инкапсулирующий источник света
///\details и позволяющий обращаться с ним так же, как с другими объектами
struct Light: public MovableObject
{
	///\details Параметры для GL_AMBIENT
	GLfloat light_dif[4];
	///\details Параметры для GL_DIFFUSE
	GLfloat light_amb[4];
	///\details Параметры для GL_SPECULAR
	GLfloat light_spec[4];
	///\details Параметры для GL_POSITION
	GLfloat light_pos[4];
	///\details Константа из (GL_LIGHT0, ...) , соответствующая данному источнику света
	GLenum light;
	///\details Параметр для GL_SPOT_CUTOFF
	int cutoff;
	///\details Флаг того, что это точечный источник (а не бесконечно удаленный)
	bool pointed;

	///\details Конструктор, в котором задаются параметры
	Light(
		GLenum light_,
		GLfloat light_amb1,
		GLfloat light_amb2,
		GLfloat light_amb3,
		GLfloat light_amb4,
		GLfloat light_dif1,
		GLfloat light_dif2,
		GLfloat light_dif3,
		GLfloat light_dif4,
		GLfloat light_spec1,
		GLfloat light_spec2,
		GLfloat light_spec3,
		GLfloat light_spec4,
		bool pointed_ = true,
		int cutoff_ = 180
		): light(light_), cutoff(cutoff_), pointed(pointed_)
	{
		light_dif[0]=(light_dif1),
		light_dif[1]=(light_dif2),
		light_dif[2]=(light_dif3),
		light_dif[3]=(light_dif4),
		light_amb[0]=(light_amb1),
		light_amb[1]=(light_amb2),
		light_amb[2]=(light_amb3),
		light_amb[3]=(light_amb4),
		light_spec[0]=(light_spec1),
		light_spec[1]=(light_spec2),
		light_spec[2]=(light_spec3);
		light_spec[3]=(light_spec4);

		light_pos[0]=0;
		light_pos[1]=0;
		light_pos[2]=1;

		if(pointed)
		{
			light_pos[3]=1;
		}
		else
		{
			light_pos[3]=0;
		}

		glEnable(light);
	}

	///\details Вывод источника света (с учетом положения, заданного в MovableObject)
	void paint()
	{

		glPushMatrix();

		// apply rotation and translation
		applyTransformations();

		glLightfv (light, GL_AMBIENT, light_amb);
		glLightfv (light, GL_DIFFUSE, light_dif);
		glLightfv (light, GL_SPECULAR, light_spec);
		glLightfv (light, GL_POSITION, light_pos);

		if(pointed)
		{
			glLightfv (light, GL_SPOT_DIRECTION, light_pos);
			glLighti (light, GL_SPOT_CUTOFF, cutoff);
		}

		glPopMatrix();
	}

	///\details Включение данного источника света
	void turnOn()
	{
		glEnable(light);
	}
	///\details Выключение данного источника света
	void turnOff()
	{
		glDisable(light);
	}
};

typedef boost::shared_ptr<Light> PLight;

#endif /* LIGHT_H_ */
