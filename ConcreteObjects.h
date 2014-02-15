/*
 * ConcreteObjects.h
 *
 *  Created on: 29.10.2008
 *      Author: Wictor
 */

#ifndef CONCRETEOBJECTS_H_
#define CONCRETEOBJECTS_H_

#include "MovableObject.h"
#include "ComplexObject.h"
#include "BlendedObject.h"
#include "MaterialObject.h"
#include "TexturedObject.h"
#include "ShadowedObject.h"
#include "Light.h"
#include "DrawUtils.h"

#define _STDCALL_SUPPORTED
#define _M_IX86
#include <GL/glut.h>

///\details Функция сигнум
inline float sgn(float f)
{
	return f>0 ? 1 : (f<0 ? -1 : 0);
}

///\details Число разбиений по первой оси по умолчанию
const int defNumSlices1=15;
///\details Число разбиений по первой оси по умолчанию
const int defNumSlices2=15;

///\details Класс для четырехугольника с разбиением и текстурой, нанесенной методом glTexGen()
class TiledTexRectangle: public MaterialObject, public TexturedObject, public ShadowedObject
{
	///\details Точки четырехугольника
	Point4f p1, p2, p3, p4;

	///\details Флаг того, что данный четырехугольник - параллелограмм
	bool parallelogramm;

	///\details Число разбиений по первой и по второй оси
	int numSlices1, numSlices2;

public:

	///\details Конструктор, задающий материал, текстуру, относительные координаты вершин,
	///\details является ли прямоугольник параллелограммом, отбрасывает ли тень,
	///\details а также число разбиений по первой и второй оси.
	///\details Точки должны задаваться с учетом порядка - против часовой стрелки от наблюдателя
	TiledTexRectangle(PMaterial m, PTexture t,
			Point4f p1_, Point4f p2_, Point4f p3_, Point4f p4_,
			bool parallelogramm_ = true,
			bool castsShadow_ = false,
			int numSlices1_ = defNumSlices1, int numSlices2_ = defNumSlices2
			):
		MaterialObject(m), TexturedObject(t),
		p1(p1_), p2(p2_), p3(p3_), p4(p4_), parallelogramm(parallelogramm_),
		numSlices1(numSlices1_), numSlices2(numSlices2_)
	{
		if(castsShadow_)
		{
			addSurface(
					new Surface(p1, p2, p3, p4)
					);
		}
	}

	///\details Рисование прямоугольника с учетом положения, материала, текстуры,
	///\details и того, является ли он параллелограммом (в этом случае разбиение будет равномернее)
	void paint()
	{
		glPushMatrix();
		applyTransformations();
		applyMaterial();
		applyTexture();
		if(!areParamsSet()) setTexGenParams(
				sgn(p2.x()-p1.x()), sgn(p2.y()-p1.y()), sgn(p2.z()-p1.z()), 0,
				sgn(p4.x()-p1.x()), sgn(p4.y()-p1.y()), sgn(p4.z()-p1.z()), 0);
		initTexGen();
		startTexGen();
		startTexturization();

		if(parallelogramm)
		drawQuad2(p1, p2, p4, numSlices1, numSlices2);
		else drawQuad(p1, p2, p3, p4, numSlices1, numSlices2);

		stopTexturization();
		stopTexGen();
		glPopMatrix();
	}
};

///\details Класс для параллелограмма с растянутой на нем текстурой
class TexRectangle: public MaterialObject, public TexturedObject
{
	///\details Длина, ширина
	GLfloat width, height;

	///\details Число разбиений
	int numSlices1, numSlices2;
public:

	///\details Конструктор, задающий материал, текстуру, длину, ширину,
	///\details отбрасывает ли объект тень,
	///\details а также число разбиений по первой и второй оси
	TexRectangle(PMaterial m, PTexture t, GLfloat width_, GLfloat height_,
			int numSlices1_ = defNumSlices1, int numSlices2_ = defNumSlices2):
		MaterialObject(m), TexturedObject(t),
		width(width_), height(height_),
		numSlices1(numSlices1_), numSlices2(numSlices2_)
	{}

	///\details Рисование объекта с учетом положения, материала и текстуры
	void paint()
	{
		Point4f p1(0, 0, 0);
		Point4f p2(0, 0, width);
		Point4f p3(0, height, 0);

		glPushMatrix();
		applyTransformations();
		applyMaterial();
		applyTexture();

		startTexturization();

		drawQuad2(p1, p2, p3, defNumSlices1, defNumSlices2, true);

		stopTexturization();
		stopTexGen();
		glPopMatrix();
	}
};

