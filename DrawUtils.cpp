/*
 * DrawUtils.cpp
 *
 *  Created on: 26.10.2008
 *      Author: Wictor
 */

#include "DrawUtils.h"
#include "Point4f.h"
#include "Material.h"

void drawTriangle(
GLfloat	x1,GLfloat	y1,GLfloat	z1,
GLfloat	x2,GLfloat	y2,GLfloat	z2,
GLfloat	x3,GLfloat	y3,GLfloat	z3,
GLuint numSlices1, GLuint numSlices2
)
{
	Point4f p1(x1, y1, z1);
	Point4f p2(x2, y2, z2);
	Point4f p3(x3, y3, z3);

	Point4f n=((p2-p1)*(p3-p1));
	n.normalize();

	Point4f curr_p1, curr_p2;

	for(size_t i=0; i<numSlices1; i++)
	{
		float k1=float(i)/numSlices1, k2=float(i+1)/numSlices1;
		glBegin(GL_QUAD_STRIP);

		//debug
//		glColor3f(float(i)/numSlices1, float(i)/numSlices1, float(i)/numSlices1);

		glNormal3f(n.x(), n.y(), n.z());
		for(size_t j=0; j<numSlices2; j++)
		{
			float l1=float(j)/numSlices2;

			curr_p1=(p1*k1 + p2*(1-k1))*(1-l1) + p3*l1;
			curr_p2=(p1*k2 + p2*(1-k2))*(1-l1) + p3*l1;

			curr_p2.Vertex3f();
			curr_p1.Vertex3f();
		}
		p3.Vertex3f();
		p3.Vertex3f();
		glEnd();
	}

}

void drawTriangle(
Point4f p1,
Point4f p2,
Point4f p3,
GLuint numSlices1, GLuint numSlices2
)
{
	drawTriangle(p1.x(), p1.y(), p1.z(), p2.x(), p2.y(), p2.z(), p3.x(), p3.y(), p3.z(),
			numSlices1, numSlices2);
}

void drawQuad(
GLfloat	x1,GLfloat	y1,GLfloat	z1,
GLfloat	x2,GLfloat	y2,GLfloat	z2,
GLfloat	x3,GLfloat	y3,GLfloat	z3,
GLfloat	x4,GLfloat	y4,GLfloat	z4,
GLuint numSlices1, GLuint numSlices2
)
{
	drawTriangle(x1, y1, z1, x2, y2, z2, x3, y3, z3, numSlices1, numSlices2);
	drawTriangle(x1, y1, z1, x3, y3, z3, x4, y4, z4, numSlices1, numSlices2);
}

void drawQuad(
const Point4f &p1,
const Point4f &p2,
const Point4f &p3,
const Point4f &p4,
GLuint numSlices1, GLuint numSlices2
)
{
	drawQuad(p1.x(), p1.y(), p1.z(), p2.x(), p2.y(), p2.z(), p3.x(), p3.y(), p3.z(), p4.x(), p4.y(), p4.z(), numSlices1, numSlices2);
}

void drawQuad2(
const Point4f &p1,
const Point4f &p2,
const Point4f &p3,
GLuint numSlices1, GLuint numSlices2, bool texturize
)
{
	//ofstream fstr("debug.txt");

	Point4f v1=(p2-p1);
	Point4f v2=(p3-p1);

	Point4f p4 = p1 + v1 + v2;

	Point4f n=(v1*v2);
	n.normalize();


	float k1=0, k2;
	float step1 = float(1)/numSlices1;
	float step2 = float(1)/(numSlices2);
	for(size_t i=0; i<numSlices1; i++)
	{
//		float k1=float(i)/numSlices1, k2=float(i+1)/numSlices1;
		k2=0;

		if(!texturize)
		{
			glBegin(GL_QUAD_STRIP);

			glNormal3f(n.x(), n.y(), n.z());
			for(size_t j=0; j<=numSlices2; j++)
			{
				Point4f curr_p1=p1 + v1*k1 + v2*k2;
				Point4f curr_p2=p1 + v1*(k1 + step1) + v2*k2;
				k2+=step2;

				curr_p1.Vertex3f();
				curr_p2.Vertex3f();
			}

			glEnd();
		}
		else
		{
			glBegin(GL_QUAD_STRIP);

			glNormal3f(n.x(), n.y(), n.z());
			for(size_t j=0; j<=numSlices2; j++)
			{
				Point4f curr_p1=p1 + v1*k1 + v2*k2;
				Point4f curr_p2=p1 + v1*(k1 + step1) + v2*k2;

				glTexCoord2f(k1, k2);
				curr_p1.Vertex3f();

				glTexCoord2f(k1 + step1, k2);
				curr_p2.Vertex3f();
				k2+=step2;
			}

			glEnd();
		}
		k1+=step1;
	}
}

const Material shadowMaterial(0.0, 0.0, 0.0, 0.3,
		0.0, 0.0, 0.0,0.3,
		0.0, 0.0, 0.0,0.3,
		0.1*128);

void renderShadow()
{
	glMatrixMode (GL_MODELVIEW);
	glPushMatrix ();
	glLoadIdentity ();
	glMatrixMode (GL_PROJECTION);
	glPushMatrix ();
	glLoadIdentity ();
	shadowMaterial.apply(GL_FRONT);
	glEnable(GL_BLEND);
	glBegin (GL_QUADS);
		glVertex3i (-1, -1, -1);
		glVertex3i (1, -1, -1);
		glVertex3i (1, 1, -1);
		glVertex3i (-1, 1, -1);
	glEnd ();
	glDisable(GL_BLEND);
	glPopMatrix ();
	glMatrixMode (GL_MODELVIEW);
	glPopMatrix ();
}

void reflectionMatrix(GLfloat reflection_matrix[4][4],
		GLfloat plane_point[3],
		GLfloat plane_normal[3])
{
	GLfloat* p = (GLfloat*)plane_point;
	GLfloat* v = (GLfloat*)plane_normal;
	float pv = p[0]*v[0]+p[1]*v[1]+p[2]*v[2];
	reflection_matrix[0][0] = 1 - 2 * v[0] * v[0];
	reflection_matrix[1][0] = - 2 * v[0] * v[1];
	reflection_matrix[2][0] = - 2 * v[0] * v[2];
	reflection_matrix[3][0] = 2 * pv * v[0];
	reflection_matrix[0][1] = - 2 * v[0] * v[1];
	reflection_matrix[1][1] = 1- 2 * v[1] * v[1];
	reflection_matrix[2][1] = - 2 * v[1] * v[2];
	reflection_matrix[3][1] = 2 * pv * v[1];
	reflection_matrix[0][2] = - 2 * v[0] * v[2];
	reflection_matrix[1][2] = - 2 * v[1] * v[2];
	reflection_matrix[2][2] = 1 - 2 * v[2] * v[2];
	reflection_matrix[3][2] = 2 * pv * v[2];
	reflection_matrix[0][3] = 0;
	reflection_matrix[1][3] = 0;
	reflection_matrix[2][3] = 0;
	reflection_matrix[3][3] = 1;
}
