/*
 * MaterialObject.h
 *
 *  Created on: 29.10.2008
 *      Author: Wictor
 */

#ifndef MATERIALOBJECT_H_
#define MATERIALOBJECT_H_

#include "MovableObject.h"
#include "Material.h"

///\details ����������� ����� ��� �������, �������� ��������
///\details �� ����������� �� MovableObject �� �������������� virtual,
///\details ����� ����� ���� ���������� ������, ����������� ������������ ���� �����
///\details � ������ ������, ����������� MovableObject
class MaterialObject: public virtual MovableObject
{
protected:
	///\details��������� �� ��������
	PMaterial material;
	///\details�������, ����������� ��������.
	///\details�� ����� �������� � ������� ������, �������������� �� �������, ������ ������� paint()
	virtual void applyMaterial()
	{
		if(material!=PMaterial()) material->apply();
	}
public:
	///\details �����������, � ������� �������� ��������
	MaterialObject(const PMaterial &material_):
		material(material_)//, enabled(true)
	{}

};

#endif /* MATERIALOBJECT_H_ */
