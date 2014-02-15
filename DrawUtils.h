/*
 * DrawUtils.h
 *
 *  Created on: 26.10.2008
 *      Author: Wictor
 */

#ifndef DRAWUTILS_H_
#define DRAWUTILS_H_

//#define _STDCALL_SUPPORTED
//#define _M_IX86
//#include <GL/glut.h>
#include <GL/gl.h>
#include <math.h>
#include "Point4f.h"

///\details Функция, рисующая треугольник с разбиением
void drawTriangle(
GLfloat	x1,GLfloat	y1,GLfloat	z1,
GLfloat	x2,GLfloat	y2,GLfloat	z2,
GLfloat	x3,GLfloat	y3,GLfloat	z3,
GLuint numSlices1=1, GLuint numSlices2=1
);

///\details Функция, рисующая произвольный четырехугольник с разбиением
void drawQuad(
GLfloat	x1,GLfloat	y1,GLfloat	z1,
GLfloat	x2,GLfloat	y2,GLfloat	z2,
GLfloat	x3,GLfloat	y3,GLfloat	z3,
GLfloat	x4,GLfloat	y4,GLfloat	z4,
GLuint numSlices1, GLuint numSlices2
);

///\details Функция, рисующая треугольник с разбиением - перегруженная версия
void drawTriangle(
Point4f p1,
Point4f p2,
Point4f p3,
GLuint numSlices1, GLuint numSlices2
);

///\details Функция, рисующая произвольный четырехугольник с разбиением - перегруженная версия
void drawQuad(
const Point4f &p1,
const Point4f &p2,
const Point4f &p3,
const Point4f &p4,
GLuint numSlices1, GLuint numSlices2
);

///\details Функция, рисующая параллелограмм с разбиением
///\details (четвертая точка доопределяется на основании того, что фигура - параллелограмм)
void drawQuad2(
const Point4f &p1,
const Point4f &p2,
const Point4f &p3,
//const Point4f &p4,
GLuint numSlices1, GLuint numSlices2, bool texturize = false
);

///\details Функция, рисующая полупрозрачную тень на весь экран
void renderShadow();

//void reflectionMatrix(GLfloat reflection_matrix[4][4],
//		GLfloat plane_point[3],
//		GLfloat plane_normal[3]);

#endif /* DRAWUTILS_H_ */
