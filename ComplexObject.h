/*
 * ComplexObject.h
 *
 *  Created on: 29.10.2008
 *      Author: Wictor
 */

#ifndef COMPLEXOBJECT_H_
#define COMPLEXOBJECT_H_

#include <boost/shared_ptr.hpp>
#include <list>
#include <iostream>

#include "MovableObject.h"

///\details ����� ��� ����������� ���������� �������� (�.�. ����������� ���� � ������ ��������)
class ComplexObject: public MovableObject
{
	///\details ��� ������ ��������
	typedef std::list<PMovableObject> container_t;

	///\details ������ ��������, �� ������� ������� ������ ������ (�.�. ��������� �� ���� ���)
	container_t objects;
public:
	///\details ����������� ���������
	ComplexObject()
	{}

	///\details ���������� ������� (�.�. ��������� �� ������� ���� ����)
	void addObject(MovableObject *obj)
	{
		if(obj) objects.push_back(PMovableObject(obj));
	}

	void addObject(PMovableObject pobj)
	{
		if(pobj!=PMovableObject()) objects.push_back(pobj);
	}

	MovableObject * setVisible(bool b)
	{
		visible=b;
		for(container_t::const_iterator it=objects.begin(); it!=objects.end(); it++)
		{
			(*it)->setVisible(b);
		}
		return this;
	}

	///\details ��������� ��������, �� ������� ������� ������ ������,
	///\details � ������ ��������� � ��������
	void paint()
	{
		if(!visible) return;
		glPushMatrix();
		applyTransformations();
		for(container_t::const_iterator it=objects.begin(); it!=objects.end(); it++)
		{
			(*it)->paint();
		}
		glPopMatrix();
	}

	///\details ���������� ������� � ����� std::ostream
	virtual void save(std::ostream &o)
	{
		o<<*this<<std::endl;
		o<<"{"<<std::endl;
		for(container_t::const_iterator it=objects.begin(); it!=objects.end(); it++)
		{
			(*it)->save(o);
		}
		o<<"}"<<std::endl;
	}

	///\details ���������� ������� ������� ���� ��������, �������� � ������
	void buildShadowVolumes(Point4f lightPos)
	{
		if(!visible) return;

		lightPos = lightPos - getPos();
		lightPos = lightPos.rotate(-getXRotation(), -getYRotation(), -getZRotation());
		glPushMatrix();
		applyTransformations();
		for(container_t::const_iterator it=objects.begin(); it!=objects.end(); it++)
		{
			(*it)->buildShadowVolumes(lightPos);
		}
		glPopMatrix();
	}

	bool test(Point4f p) const
	{
		p=p-getPos();
		//p=p.rotate(-getXRotation(), -getYRotation(), -getZRotation());
		for(container_t::const_iterator it=objects.begin(); it!=objects.end(); it++)
		{
			if((*it)->test(p)) return true;
		}
		return false;
	}
};

#endif /* COMPLEXOBJECT_H_ */