///\details Класс для треугольника, обладающего материалом (но не текстурой)
class MatTriangle: public MaterialObject
{
	///\details Координаты вершины
	Point4f p1;

	///\details Вектор, задающий вторую вершину
	Point4f v1;

	///\details Вектор, задающий третью вершину
	Point4f v2;
public:
	///\details Конструктор, в котором задается материал и относительное положение вершин
	MatTriangle(PMaterial m, Point4f p1_, Point4f v1_, Point4f v2_):
		MaterialObject(m),
		p1(p1_), v1(v1_), v2(v2_)
	{}

	///\details Рисование объекта с учетом материала и положения
	void paint()
	{

		glPushMatrix();

		applyTransformations();
		applyMaterial();

		drawTriangle(p1, p1 + v1, p1 + v2, defNumSlices1, defNumSlices2);

		glPopMatrix();
	}
};

///\details Класс для полупрозрачного параллелограмма
class BlendedSurface : public BlendedObject
{
public:

	///\details Конструктор, задающий материал, текстуру и относительное положение вершин
	BlendedSurface(PMaterial m, PTexture t, Point4f p1, Point4f p2, Point4f p3, Point4f p4)
	{
		addObject(new TiledTexRectangle(m, t, p1, p2, p3, p4));
	}
};

///\details Класс для шестигранника, имеющего текстуру, наложенную с помощью glTexGen()
class TiledTexHexahedron: public ComplexObject
{
public:

	///\details Конструктор, задающий материал, текстуру,
	///\details относительные координаты вершин (кроме нулевой вершины)
	///\details и то, отбрасывает ли объект тень
	///\details Координаты вершин должны задаваться так:
	///\details сначала три вершины нижней грани против часовой стрелки,
	///\details затем четыре вершины верхней грани -
	///\details сначала вершина, соответствующая нулевой в нижней грани,
	///\details затем три вершины, соответствующие остальным трем в нижней грани
	TiledTexHexahedron(PMaterial material, PTexture texture,
						Point4f p2, Point4f p3, Point4f p4,
			Point4f p5, Point4f p6, Point4f p7, Point4f p8,
			bool castsShadow = false
			)
	{
		Point4f p1(0, 0, 0);

		//bottom
		addObject(
				new TiledTexRectangle(material, texture, p1, p2, p3, p4, false, castsShadow)
				);

		//top
		addObject(
				new TiledTexRectangle(material, texture, p8, p7, p6, p5, false, castsShadow)
				);

		//sides
		addObject(
				new TiledTexRectangle(material, texture, p1, p4, p8, p5, false, castsShadow)
				);
		addObject(
				new TiledTexRectangle(material, texture, p1, p5, p6, p2, false, castsShadow)
				);
		addObject(
				new TiledTexRectangle(material, texture, p2, p6, p7, p3, false, castsShadow)
				);
		addObject(
				new TiledTexRectangle(material, texture, p3, p7, p8, p4, false, castsShadow)
				);
	}
};

///\details Класс для параллелепипеда, имеющего текстуру, наложенную с помощью glTexGen()
class TiledTexBox : public TiledTexHexahedron
{
public:
	///\details Конструктор, задающий материал, текстуру, длину, ширину, высоту и отбрасывает
	///\details ли объект тень
	TiledTexBox(PMaterial material, PTexture texture,
			GLfloat w, GLfloat h, GLfloat d, bool castsShadow = false):
				TiledTexHexahedron(
						material,
						texture,
						Point4f(w, 0, 0),
						Point4f(w, 0, d),
						Point4f(0, 0, d),
						Point4f(0, h, 0),
						Point4f(w, h, 0),
						Point4f(w, h, d),
						Point4f(0, h, d),
						castsShadow
						)
	{}
};

class Walls: public ComplexObject
{
public:
	Walls(PMaterial M, PTexture T,
			GLfloat areaLength, GLfloat areaWidth,
			GLfloat wallWidth, GLfloat wallHeight, bool castsShadow = true)
	{
		TiledTexBox *p1 = new TiledTexBox(M, T,
				areaLength + 2*wallWidth,
				wallHeight,
				wallWidth,
				castsShadow);

		TiledTexBox *p2 = new TiledTexBox(M, T,
				areaLength + 2*wallWidth,
				wallHeight,
				wallWidth,
				castsShadow);

		p2->setPos(0, 0, areaWidth + wallWidth);

		TiledTexBox *p3 = new TiledTexBox(M, T,
				wallWidth,
				wallHeight,
				areaWidth + 2*wallWidth,
				castsShadow);

		TiledTexBox *p4 = new TiledTexBox(M, T,
				wallWidth,
				wallHeight,
				areaWidth + 2*wallWidth,
				castsShadow);

		p4->setPos(areaLength + wallWidth, 0, 0);

		addObject(p1);
		addObject(p2);
		addObject(p3);
		addObject(p4);
	}
};

