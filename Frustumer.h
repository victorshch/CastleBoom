/*
 * Frustumer.h
 *
 *  Created on: 01.11.2008
 *      Author: Wictor
 */

#ifndef FRUSTUMER_H_
#define FRUSTUMER_H_

#include <GL/gl.h>
#include <cmath>

extern GLfloat rndParams[][6][2];

class Frustumer
{
	float m_top, m_bottom, m_near, m_far;

	int index;
public:
	Frustumer(GLfloat angle_, GLfloat near_, GLfloat far_, int numShakes):
		m_near(near_), m_far(far_), index(numShakes-1)
	{
		m_top=(std::tan(angle_*0.5*M_PI/360.0))*m_near;
		m_bottom=-m_top;
	}

	void setNumShakes(int newNumShakes)
	{
		index=newNumShakes-1;
	}

	int getNumShakes() { return index+1; }

	static int getMaxNumShakes() { return 6; }

	void shakedFrustum(GLfloat width, GLfloat height, int i)
	{
		glMatrixMode (GL_PROJECTION);
		glLoadIdentity();

		GLfloat scalex, scaley, left, right, aspect;

		aspect = width/height;
		left = aspect * m_bottom;
		right = aspect * m_top;

		scalex = (right - left)/width;
		scaley = (m_top - m_bottom)/height;

		glFrustum(left - rndParams[index][i][0] * scalex,
				  right - rndParams[index][i][0] * scalex,
				  m_bottom - rndParams[index][i][1] * scaley,
				  m_top - rndParams[index][i][1] * scaley,
				  m_near, m_far);
	}

	void frustum(GLfloat width, GLfloat height)
	{
		glMatrixMode (GL_PROJECTION);
		glLoadIdentity();

		GLfloat left, right, aspect;

		aspect = width/height;
		left = aspect * m_bottom;
		right = aspect * m_top;

		glFrustum(left,
				  right,
				  m_bottom,
				  m_top,
				  m_near, m_far);
	}
};

#endif /* FRUSTUMER_H_ */
