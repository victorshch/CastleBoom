/*
 * Mirror.h
 *
 *  Created on: 03.11.2008
 *      Author: Wictor
 */

#ifndef MIRROR_H_
#define MIRROR_H_

#include "MovableObject.h"
#include "Surface.h"
#include "DrawUtils.h"

class Mirror
{
	MovableObject *mirrorObject, *scene;
	PSurface surface;

	GLfloat matrix[4][4];
	void calcMatrix()
	{
		Point4f norm = surface->normal();
		norm.normalize();
		Point4f point = (*surface)[0];
		GLfloat n[3], p[3];
		n[0] = norm.x;
		n[1] = norm.y;
		n[2] = norm.z;

		p[0] = point.x;
		p[1] = point.y;
		p[2] = point.z;

		reflectionMatrix(matrix, p, n);
	}

public:
	Mirror()
	{}

	void setObject(MovableObject *object)
	{
		mirrorObject = object;
	}

	void setScene(MovableObject *object)
	{
		scene = object;
	}

	void setSurface(Point4f p1, Point4f p2, Point4f p3, Point4f p4)
	{
		surface = PSurface(new Surface(p1, p2, p3, p4));
		calcMatrix();
	}

	void setSurface(Surface *s)
	{
		surface = PSurface(s);
		calcMatrix();
	}

	void renderMirroredImage()
	{
		glEnable(GL_STENCIL_TEST);
		/* условие всегда выполнено и значение в буфере будет
		равно 1*/
		glStencilFunc(GL_ALWAYS, 1, 0);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		//mirrorObject->paint();
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);

		surface->render();

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);
		/* условие выполнено и тест дает истину только если
		значение в буфере маски равно 1 */
		glStencilFunc (GL_EQUAL, 0x1, 0xffffffff);
		/* ничего не меняем в буфере */
		glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);
		glPushMatrix();
		//glMultMatrixf((float *)matrix);
		//glRotatef(180, 1, 0, 0);
		glTranslatef(0, -2, 0);
		scene->paint();
		glPopMatrix();
		glDisable(GL_STENCIL_TEST);

		mirrorObject->paint();
	}
};

#endif /* MIRROR_H_ */