///\details Класс для сферы, не имеющей текстуры
class MatSphere : public MaterialObject
{
	///\details Радиус сферы
	GLfloat radius;
public:
	///\details Конструктор, задающий материал и радиус
	MatSphere(PMaterial m, GLfloat r):
		MaterialObject(m), radius(r)
	{}

	///\details Рисование сферы с учетом материала и положения
	void paint()
	{
		glPushMatrix();
		applyMaterial();
		applyTransformations();

		glutSolidSphere (radius, defNumSlices1, defNumSlices2);

		glPopMatrix();
	}
};

class Cylinder: public MaterialObject, public TexturedObject, public ShadowedObject
{

	GLUquadricObj *qobj;

	GLfloat radius1, radius2, height;

	int numSlices1, numSlices2;
public:

	Cylinder(PMaterial m, PTexture t, GLfloat r1, GLfloat r2, GLfloat h,
			bool castsShadow_ = true,
			int numSlices1_ = defNumSlices1, int numSlices2_ = defNumSlices2
			):
		MaterialObject(m), TexturedObject(t), radius1(r1), radius2(r2), height(h),
		numSlices1(numSlices1_), numSlices2(numSlices2_)
	{
		qobj = gluNewQuadric();
		if(castsShadow_)
		{
//			addHexahedronSurface(
//					Point4f(-r1, 0 ,-r1),
//					Point4f(r1, 0 ,-r1),
//					Point4f(r1, 0 ,r1),
//					Point4f(-r1, 0 ,r1),
//					Point4f(-r2, h ,-r2),
//					Point4f(r2, h ,-r2),
//					Point4f(r2, h ,r2),
//					Point4f(-r2, h ,r2)
//					);

			addCylinderSurface(
					Point4f(0, 0, 0),
					r1, r2, h, numSlices1
					);
		}

		if(t!=PTexture())
		{
			gluQuadricTexture(qobj, GL_TRUE);
		}
	}

	void setHeight(GLfloat h)
	{
		height=h;
	}

	GLfloat getHeight()
	{
		return height;
	}

	///\details Рисование цилиндра с учетом метериала и положения.
	///\details Разворачивает цилиндр так, чтобы его ось была параллельна оси y
	void paint()
	{
		if(!visible) return;
		glPushMatrix();
		applyTransformations();
		applyMaterial();
		applyTexture();

		startTexturization();

		glRotatef(270, 1, 0, 0);
		gluCylinder(qobj, radius1, radius2, height, numSlices1, numSlices2);

		stopTexturization();

		glPopMatrix();
	}
};

class Disc: public MaterialObject, public ShadowedObject
{
	GLUquadricObj *qobj;
	GLfloat radius;
	int numSlices1, numSlices2;
public:
	Disc(PMaterial m, GLfloat r,
			bool castsShadow_ = false,
			int numSlices1_ = defNumSlices1, int numSlices2_ = defNumSlices2
			):
		MaterialObject(m), radius(r),
		numSlices1(numSlices1_), numSlices2(numSlices2_)
	{
		qobj = gluNewQuadric();
		if(castsShadow_)
		{
			addSurface(
					Point4f(-r, 0 ,-r),
					Point4f(r, 0 ,-r),
					Point4f(r, 0 ,r),
					Point4f(-r, 0 ,r)
					);

			addSurface(
					Point4f(-r, 0 ,r),
					Point4f(r, 0 ,r),
					Point4f(r, 0 ,-r),
					Point4f(-r, 0 ,-r)
					);
		}
	}

	void paint()
	{
		if(!visible) return;
		glPushMatrix();
		applyTransformations();
		applyMaterial();

		glRotatef(270, 1, 0, 0);
		gluDisk(qobj, 0, radius, numSlices1, numSlices2);

		glPopMatrix();
	}
};

class Sphere: public MaterialObject, public TexturedObject, public ShadowedObject
{
	GLUquadricObj *qobj;
	GLfloat radius;
	int numSlices1, numSlices2;
public:
	Sphere(PMaterial m, PTexture t,
			GLfloat r,
			bool castsShadow_ = true,
			int numSlices1_ = defNumSlices1, int numSlices2_ = defNumSlices2
			):
		MaterialObject(m), TexturedObject(t), radius(r),
		numSlices1(numSlices1_), numSlices2(numSlices2_)
	{
		qobj = gluNewQuadric();
		if(castsShadow_)
		{
			addSphereSurface(
					r,
					numSlices1/2,
					numSlices2/2
					);
		}

		if(t!=PTexture())
		{
			gluQuadricTexture(qobj, GL_TRUE);
		}
	}

