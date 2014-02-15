/*
 * Shadower.h
 *
 *  Created on: 02.11.2008
 *      Author: Wictor
 */

#ifndef SHADOWER_H_
#define SHADOWER_H_

#include <list>

#include <cstdlib>
#include <ctime>

#include "Light.h"
#include "MovableObject.h"

#include "ShadowedObject.h"

#include "DrawUtils.h"

#include "Surface.h"

#include "Frustumer.h"

#undef PAINT_SHADOW_VOLUMES

const float maxPerturbation = 0.15;
const float minPerturbation = 0.05;

///\details �����, ��������������� ���������� ����� ������� Shadow Volumes
class Shadower
{
	///\details ������ �������������� ���������� �����
	std::list<PLight> lightList;
	///\details ������ �����
	MovableObject *scene;
	///\details ���� ����, ��� ���������� ����� ��������
	bool enabled;

	std::vector<Point4f> jitterVectors;

	///\details ���������� ����� ��� ������ ���������
	void renderShadowsForOneLight(Point4f lightPos)
	{

#ifdef PAINT_SHADOW_VOLUMES
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_BLEND);
		glColor4f(0.5, 0.5, 0.5, 0.5);
		scene->buildShadowVolumes(lightPos);
		glDisable(GL_BLEND);
		glDisable(GL_COLOR_MATERIAL);
		return;
#endif

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);
		glEnable(GL_CULL_FACE);
		glEnable(GL_STENCIL_TEST);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(0.0f, 100.0f);

		glClear(GL_STENCIL_BUFFER_BIT);

		glCullFace(GL_FRONT);
		glStencilFunc(GL_ALWAYS, 0x0, 0xff);
		glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);

		scene->buildShadowVolumes(lightPos);

		glCullFace(GL_BACK);
		glStencilFunc(GL_ALWAYS, 0x0, 0xff);
		glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);

		scene->buildShadowVolumes(lightPos);

		glDisable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_CULL_FACE);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);

		glStencilFunc(GL_NOTEQUAL, 0x0, 0xff);
		//glStencilFunc(GL_NEVER, 0x0, 0xff);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

		renderShadow();

		glDisable(GL_STENCIL_TEST);
	}

	///\details ���������/���������� ������
	void turnLights(bool t = false)
	{
		for(std::list<PLight>::iterator it=lightList.begin();
				it!=lightList.end(); it++)
		{
			if(!t) (*it)->turnOff();
			else (*it)->turnOn();
		}
	}

	int numJitters;
public:
	///\details ����������� ���������
	Shadower(): enabled(false), scene(0), numJitters(0), jitterVectors(8)
	{
		std::srand(std::time(0));
		jitterVectors[0] = Point4f(maxPerturbation, 0, 0);
		jitterVectors[1] = Point4f(-maxPerturbation, 0, 0);
		jitterVectors[2] = Point4f(0, 0, maxPerturbation);
		jitterVectors[3] = Point4f(0, 0, -maxPerturbation);
		jitterVectors[4] = Point4f(maxPerturbation, maxPerturbation, 0);
		jitterVectors[5] = Point4f(-maxPerturbation, maxPerturbation, 0);
		jitterVectors[6] = Point4f(0, maxPerturbation, maxPerturbation);
		jitterVectors[7] = Point4f(0, maxPerturbation, -maxPerturbation);
	}

	void setNumJitters(int v)
	{
		numJitters = v;
//		jitterVectors.resize(v);
//		int n = std::sqrt(numJitters);
//		for(int i=0; i<numJitters; i++)
//		{
//			float phi = rndParams[numJitters][i][0]*2*M_PI;
//			float theta = rndParams[numJitters][i][1]*M_PI;
////			for(int j=0; j<n; j++)
////			{
////
//				jitterVectors[i].x() = std::cos(phi)*std::sin(theta);
//				jitterVectors[i].y() = std::sin(phi)*std::sin(theta);
//				jitterVectors[i].z() = std::cos(theta);
////				jitterVectors[i + n*j].x() = (std::rand()%1000)/1000.0;
////				jitterVectors[i + n*j].y() = (std::rand()%1000)/1000.0;
////				jitterVectors[i + n*j].z() = (std::rand()%1000)/1000.0;
////
//				jitterVectors[i] = jitterVectors[i] * maxPerturbation;
////			}
//		}
////		for(int k = n*n; k<numJitters; k++)
////		{
////			jitterVectors[k].x() = std::cos(k*M_PI/n);
////			jitterVectors[k].y() = std::sin(k*M_PI/n);
////			jitterVectors[k].z() = 0;
////			jitterVectors[k] = jitterVectors[k] * maxPerturbation;
////		}
	}
	int getNumJitters() const { return numJitters; }

	///\details ��������� ���������� scene - ����� �������� ��� ������������� �����
	void setScene(MovableObject * newScene)
	{
		scene = newScene;
	}

	///\details ���������� ��������� ����� � ������ �������������� ���������� �����
	void addLight(PLight newLight)
	{
		lightList.push_back(newLight);
	}

	///\details ���������� ����� ���������� �����
	int numLights() { return lightList.size(); }

	///\details ��������� ���������� �����
	void enable() { enabled = true; }
	///\details ���������� ���������� �����
	void disable() { enabled = false; }

	///\details ���������� �����
	void paintShadows()
	{
		if(!enabled)
		{
			return;
		}

		for(std::list<PLight>::iterator it=lightList.begin();
		it!=lightList.end(); it++)
		{
			renderShadowsForOneLight((*it)->getAbsPos());
		}
	}

	void paintJitteredShadows(int i)
	{
		if(!enabled)
		{
			return;
		}

		for(std::list<PLight>::iterator it=lightList.begin();
		it!=lightList.end(); it++)
		{
			Point4f newPos;
			newPos = (*it)->getAbsPos() + jitterVectors[i];
			renderShadowsForOneLight(newPos);
		}
	}
};

#endif /* SHADOWER_H_ */
