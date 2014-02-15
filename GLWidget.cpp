/*
 * GLWidget.cpp
 *
 *  Created on: 23.10.2008
 *      Author: Wictor
 */

#include "GLWidget.h"

#include "DrawUtils.h"

#include "Material.h"

#include "MaterialObject.h"
#include "TexturedObject.h"
#include "Light.h"

#include "CompiledObject.h"

#include "ConcreteObjects.h"

#include "GameSettings.h"

//#include "Surface.h"

#include <QColor>
#include <QImage>
#include <QGLContext>
#include <QMessageBox>

#include <fstream>
using namespace std;

//debug
//ofstream debugstr("debug.txt");

const GLfloat projAngle = 60.0;
const GLfloat projNear = 0.1;
const GLfloat projFar = 100.0;
//const int defNumShakes = 6;
const float explTime = 1.5;

const Point4f redBallPos = Point4f(0, 1.0, -3.1);
const Point4f redBallRot = Point4f(0, 0, 0);
const Point4f blueBallPos = Point4f (0, 1.0, 2.9);
const Point4f blueBallRot = Point4f(0, 180, 0);

//ofstream ofstr("debug.txt");

GLWidget::GLWidget(QWidget *parent, Settings *s):
QGLWidget(parent), currObject(&camera), currEdObject(0),
frustumer(projAngle, projNear, projFar, 1), status("Waiting")
{
	setFocusPolicy(Qt::StrongFocus);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	makeCurrent ();

	QObject::connect(&engine, SIGNAL(needRedisplay(void)), this, SLOT(redisplay(void)), Qt::QueuedConnection);
	QObject::connect(&engine, SIGNAL(setStatus(const QString &)), this, SLOT(setStatus(const QString &)));

	qRegisterMetaType<GameSettings>("GameSettings");

	QObject::connect(&engine, SIGNAL(settingsResult(const QString &)), s, SLOT(settingsResult(const QString &)));
	QObject::connect(s, SIGNAL(setSettings(const QString &, int, const GameSettings &)),
				&engine, SLOT(setSettings(const QString &, int, const GameSettings &)));
	QObject::connect(&engine, SIGNAL(error(const QString &)), this, SLOT(error(const QString &)));

	s->show();
}

void GLWidget::error(const QString &str)
{
	QMessageBox::critical(this, "Engine error", str, QMessageBox::Ok, QMessageBox::Ok);
}

QSize GLWidget::sizeHint() const
{
	return QSize(400, 400);
}

void GLWidget::setFrutsumerIndex(int index)
{
	//frustumer.setNumShakes(index+1);
	shadower.setNumJitters(index + 1);
	timeStep = //(index + 1)*0.01;
		0.05 + 0.02*index;
	interval = 20*(index+1);
		//10;
	updateGL();
}

void GLWidget::setStatus(const QString &str)
{
	status = str;
}

void GLWidget::initializeGL()
{
	makeCurrent ();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	//glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClearAccum(0, 0, 0, 0);

	shadower.enable();
	//shadower.disable();
	shadower.setScene(&scene);

	glEnable(GL_NORMALIZE);

	//glDrawBuffer(GL_BACK);
	//glReadBuffer(GL_BACK);

	constructScene();
}

void GLWidget::resizeGL(int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);

	width = w;
	height = h;
}