	void paint()
	{
		if(!visible) return;
		glPushMatrix();
		applyTransformations();
		applyMaterial();

		applyTexture();

		startTexturization();


		//glRotatef(270, 1, 0, 0);
		gluSphere(qobj, radius, numSlices1, numSlices2);

		stopTexturization();

		glPopMatrix();
	}
};


class Tower: public ComplexObject
{
	Cylinder *c, *head;
	Disc *d;
public:
	Tower(PMaterial towerM, PTexture towerT, PMaterial roofM, PTexture roofT, PMaterial middleM,
			GLfloat towerR, GLfloat towerH, GLfloat roofR, GLfloat roofH,
			bool castsShadow=true,
			int numSlices1 = defNumSlices1, int numSlices2 = defNumSlices2
			)
	{
		addObject(
				c=new Cylinder(towerM, towerT, towerR, towerR, towerH, castsShadow, 2*numSlices1, 2*numSlices2)
				);

		addObject(
				(head=new Cylinder(roofM, roofT, roofR, 0, roofH, castsShadow, 2*numSlices1, 2*numSlices2))
				->setPos(0, towerH, 0)
				);

		addObject(
				(d=new Disc(middleM, roofR, false, numSlices1, numSlices2))
				->setPos(0, towerH, 0)
				->setXRotation(180)
				);
	}
	void setHeight(GLfloat h)
	{
		c->setHeight(h);
		head->setPos(0, h, 0);
		d->setPos(0, h, 0);
	}

	GLfloat getHeight()
	{
		return c->getHeight();
	}
};

class Castle: public ComplexObject
{
public:
	Castle(
			PMaterial wallM, PTexture wallT,
			PMaterial towerM, PTexture towerT,
			PMaterial roofM, PTexture roofT,
			PMaterial middleM,
			GLfloat castleL, GLfloat castleW,
			GLfloat wallW, GLfloat wallH,
			GLfloat towerR, GLfloat towerH,
			GLfloat roofR, GLfloat roofH
			)
	{
		addObject(
				new Walls(wallM, wallT, castleL, castleW, wallW, wallH)
				);

		Tower *t = new Tower(towerM, towerT, roofM, roofT, middleM,
				towerR, towerH, roofR, roofH
				);

		Tower *t2 = new Tower(towerM, towerT, roofM, roofT, middleM,
				towerR, towerH, roofR, roofH
				);

		Tower *t3 = new Tower(towerM, towerT, roofM, roofT, middleM,
				towerR, towerH, roofR, roofH
				);

		Tower *t4 = new Tower(towerM, towerT, roofM, roofT, middleM,
				towerR, towerH, roofR, roofH
				);

		addObject(
				(t2)->setPos(wallW/2, 0, wallW/2)
				);

		addObject(
				(t3)->setPos(castleL + 3.0*wallW/2, 0, wallW/2)
				);

		addObject(
				(t4)->setPos(castleL + 3.0*wallW/2, 0, castleW+3*wallW/2)
				);

		addObject(
				t->setPos(wallW/2, 0, castleW+3*wallW/2)
				);

	}
};

class Arrow: public BlendedObject
{
	Tower *t;
public:
	Arrow(PMaterial m, GLfloat len, GLfloat len2, GLfloat r1, GLfloat r2,
			Point4f initialRotation, int numSlices1 = defNumSlices1, int numSlices2 = defNumSlices2)
	{
		addObject(
			(t=new Tower(m, PTexture(), m, PTexture(), m, r1, len, r2, len2, false, numSlices1, numSlices2))
			->setRot(initialRotation)
				);
	}

	//MovableObject *getArrow() { return t; }

	void setRelRotation(Point4f r)
	{
		t->setRot(r);
	}

	void changeRelRotation(Point4f dr)
	{
		t->changeRot(dr);
	}

	Point4f getAbsRotation()
	{
		return getRot() + t->getRot();
	}

	Point4f getRelRotation()
	{
		return t->getRot();
	}

	void setLen(GLfloat l)
	{
		t->setHeight(l);
	}

	GLfloat getLen()
	{
		return t->getHeight();
	}

	void changeLen(GLfloat dL)
	{
		t->setHeight(t->getHeight()+dL);
	}
};

#endif /* CONCRETEOBJECTS_H_ */
