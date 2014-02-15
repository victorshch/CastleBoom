/*
 * ShadowedObject.h
 *
 *  Created on: 02.11.2008
 *      Author: Wictor
 */

#ifndef SHADOWEDOBJECT_H_
#define SHADOWEDOBJECT_H_

#include <list>
#include <utility>

#include "MovableObject.h"
#include "Surface.h"

const GLfloat eps = 0.01;

///\details Класс для объекта, который отбрасывает тень
class ShadowedObject: public virtual MovableObject
{
	///\details Флаг того, что объект действительно может отбрасывать тень
	bool enabled;

	///\details Список отбрасывающих тень поверхностей, которыми обладаен данный объект
	std::list<PSurface> list;
public:
	///\details Конструктор умолчания
	ShadowedObject(): enabled(false)
	{
	}

	///\details Функция, добавляющая отбрабывающую тень поверхность объекту
	///\details Ее нужно вызывать при определении объекта
	///\details (например, в конструкторе дочернего класса)
	void addSurface(Surface *s)
	{
		if(!enabled) enabled=true;
		list.push_back(PSurface(s));
	}

	///\details Функция, аналогичная предыдущей функции, перегруженная для удобства
	void addSurface(
			Point4f p1, Point4f p2, Point4f p3, Point4f p4)
	{
		addSurface(
				new Surface(p1, p2, p3, p4)
				);
	}

	///\details Функция, добавляющая шестигранную поверхность, отбрасывающую тень
	void addHexahedronSurface(
			Point4f p1,
			Point4f p2,
			Point4f p3,
			Point4f p4,
			Point4f p5,
			Point4f p6,
			Point4f p7,
			Point4f p8
			)
	{
		//bottom
		addSurface(p1, p2, p3, p4);

		//sides
		addSurface(p1, p4, p8, p5);
		addSurface(p4, p3, p7, p8);
		addSurface(p3, p2, p6, p7);
		addSurface(p2, p1, p5, p6);

		//top
		addSurface(p8, p7, p6, p5);
	}

	void addCylinderSurface(Point4f p, GLfloat r1, GLfloat r2, GLfloat h, size_t numSlices)
	{
		Surface bottom(numSlices), top(numSlices);
		Point4f v(0, 0, 0);

		r1-=eps;
		r2-=eps;

		GLfloat phi=0, step=-400.0/(numSlices-1);
		for(size_t i=0; i<numSlices; i++)
		{
			v.x()=cos((phi*M_PI)/180.0)*r1;
			v.z()=sin((phi*M_PI)/180.0)*r1;
//			v.x()=cos(phi)*r1;
//			v.z()=sin(phi)*r1;
			bottom[i]=p+v;
			phi+=step;
		}

		phi=0;
		v.y()=h;
		for(size_t i=0; i<numSlices; i++)
		{
			v.x()=cos((phi*M_PI)/180.0)*r2;
			v.z()=sin((phi*M_PI)/180.0)*r2;
//			v.x()=cos(phi)*r2;
//			v.z()=sin(phi)*r2;
			top[i]=p+v;
			phi+=step;
		}

		for(size_t i=0; i<numSlices; i++)
		{
			addSurface(

					top[i],
					bottom[i],
					bottom[(i+1)%(numSlices)],
					top[(i+1)%(numSlices)]

					);
		}
	}

	void addSphereSurface(GLfloat r, GLint numSlices1, GLint numSlices2)
	{
		r-=eps/2.0;

		GLfloat phi=0, psi;
		GLfloat phiStep=360.0/(numSlices1), psiStep=180.0/(numSlices2);
		for(int i=0; i<2*numSlices1; i++)
		{
			psi=0;
			for(int j=0; j<2*numSlices2; j++)
			{
				addSurface(

						Point4f::fromSphereCoords(r, phi+phiStep, psi),
						Point4f::fromSphereCoords(r, phi, psi),
						Point4f::fromSphereCoords(r, phi, psi+psiStep),
						Point4f::fromSphereCoords(r, phi+phiStep, psi+psiStep)

						);
				psi+=psiStep;
			}
			phi+=phiStep;
		}
	}

	///\details Функция построения теневых объемов. lightPos должна быть абсолютным положением источника света
	void buildShadowVolumes(Point4f lightPos)
	{
		if(!visible) return;
		// Переводим координаты источника света в локальные для нашего объекта
		lightPos = lightPos - getPos();
		lightPos = lightPos.rotate(-getXRotation(), -getYRotation(), -getZRotation());

		glPushMatrix();

		applyTransformations();
		for(std::list<PSurface>::iterator it = list.begin(); it!=list.end(); it++)
		{
			float d = lightPos.dotProduct((**it).normal());
			// рисуем только те поверхности, которые смотрят на источник света
			if(d>0) (*it)->renderShadowVolume(lightPos);
		}

		glPopMatrix();
	}

	bool test(Point4f p) const
	{
		p=p-getPos();
		//p=p.rotate(-getXRotation(), -getYRotation(), -getZRotation());
		for(std::list<PSurface>::const_iterator it = list.begin(); it!=list.end(); it++)
		{
				if((*it)->testInPolygon(p)) return true;
		}
		return false;
	}
};

#endif /* SHADOWEDOBJECT_H_ */
