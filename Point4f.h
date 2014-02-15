#ifndef POINT4F_H_
#define POINT4F_H_

#include <cmath>
#include <GL/glut.h>
#include <iostream>

#include <boost/functional/hash.hpp>

using std::sin;
using std::cos;

///\details Класс для инкапсуляции точки или вектора
class Point4f
{
	float  coords[4];
public:

	friend std::size_t hash_value(const Point4f & p)
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, p.x());
        boost::hash_combine(seed, p.y());
        boost::hash_combine(seed, p.z());

        return seed;
    }

	class XComp
	{
	public:
		bool operator ()(const Point4f &p1, const Point4f &p2) const { return p1.x()<p2.x(); }
	};

	class YComp
	{
	public:
		bool operator ()(const Point4f &p1, const Point4f &p2) const { return p1.y()<p2.y(); }
	};

	class ZComp
	{
	public:
		bool operator ()(const Point4f &p1, const Point4f &p2) const { return p1.z()<p2.z(); }
	};

	float &x() { return coords[0]; }
	float &y() { return coords[1]; }
	float &z() { return coords[2]; }
	float &d() { return coords[4]; }


	float x() const { return coords[0]; }
	float y() const { return coords[1]; }
	float z() const { return coords[2]; }
	float d() const { return coords[4]; }

	///\details Конструктор, задающий значения координат
	Point4f(float x_, float y_, float z_, float d_ = 1)//: x(x_), y(y_), z(z_)
	{
		coords[0]=x_;
		coords[1]=y_;
		coords[2]=z_;
		coords[3]=d_;
	}

	///\details Конструктор по умолчанию - ничего не инициализирует
	Point4f() {}

	static Point4f fromSphereCoords(GLfloat r, GLfloat phi, GLfloat psi)
	{
		return Point4f(
				r*cos(phi*M_PI/180.0)*sin(psi*M_PI/180.0),
				r*sin(phi*M_PI/180.0)*sin(psi*M_PI/180.0),
				r*cos(psi*M_PI/180.0)
				);
	}

	///\details Конструктор, инициализирующий объект из массива float
	Point4f(float c[4])//:x(c[0]), y(c[1]), z(c[2])
	{
		coords[0]=c[0];
		coords[1]=c[1];
		coords[2]=c[2];
		coords[3]=c[3];
	}

	bool operator == (const Point4f &p) const
	{
		return x()==p.x() && y()==p.y() && z() == p.z();
	}

	///\details Оператор сложения
	Point4f operator +(const Point4f &p) const
	{
		return Point4f(x() + p.x(), y()+p.y(), z()+p.z());
	}

	Point4f &operator +=(const Point4f &p)
	{
		x()+=p.x();
		y()+=p.y();
		z()+=p.z();
		return *this;
	}

	Point4f &operator -=(const Point4f &p)
	{
		x()-=p.x();
		y()-=p.y();
		z()-=p.z();
		return *this;
	}

	///\details Оператор вычитания
	Point4f operator -(const Point4f &p) const
	{
		return Point4f(x() - p.x(), y()-p.y(), z()-p.z());
	}

	///\details Унарный оператор -
	Point4f &operator -()
	{
		x()=-x();
		y()=-y();
		z()=-z();
		return *this;
	}

	///\details Умножение на число
	Point4f operator *(float f) const
	{
		return Point4f(x()*f, y()*f, z()*f);
	}

	///\details Деление на число
	Point4f operator /(float f) const
	{
		return Point4f(x()/f, y()/f, z()/f);
	}

	///\details Векторное произведение
	Point4f operator *(const Point4f &p) const
	{
		return Point4f(y()*p.z()-z()*p.y(), z()*p.x()-x()*p.z(), x()*p.y()-y()*p.x());
	}

	///\details Длина вектора
	float len() const
	{
		return sqrt(x()*x() + y()*y() + z()*z());
	}

	///\details Функция, нормализующая данный вектор
	Point4f &normalize()
	{
		float l=len();
		x()/=l; y()/=l; z()/=l;
		return *this;
	}

	///\details Функция умножения данного вектора на матрицу (матрица должна быть расположена по столбцам)
	Point4f mulByMatrix(const float m[16])
	{
		float newCoords[4]={0, 0, 0, 0};
//		float coords[4]={0, 0, 0, 1};
//		coords[0]=x();
//		coords[1]=y();
//		coords[2]=z();
		for(size_t i=0; i<16; i++)
		{
			newCoords[i%4]+=coords[i/4]*m[i];
		}
		//newCoords[3]=1;
		return Point4f(newCoords);
	}

	///\details Вызов glTranslatef()
	void Translatef() { glTranslatef(x(), y(), z()); }

	///\details Вызов glVertex3fv()
	void Vertex3f() const { glVertex3fv(coords); }
	void Vertex4f() const { glVertex4fv(coords); }

	///\details Поворот точки (x, y) на угол phi
	static void rotate2d(float &x, float &y, float phi)
	{
		phi = phi*M_PI/180.0;
		float X, Y;
		X = cos(phi)*x - sin(phi)*y;
		Y = sin(phi)*x + cos(phi)*y;
		x=X;
		y=Y;
	}

	///\details Поворот точки на угол xRot вокрук оси X,
	///\details на угол yRot вокруг оси Y,
	///\details на угол zRot вокруг оси Z
	Point4f rotate(float xRot, float yRot, float zRot) const
	{
		Point4f res(*this);
		rotate2d(res.y(), res.z(), xRot);
		rotate2d(res.x(), res.z(), -yRot);
		rotate2d(res.x(), res.y(), zRot);
		return res;
	}

	Point4f rotate(Point4f rot) const
	{
		return rotate(rot.x(), rot.y(), rot.z());
	}

	Point4f rotateInv(float xRot, float yRot, float zRot) const
	{
		Point4f res(*this);
		rotate2d(res.x(), res.y(), zRot);
		rotate2d(res.x(), res.z(), -yRot);
		rotate2d(res.y(), res.z(), xRot);
		return res;
	}

	Point4f rotateInv(Point4f rot) const
	{
		return rotateInv(rot.x(), rot.y(), rot.z());
	}

	///\details Скалярное произведение данного вектора на вектор p
	float dotProduct(Point4f p)
	{
		return x()*p.x()+y()*p.y()+z()*p.z();
	}
};

///\details Оператор вывода в std::ostream
std::ostream & operator << (std::ostream &o, const Point4f &p);

#endif /* POINT4F_H_ */
