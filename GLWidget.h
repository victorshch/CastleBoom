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

///\details Класс виджета для OpenGL (в нем все рисуется)
class GLWidget: public QGLWidget
{
	Q_OBJECT
public:
	///\details Конструктор - в нем инициализируются вещи, связанные с графическим интерфейсом
	GLWidget(QWidget *parent, Settings *s);

	///\details Возвращает предпочитаемый размер - для графического интерфейса
	QSize sizeHint() const;

	Engine engine;

protected:
	///\details Функция, в которой инициализируются вещи, связанные с OpenGL
	void initializeGL();
	///\details Функция, автоматически вызываемая при изменении размеров окна
	void resizeGL(int w, int h);
	///\details Функция, автоматически вызываемая при перерисовке
	void paintGL();

	///\details Функция, автоматически вызываемая при нажатии клавиши
	void keyPressEvent(QKeyEvent *event);

private:

	///\details Объект камеры
	Camera camera;

	///\details Объект, инкапсулирующий glFrustnum()
	Frustumer frustumer;

	///\details Объект сцены
	ComplexObject scene;

	MovableObject *redCastle, *blueCastle, *ball;

	Arrow *redArrow, *blueArrow, *currArrow;

	ParticleSystem *particleSystem;

	///\details Текущие объекты, которые можно двигать.
	///\details В release-версии им присваивается &camera
	MovableObject *currObject, *currEdObject;

	///\details Класс, инкапсулирующий наложение теней методом Shadow Volumes
	Shadower shadower;

	///\details Ширина и высота области вывода
	int width, height;

	QString status;

	///\details Функция, заполняющая объект scene объектами
	void constructScene();

	void paintScene();
public slots:
	///\details слот для изменения числа изменений положения камеры
	void setFrutsumerIndex(int index);

	void redisplay();

	void setStatus(const QString &str);

	void error(const QString &str);

};

#endif /* GLWIDGET_H_ */
