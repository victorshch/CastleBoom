/*
 * Camera.h
 *
 *  Created on: 26.10.2008
 *      Author: Wictor
 */

#ifndef CAMERA_H_
#define CAMERA_H_


#define _STDCALL_SUPPORTED
#define _M_IX86
#include <GL/glut.h>
#include <vector>
#include "Point4f.h"

#include "MovableObject.h"

///\details ������� ������ 1
const Point4f defPos(0, 1, -5.4);

///\details ������� ������ 1
const Point4f defRot(0, 0, 0);

///\details ������� ������ 2
const Point4f defPos2(0, 0.6, 0.81);

///\details ������� ������ 2
const Point4f defRot2(25, 0, 0);

///\details ������� ������ 3
const Point4f defPos3(0, 0.5, 0.81);

///\details ������� ������ 3
const Point4f defRot3(0, 0, 0);

const Point4f defTarget(-1, 0, 0);

const int maxPositions = 6;

///\details ����� ��� ������
class Camera : public MovableObject
{
	Point4f target;
	Point4f positions[6];
	GLfloat yRotations[6];
public:
	///\details �����������, ���������������� ��������� ������ ���������� 1
	Camera(): MovableObject(defPos), target(defTarget)
	{
		positions[3] = Point4f(4.5, 1, 6.5);
		yRotations[3] = -45;

		positions[0] = Point4f(4.5, 1, -6.5);
		yRotations[0] = 45;

		positions[1] = Point4f(-4.5, 1, -6.5);
		yRotations[1] = 135;

		positions[2] = Point4f(-4.5, 1, 6.5);
		yRotations[2] = -135;

		positions[4] = Point4f(7.5, 1, 0);
		yRotations[4] = 0;

		positions[5] = Point4f(-7.5, 1, 0);
		yRotations[5] = 180;

	}

	void setPos(int n)
	{
		MovableObject::setPos(positions[n-1]);
		setYRotation(yRotations[n-1]);
	}

	///\details ������� ������ ������ � ��������� pos (������� ���������� � ������ MovableObject)
	///\details � ������������ �� � ������������ � h_rotation, rotation, z_rotation
	void paint()
	{
		glMatrixMode (GL_MODELVIEW);
		glLoadIdentity();

		gluLookAt(
				pos.x(),pos.y(),pos.z(),
				pos.x() + target.x(),
				pos.y() + target.y(),
				pos.z() + target.z(),
				0.0f,1.0f,0.0f);
	}

	void advance(GLfloat f)
	{
		changePos(target*f);
	}

	void strafe(GLfloat f)
	{
		changePos(target.rotate(0, 90, 0)*f);
	}

	void changeYRotation(float deltaPhi)
	{
		MovableObject::changeYRotation(deltaPhi);
		target=defTarget.rotate(getXRotation(), getYRotation(), getZRotation());
	}

	void setYRotation(GLfloat phi)
	{
		MovableObject::setYRotation(phi);
		target=defTarget.rotate(getXRotation(), getYRotation(), getZRotation());
	}
};

#endif /* CAMERA_H_ */