void GLWidget::paintGL()
{
	//makeCurrent ();
	/* очищаем буфер кадра и буфер глубины */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT |
			GL_STENCIL_BUFFER_BIT);

	paintScene();

	glColor3f(1,0,0);
	renderText(10, 20, "Red castle health: "+QString::number(engine.getHealth(0)));

	glColor3f(0,0,1);
	renderText(width - 140, 20, "Blue castle health: "+QString::number(engine.getHealth(1)));

	glColor3f(0,0,0);
	renderText(10, height-10, status);

	/* выводим сцену на экран */
	glFlush ();
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
	//b->setRot(a->getRelRotation());
     switch (event->key()) {
     case Qt::Key_Left:
         if(engine.getState()==Aiming) engine.getArrow()->changeRelRotation(Point4f(0, 5, 0));
         break;
     case Qt::Key_Right:
    	 if(engine.getState()==Aiming) engine.getArrow()->changeRelRotation(Point4f(0, -5, 0));
         break;
     case Qt::Key_Down:
    	 if(engine.getState()==Aiming)
    		 if(engine.getArrow()->getRelRotation().x()<=85) engine.getArrow()->changeRelRotation(Point4f(5, 0, 0));
         break;
     case Qt::Key_Up:
    	 if(engine.getState()==Aiming)
    		 if(engine.getArrow()->getRelRotation().x()>=5) engine.getArrow()->changeRelRotation(Point4f(-5, 0, 0));
         break;
     case Qt::Key_W:
    	 if(event->modifiers() == Qt::ShiftModifier)
    		 camera.advance(0.1);
    	 else
    		 camera.advance(1);
         break;
     case Qt::Key_S:
    	 if(event->modifiers() == Qt::ShiftModifier)
    		 camera.advance(-0.1);
    	 else
    		 camera.advance(-1);
         break;
     case Qt::Key_A:
    	 if(event->modifiers() == Qt::ShiftModifier)
			 camera.strafe(0.5);
		 else
			 camera.changeYRotation(5);
    	 break;
     case Qt::Key_D:
    	 if(event->modifiers() == Qt::ShiftModifier)
			 camera.strafe(-0.5);
		 else
			 camera.changeYRotation(-5);
         break;
     case Qt::Key_PageUp:
    	 if(event->modifiers() == Qt::ShiftModifier)
			 camera.changePos(Point4f(0, 0.1, 0));
		 else
			 if(engine.getState() == Aiming) if(engine.incrV0()) engine.getArrow()->changeLen(0.1);
    	 break;
     case Qt::Key_PageDown:
         if(event->modifiers() == Qt::ShiftModifier)
        	 camera.changePos(Point4f(0, -0.1, 0));
		 else
			 if(engine.getState() == Aiming) if(engine.decrV0()) engine.getArrow()->changeLen(-0.1);
		 break;
     case Qt::Key_1:
     {
    	 camera.setPos(1);
     }break;
     case Qt::Key_2:
     {
    	 camera.setPos(2);
     }break;
     case Qt::Key_3:
     {
    	 camera.setPos(3);
     }break;
     case Qt::Key_4:
     {
    	 camera.setPos(4);
     }break;
     case Qt::Key_5:
     {
    	 camera.setPos(5);
     }break;
     case Qt::Key_6:
     {
    	 camera.setPos(6);
     }break;
     case Qt::Key_Space:
	  {
		 if(engine.getState()==Aiming) engine.shoot();
	  }break;
     default: return;
     }
     //resizeGL(geometry().width(), geometry().height());
     updateGL();
//     updateGL();
}

void GLWidget::redisplay()
{
	//if(engine.getState()==BallFlying)
		if(engine.getBoomed())
		{
			if(particleSystem->getVisible()==false)
			{
				particleSystem->setPos(engine.getBallCoords());
				particleSystem->start();
			}
			else if(particleSystem->getTime()<=explTime)
			{
				ball->setVisible(false);
				particleSystem->update(0.1);
			}
			else
			{
				particleSystem->stop();
				engine.stop();
				ball->setVisible(true);
			}
		}
		ball->setPos(engine.getBallCoords());
	updateGL();
	updateGL();
}

void GLWidget::paintScene()
{
//		glTranslatef(0, -0.5, 0);

	int numShakes = shadower.getNumJitters();
	frustumer.frustum(width, height);
	camera.paint();
	scene.paint();


	for(int i=0; i<numShakes; i++)
	{
		shadower.paintJitteredShadows(i);
		if (i==0)
		/* на первой итерации загружаем изображение */
			glAccum(GL_LOAD,1/float(numShakes));
		else
		/* добавляем к уже существующему */
		glAccum(GL_ACCUM,1/float(numShakes));
	}
	/* Пишем то, что получилось, назад в исходный буфер */
	glAccum(GL_RETURN,1.0);

	//glCallList(1);
}

