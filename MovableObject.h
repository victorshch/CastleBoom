/*
 * MovableObject.h
 *
 *  Created on: 27.10.2008
 *      Author: Wictor
 */

#ifndef MOVABLEOBJECT_H_
#define MOVABLEOBJECT_H_

#include <boost/shared_ptr.hpp>

#include "Point4f.h"
#include <string>
#include <iostream>

#include <fstream>

///\details Абстрактный класс объекта, имеющего положение в пространстве
class MovableObject
{
	friend std::ostream & operator << (std::ostream &o, const MovableObject &m);

	///\details Имя объекта
	///\details Может быть использовано для сохранения объекта в файл
	std::string name;

	static void normalizeAngle(GLfloat &f)
	{
		while(f>=360) f-=360;
		while(f<0) f+=360;
	}
protected:
	///\details Положение объекта
	Point4f pos;

	///\details Абсолютное положение объекта - его можно задавать дополнительно,
	///\details оно используется, например, при построении теней
	Point4f absPos;

	///\details Поворот вокруг оси Y
	float rotation;

	///\details Поворот вокруг оси X
	float h_rotation;

	///\details Поворот вокруг оси Z
	float z_rotation;

	bool visible;
public:
	///\details Конструктор, в котором можно задать положение
	MovableObject(const Point4f & initPos = Point4f(0, 0, 0) ):
		pos(initPos), rotation(0), h_rotation(0), z_rotation(0), name(""), visible(true)
	{}

	///\details Применение переноса и поворота
	///\details Нужно вызывать внутри метода paint() объекта дочернего класса,
	///\details скорее всего нужно вызывать внутри glPushMatrix() ... glPopMatrix()
	virtual void applyTransformations()
	{
		(pos).Translatef();
		glRotatef(z_rotation, 0, 0, 1);
		glRotatef(rotation, 0, 1, 0);
		glRotatef(h_rotation, 1, 0, 0);

	}

	virtual MovableObject * setVisible(bool b)
	{
		visible=b;
		return this;
	}

	bool getVisible() { return visible; }

	///\details Устанавливает значение pos
	MovableObject *setPos(const Point4f &newPos)
	{
		pos=newPos;
		return this;
	}

	///\details Устанавливает значение pos - перегруженная версия
	MovableObject *setPos(GLfloat x, GLfloat y, GLfloat z)
	{
		pos.x()=x;
		pos.y()=y;
		pos.z()=z;
		return this;
	}

	///\details Возвращает занчение pos
	Point4f getPos() const { return pos; }

	///\details Устанавливает значение поворота вокруг оси Y
	MovableObject *setYRotation(float newRotation)
	{
		rotation=newRotation;
		normalizeAngle(rotation);
		return this;
	}

	///\details Устанавливает значение поворота вокруг оси X
	MovableObject *setXRotation(float newHRotation)
	{
		h_rotation=newHRotation;
		normalizeAngle(h_rotation);
		return this;
	}

	///\details Устанавливает значение поворота вокруг оси Z
	MovableObject *setZRotation(float newZRotation)
	{
		z_rotation = newZRotation;
		normalizeAngle(z_rotation);
		return this;
	}

	MovableObject *setRotation(Point4f newRotation)
	{
		setXRotation(newRotation.x());
		setYRotation(newRotation.y());
		setZRotation(newRotation.z());
		return this;
	}

	///\details Устанавливает значение поворота вокруг оси X, Y, Z
	MovableObject *setRot(Point4f rot)
	{
		setXRotation(rot.x());
		setYRotation(rot.y());
		setZRotation(rot.z());
		return this;
	}

	///\details Возвращает  значение поворота вокруг оси X
	float getXRotation() const { return h_rotation; }

	///\details Возвращает  значение поворота вокруг оси Y
	float getYRotation() const { return rotation; }

	///\details Возвращает  значение поворота вокруг оси Z
	float getZRotation() const { return z_rotation; }

	Point4f getRot() const
	{
		return Point4f(h_rotation, rotation, z_rotation);
	}

	///\details Прибавляет к pos значение shift
	void changePos(const Point4f &shift)
	{
		pos=pos+shift;
	}

	///\details Прибавляет к повороту вокруг оси Y значение deltaPhi
	void changeYRotation(float deltaPhi)
	{
		rotation+=deltaPhi;
		normalizeAngle(rotation);
	}

	///\details Прибавляет к повороту вокруг оси X значение deltaPhi
	void changeXRotation(float deltaPhi)
	{
		h_rotation+=deltaPhi;
		normalizeAngle(h_rotation);
	}

	///\details Прибавляет к повороту вокруг оси Z значение deltaPhi
	void changeZRotation(float deltaPhi)
	{
		z_rotation+=deltaPhi;
		normalizeAngle(z_rotation);
	}

	void changeRot(Point4f r)
	{
		changeXRotation(r.x());
		changeYRotation(r.y());
		changeZRotation(r.z());
	}

	///\details Устанавливает имя объекта
	void setName(const std::string &name_) { name = name_; }
	///\details Возвращает имя объекта
	const std::string &getName() { return name; }

	///\details Устанавливает absPos
	void setAbsPos(Point4f p) { absPos = p; }
	///\details Возвращает absPos
	Point4f getAbsPos() { return absPos; }

	///\details Рисование объекта - должно быть определено в дочерных классах
	virtual void paint() = 0;

	///\details Сохранение объекта в std::ostream
	virtual void save(std::ostream &o)
	{
		o<<(*this)<<std::endl;
	}

	///\details Построение теневых объемов - должно быть переопределено для объектов,
	///\details отбрасывающих тень
	///\details Если ее не переопределить, то она ничего не делает
	virtual void buildShadowVolumes(Point4f lightPos) {}

	virtual bool test(Point4f p) const { return false; }

	///\details Виртуальный деструктор - на всякий случай
	virtual ~MovableObject()
	{}

};

///\details Тип указателя на объект
typedef boost::shared_ptr<MovableObject> PMovableObject;
///\details Оператор вывода объекта в std::ostream
std::ostream & operator << (std::ostream &o, const MovableObject &m);

#endif /* MOVABLEOBJECT_H_ */
