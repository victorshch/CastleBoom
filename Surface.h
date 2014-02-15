/*
 * Surface.h
 *
 *  Created on: 02.11.2008
 *      Author: Wictor
 */

#ifndef SURFACE_H_
#define SURFACE_H_

#include <boost/shared_ptr.hpp>
#include <vector>
#include <algorithm>

#include "Point4f.h"
#include "DrawUtils.h"
#include "MovableObject.h"

#include <fstream>
using std::ofstream;
using std::endl;

///\details Значение бесконечности для построения теневых объемов
const GLfloat infinity = 20.0;
const GLfloat sensitivity = 0.01;

///\details Класс для поверхности, отбрасывающей тень
class Surface
{
	///\details Точки поверхности
	std::vector<Point4f> points;
	Point4f m_normal;

	std::vector<Point4f> projected;

	enum Coord { xCoord, yCoord, zCoord };

	Coord deletedCoord;

	GLfloat d;
public:

	///\details Число углов поверхности
	const int numPoints;

	Coord smallestRange;

	void calcParams()
	{
		m_normal = (points[2]-points[1])*(points[3]-points[1]);
		m_normal.normalize();
		d = m_normal.dotProduct(points[1]);
	}

	///\details Конструктор, задающий углы поверхности
	///\details Порядок должен быть против часовой стрелки,
	///\details если смотреть со стороны наблюдателя
	Surface(Point4f p1_,
			Point4f p2_,
			Point4f p3_,
			Point4f p4_)
	:points(4), numPoints(4)
	{
		points[0]=p1_;
		points[1]=p2_;
		points[2]=p3_;
		points[3]=p4_;
		calcParams();
		project();
	}

	Surface(size_t n = 4, bool computeProjection = true): points(n), numPoints(n), deletedCoord(zCoord)
	{
		if(computeProjection) project();
	}

	///\details Умножение каждой точки поверхности на матрицу, заданную по столбцам
	void transform(GLfloat m[16])
	{
		for(int i=0; i<numPoints; i++)
		{
			points[i] = points[i].mulByMatrix(m);
		}
	}

	///\details Перенос поверхности на вектор p
	void translate(Point4f p)
	{
		for(int i=0; i<numPoints; i++)
		{
			points[i] = points[i] + p;
		}
	}

	///\details Рисование поверхности
	///\details Эта функция написана для случая четырехугольника
	void render()
	{
		glBegin(GL_TRIANGLE_FAN);
		for(int i=0; i<=numPoints-1; i++)
		{
			points[i].Vertex3f();
		}
		glEnd();
	}

	///\details Рисование поверхности "наоборот"
	void renderInversely()
	{
		glBegin(GL_TRIANGLE_FAN);

		for(int i=numPoints-1; i>=0; i--)
		{
			points[i].Vertex3f();
		}

		glEnd();
	}

	void invert()
	{
		for(int i=0; i<=(numPoints/2)-1; i++)
		{
			std::swap(points[i], points[numPoints-i-1]);
		}
	}

	///\details Возвращает нормаль к поверхности
	Point4f normal() const
	{
		return m_normal;
	}

	///\details Рисование теневого объема поверхности
	void renderShadowVolume(Point4f lightPos)
	{
		Surface extruded(numPoints);
		for(int i=0; i<numPoints; i++)
		{
			Point4f v = points[i] - lightPos;
			v.normalize();
			v = v * infinity;
			extruded[i] = v + points[i];
		}

		render();
		extruded.renderInversely();

		glBegin(GL_QUAD_STRIP);

		points[0].Vertex3f();
		extruded[0].Vertex3f();

		for(int i=1; i<=numPoints; i++)
		{
			points[i%numPoints].Vertex3f();
			extruded[i%numPoints].Vertex3f();
		}

		glEnd();
	}

	///\details Оператор для обращения к точке поверхности
	Point4f &operator [](int i) { return points[i]; }

	GLfloat distance(Point4f p) const
	{
		return std::abs(p.dotProduct(normal()) + d);
	}

	bool testInPlane(Point4f p) const
	{
		return distance(p)<sensitivity;
	}

	bool testInPolygon(Point4f p) const
	{


		if(!testInPlane(p)) return false;

		project(p);

		int i, j;
		bool c = false;
		for (i = 0, j = numPoints-1; i < numPoints; j = i++)
		{
		if ( ((projected[i].y()>p.y()) != (projected[j].y()>p.y())) &&
		 (p.x() < (projected[j].x()-projected[i].x()) * (p.y()-projected[i].y()) /
				 (projected[j].y()-projected[i].y()) + projected[i].x()) )
		   c = !c;
		}

		return c;
	}

	GLfloat xRange() const
	{
		return std::max_element(points.begin(), points.end(), Point4f::XComp())->x()-
		std::min_element(points.begin(), points.end(), Point4f::XComp())->x();
	}

	GLfloat yRange() const
	{
		return std::max_element(points.begin(), points.end(), Point4f::YComp())->y()-
		std::min_element(points.begin(), points.end(), Point4f::YComp())->y();
	}

	GLfloat zRange() const
	{
		return std::max_element(points.begin(), points.end(), Point4f::ZComp())->z()-
		std::min_element(points.begin(), points.end(), Point4f::ZComp())->z();
	}

	// does the most optimal projection
	void project()
	{
		GLfloat xR = xRange();
		GLfloat yR = yRange();
		GLfloat zR = zRange();
		GLfloat minR = std::min(xR, std::min(yR, zR));

		projected.resize(numPoints);

		if(minR==zR)
		{
			for(int i=0; i<numPoints; i++)
			{
				projected[i].x()=points[i].x();
				projected[i].y()=points[i].y();
			}
			deletedCoord = zCoord;
			return;
		}
		if(minR==yR)
		{
			for(int i=0; i<numPoints; i++)
			{
				projected[i].x()=points[i].x();
				projected[i].y()=points[i].z();
			}
			deletedCoord = yCoord;
			return;
		}
		if(minR==xR)
		{
			for(int i=0; i<numPoints; i++)
			{
				projected[i].x()=points[i].y();
				projected[i].y()=points[i].z();
			}
			deletedCoord = xCoord;
			return;
		}
	}

	void project(Point4f &p) const
	{
		switch(deletedCoord)
		{
		case xCoord: p.x()=p.y(); p.y()=p.z(); return;
		case yCoord: p.y()=p.z(); return;
		default:return;
		}
	}
};

///\details Тип указателя на поверхность
typedef boost::shared_ptr<Surface> PSurface;

#endif /* SURFACE_H_ */