void GLWidget::constructScene()
{
	camera.setPos(1);

	PTexture wallT = PTexture(new Texture(this, "textures/wall.bmp", 2.0));
	PTexture towerT = PTexture(new Texture(this, "textures/tower.bmp"));
	PTexture roofT = PTexture(new Texture(this, "textures/roof.bmp"));
	PTexture roof2T = PTexture(new Texture(this, "textures/roof2.bmp"));
	PTexture ballT = PTexture(new Texture(this, "textures/ball.bmp"));
	PTexture fireT = PTexture(new Texture(this, "textures/fire.bmp"));

	PMaterial middleM = PMaterial(new Material(
			0.1, 0.1, 0.1, 1,
			0.1f,0.1f,0.1f, 1,
			0.0f,0.0f,0.0f, 0,
			0.05f*128));

	PMaterial roofM = PMaterial(new Material(
			0.1, 0.1, 0.1, 1,
			0.1f,0.1f,0.1f, 1,
			0.5f,0.5f,0.5f, 1,
			0.1f*128));

	PMaterial wallM = PMaterial(new Material(
			1.0, 1.0, 1.0, 1,
			0.1f,0.1f,0.1f, 1,
			0.0f,0.0f,0.0f, 0,
			0.05f*128));

	PTexture earthT = PTexture(new Texture(this, "textures/grass.bmp", 1));
	//PTexture earthT = PTexture();
	PMaterial earthM = PMaterial(new Material(
			1.0, 1.0, 1.0, 1,
			0.1f,0.1f,0.1f, 1,
			0.0f,0.0f,0.0f, 0,
			0.05f*128));

	PMaterial arrowM = PMaterial(
			new Material(
					0.0, 1.0, 0.5, 0.9,
					0.0, 1.0, 0.5, 0.9,
					0.0, 1.0, 0.5, 0.9,
					0.05f*128,
					0.0, 1.0, 0.5, 0.9)
			);

	PMaterial particleM = PMaterial(
			new Material(
					1.0, 1.0, 1.0, 0.9,
					1.0, 1.0, 1.0, 0.9,
					1.0, 1.0, 1.0, 0.9,
					0.05f*128,
					1.0, 1.0, 1.0, 1.0)
			);

	PLight light = PLight(
			new Light(GL_LIGHT0,
					0.5, 0.5, 0.5, 0.5,
					1, 1, 1, 1,
					1, 1, 1, 1
					)
			);
	PMaterial sphereM = PMaterial(
			new Material(0.2f,0.2f,0.2f, 1,
			0.2f,0.2f,0.2f, 1,
			0.8f,0.8f,0.8f, 1,
			0.8f*128,
			0.9, 0.9, 0.9)
			);
	MatSphere *sphere1 = new MatSphere(sphereM, 0.1);

	ComplexObject *lighter1 = new ComplexObject;

	lighter1->addObject(light);
	lighter1->addObject(sphere1);

	lighter1->setPos(4, 4.5, 0);
	light->setAbsPos(Point4f(4, 4.5, 0));

	CompiledObject *staticScene = new CompiledObject;

	staticScene->addObject(lighter1);

	shadower.addLight(light);

	staticScene->addObject(
			new TiledTexRectangle(earthM, earthT,
					Point4f(3, 0, 5),
					Point4f(3, 0, -5),
					Point4f(-3, 0, -5),
					Point4f(-3, 0, 5)
					)
			);

	staticScene->addObject(
				(redCastle=new Castle(
						wallM, wallT,
						wallM, towerT,
						roofM, roofT,
						middleM,
						1.5, 1.0,
						0.2, 0.5,
						0.2, 1,
						0.3, 0.5
						)
				)
				->setPos(-0.95, 0, -4.5)
	);

	staticScene->addObject(
				(currEdObject=blueCastle=new Castle(
						wallM, wallT,
						wallM, towerT,
						roofM, roof2T,
						middleM,
						1.5, 1.0,
						0.2, 0.5,
						0.2, 1,
						0.3, 0.5
						)
				)
				->setPos(-0.95, 0, 2.8)
				//->setPos(-0.95, 0, 0)
	);

	staticScene->compile();

	scene.addObject(staticScene);

	CompiledObject *staticBall = new CompiledObject(false);

	staticBall->addObject(
			(new Sphere(roofM, ballT, 0.2, true, 30, 30))
			);
	staticBall->compile();

	ComplexObject *c = new ComplexObject;
	c->addObject(staticBall);
	ball = c;
//	scene.addObject(
//			(ball=new Sphere(roofM, ballT, 0.2, true, 30, 30))
//			);

	scene.addObject(ball);

	scene.addObject(
			(redArrow = new Arrow(arrowM, 0.3, 0.06, 0.01, 0.02, Point4f(45, 0, 0)))
			->setPos(redBallPos)
			->setVisible(false)
			);

	scene.addObject(
			(blueArrow = new Arrow(arrowM, 0.3, 0.06, 0.01, 0.02, Point4f(45, 0, 0)))
			->setPos(blueBallPos)
			->setYRotation(180)
			->setVisible(false)
			);

	scene.addObject(
			(particleSystem = new ParticleSystem(
					particleM,
					fireT,
					false, //looped
					300, //part.count
					1,//spawn rate
					1,//spawn time
					0.02,//size
					1.0,//init life
					0.1,//life fade min
					0.5,//life fade max
					Point4f(-0.2, -0.2, -0.2),//pos min
					Point4f(0.2, 0.2, 0.2),//pos max
					Point4f(-1, -1, -1),//v min
					Point4f(1, 1, 1),//v max
					Point4f(0, -1, 0),//a min
					Point4f(0, -1, 0)//a max
					))
					->setVisible(false)
			);


	engine.init(redBallPos, blueBallPos, redCastle, blueCastle, redArrow, blueArrow);

	ball->setPos(engine.getBallCoords());

}
