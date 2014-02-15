/*
 * GLWidget.h
 *
 *  Created on: 23.10.2008
 *      Author: Wictor
 */

#ifndef GLWIDGET_H_
#define GLWIDGET_H_

#include <QtOpenGL/QGLWidget>
#include <QKeyEvent>
#include <QString>

#include "Camera.h"
#include "Frustumer.h"
#include "ComplexObject.h"
#include "ConcreteObjects.h"
#include "ParticleSystem.h"
#include "Shadower.h"

#include "Engine.h"

#include "Settings.h"

//#define RELEASE_VERSION
//#undef DESIGN_VERSION

#undef RELEASE_VERSION
#define DESIGN_VERSION

///\details ����� ������� ��� OpenGL (� ��� ��� ��������)
class GLWidget: public QGLWidget
{
	Q_OBJECT
public:
	///\details ����������� - � ��� ���������������� ����, ��������� � ����������� �����������
	GLWidget(QWidget *parent, Settings *s);

	///\details ���������� �������������� ������ - ��� ������������ ����������
	QSize sizeHint() const;

	Engine engine;

protected:
	///\details �������, � ������� ���������������� ����, ��������� � OpenGL
	void initializeGL();
	///\details �������, ������������� ���������� ��� ��������� �������� ����
	void resizeGL(int w, int h);
	///\details �������, ������������� ���������� ��� �����������
	void paintGL();

	///\details �������, ������������� ���������� ��� ������� �������
	void keyPressEvent(QKeyEvent *event);

private:

	///\details ������ ������
	Camera camera;

	///\details ������, ��������������� glFrustnum()
	Frustumer frustumer;

	///\details ������ �����
	ComplexObject scene;

	MovableObject *redCastle, *blueCastle, *ball;

	Arrow *redArrow, *blueArrow, *currArrow;

	ParticleSystem *particleSystem;

	///\details ������� �������, ������� ����� �������.
	///\details � release-������ �� ������������� &camera
	MovableObject *currObject, *currEdObject;

	///\details �����, ��������������� ��������� ����� ������� Shadow Volumes
	Shadower shadower;

	///\details ������ � ������ ������� ������
	int width, height;

	QString status;

	///\details �������, ����������� ������ scene ���������
	void constructScene();

	void paintScene();
public slots:
	///\details ���� ��� ��������� ����� ��������� ��������� ������
	void setFrutsumerIndex(int index);

	void redisplay();

	void setStatus(const QString &str);

	void error(const QString &str);

};

#endif /* GLWIDGET_H_ */
