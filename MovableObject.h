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

///\details ����������� ����� �������, �������� ��������� � ������������
class MovableObject
{
	friend std::ostream & operator << (std::ostream &o, const MovableObject &m);

	///\details ��� �������
	///\details ����� ���� ������������ ��� ���������� ������� � ����
	std::string name;

	static void normalizeAngle(GLfloat &f)
	{
		while(f>=360) f-=360;
		while(f<0) f+=360;
	}
protected:
	///\details ��������� �������
	Point4f pos;

	///\details ���������� ��������� ������� - ��� ����� �������� �������������,
	///\details ��� ������������, ��������, ��� ���������� �����
	Point4f absPos;

	///\details ������� ������ ��� Y
	float rotation;

	///\details ������� ������ ��� X
	float h_rotation;

	///\details ������� ������ ��� Z
	float z_rotation;

	bool visible;
public:
	///\details �����������, � ������� ����� ������ ���������
	MovableObject(const Point4f & initPos = Point4f(0, 0, 0) ):
		pos(initPos), rotation(0), h_rotation(0), z_rotation(0), name(""), visible(true)
	{}

	///\details ���������� �������� � ��������
	///\details ����� �������� ������ ������ paint() ������� ��������� ������,
	///\details ������ ����� ����� �������� ������ glPushMatrix() ... glPopMatrix()
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

	///\details ������������� �������� pos
	MovableObject *setPos(const Point4f &newPos)
	{
		pos=newPos;
		return this;
	}

	///\details ������������� �������� pos - ������������� ������
	MovableObject *setPos(GLfloat x, GLfloat y, GLfloat z)
	{
		pos.x()=x;
		pos.y()=y;
		pos.z()=z;
		return this;
	}

	///\details ���������� �������� pos
	Point4f getPos() const { return pos; }

	///\details ������������� �������� �������� ������ ��� Y
	MovableObject *setYRotation(float newRotation)
	{
		rotation=newRotation;
		normalizeAngle(rotation);
		return this;
	}

	///\details ������������� �������� �������� ������ ��� X
	MovableObject *setXRotation(float newHRotation)
	{
		h_rotation=newHRotation;
		normalizeAngle(h_rotation);
		return this;
	}

	///\details ������������� �������� �������� ������ ��� Z
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

	///\details ������������� �������� �������� ������ ��� X, Y, Z
	MovableObject *setRot(Point4f rot)
	{
		setXRotation(rot.x());
		setYRotation(rot.y());
		setZRotation(rot.z());
		return this;
	}

	///\details ����������  �������� �������� ������ ��� X
	float getXRotation() const { return h_rotation; }

	///\details ����������  �������� �������� ������ ��� Y
	float getYRotation() const { return rotation; }

	///\details ����������  �������� �������� ������ ��� Z
	float getZRotation() const { return z_rotation; }

	Point4f getRot() const
	{
		return Point4f(h_rotation, rotation, z_rotation);
	}

	///\details ���������� � pos �������� shift
	void changePos(const Point4f &shift)
	{
		pos=pos+shift;
	}

	///\details ���������� � �������� ������ ��� Y �������� deltaPhi
	void changeYRotation(float deltaPhi)
	{
		rotation+=deltaPhi;
		normalizeAngle(rotation);
	}

	///\details ���������� � �������� ������ ��� X �������� deltaPhi
	void changeXRotation(float deltaPhi)
	{
		h_rotation+=deltaPhi;
		normalizeAngle(h_rotation);
	}

	///\details ���������� � �������� ������ ��� Z �������� deltaPhi
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

	///\details ������������� ��� �������
	void setName(const std::string &name_) { name = name_; }
	///\details ���������� ��� �������
	const std::string &getName() { return name; }

	///\details ������������� absPos
	void setAbsPos(Point4f p) { absPos = p; }
	///\details ���������� absPos
	Point4f getAbsPos() { return absPos; }

	///\details ��������� ������� - ������ ���� ���������� � �������� �������
	virtual void paint() = 0;

	///\details ���������� ������� � std::ostream
	virtual void save(std::ostream &o)
	{
		o<<(*this)<<std::endl;
	}

	///\details ���������� ������� ������� - ������ ���� �������������� ��� ��������,
	///\details ������������� ����
	///\details ���� �� �� ��������������, �� ��� ������ �� ������
	virtual void buildShadowVolumes(Point4f lightPos) {}

	virtual bool test(Point4f p) const { return false; }

	///\details ����������� ���������� - �� ������ ������
	virtual ~MovableObject()
	{}

};

///\details ��� ��������� �� ������
typedef boost::shared_ptr<MovableObject> PMovableObject;
///\details �������� ������ ������� � std::ostream
std::ostream & operator << (std::ostream &o, const MovableObject &m);

#endif /* MOVABLEOBJECT_H_ */
