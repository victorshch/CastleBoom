/*
 * BlendedObject.h
 *
 *  Created on: 03.11.2008
 *      Author: Wictor
 */

#ifndef BLENDEDOBJECT_H_
#define BLENDEDOBJECT_H_

#include "ComplexObject.h"

///\details ����� - ������� ��� �������, ������� ����� �������� � �������������� GL_BLEND
class BlendedObject: public ComplexObject
{
public:
	///\details ����������� ���������
	BlendedObject()
	{}

	///\details ������� ��� ������ ���������
	void applyBlending()
	{
		glEnable(GL_BLEND);
	}

	///\details ������� ��� ���������� ���������
	void stopBlending()
	{
		glDisable(GL_BLEND);
	}

	///\details ��������� ������� � ������ ���������
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
