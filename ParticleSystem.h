/*
 * ParticleSystem.h
 *
 *  Created on: 23.11.2008
 *      Author: Wictor
 */

#ifndef PARTICLESYSTEM_H_
#define PARTICLESYSTEM_H_

#include "Point4f.h"
#include "MovableObject.h"
#include "MaterialObject.h"
#include "TexturedObject.h"

#define _STDCALL_SUPPORTED
#define _M_IX86
#include <GL/glut.h>


#include <list>

#include <cstdlib>

template<class T>
inline T random(const T &min, const T &max, size_t num = 1000)
{
	return min + ((max-min)*float(std::rand()%num))/float(num);
}

template<>
inline Point4f random(const Point4f &min,
		const Point4f &max, size_t num)
{
	return Point4f(random(min.x(), max.x(), num), random(min.y(), max.y(), num),
			random(min.z(), max.z(), num));
}

class ParticleSystem: public MaterialObject, public TexturedObject
{
	//friend class Particle;
	public:
	class Particle {
			ParticleSystem *pSystem;
			// ���� ���������� �������
			bool alive;

			// ������� "�����������" ������� (� ���������� [0.0; 1.0]) � ��������
			// "��������" (���������� �������� ���������� Life) �������
			float life, lifeFade;

			// ���������, �������� � ��������� �������
			Point4f position, velocity, acceleration;

	public:
			bool isAlive() const { return alive; }

			const Point4f &getPos() const { return position; }

			float getLife() const { return life; }

			Particle(ParticleSystem *parent): pSystem(parent), alive(false)
			{}

			void spawn()
			{
				alive = true;
				position = pSystem->genPos();
				velocity = pSystem->genVel();
				acceleration = pSystem->genAcc();
				life = pSystem->getInitLife();
				lifeFade = pSystem->genLifeFade();
			}

			void update(float dt)
			{
				if(!alive) return;
				life-=lifeFade*dt;
				if(life<=0)
				{
					alive = false;
					return;
				}
				position += velocity*dt;
				velocity += acceleration*dt;
			}

			void kill()
			{
				alive = false;
			}

	};

	~ParticleSystem()
	{
		std::srand(std::time(0));
		stop();
	}

	// ������������� � ��������������� ������� ������
	ParticleSystem(PMaterial m,
			PTexture texture,
			const bool looped_,
			size_t particlesCount_,
			size_t spawnRate_,
		const float spawnTime_,
		const float size_,
		float initLife_,
		const float lifeFadeMin_,
		const float lifeFadeMax_,
		const Point4f &positionMin_,
		const Point4f &positionMax_,
		const Point4f &velocityMin_,
		const Point4f &velocityMax_,
		const Point4f &accelerationMin_,
		const Point4f &accelerationMax_):
			MaterialObject(m),
			TexturedObject(texture),
			looped(looped_),
			particlesCount(particlesCount_),
			spawnRate(spawnRate_),
			spawnTime(spawnTime_),
			size(size_),
			initLife(initLife_),
			lifeFadeMin(lifeFadeMin_),
			lifeFadeMax(lifeFadeMax_),
			positionMin(positionMin_),
			positionMax(positionMax_),
			velocityMin(velocityMin_),
			velocityMax(velocityMax_),
			accelerationMin(accelerationMin_),
			accelerationMax(accelerationMax_), particles(particlesCount, Particle(this))
	{}

	// ���������� ������� ������
	void update(const float dt)
	{
		size_t spawnCount = 0;

		timeCounter += dt;
		if(looped)
		{
		spawnTimeCounter += dt;
			if(spawnTimeCounter > spawnTime)
			{
				spawnTimeCounter = 0;
				spawnCount = spawnRate;
			}
		}

		for (cont_t::iterator it=particles.begin();  it!=particles.end();  it++)
		{
			if(it->isAlive()) it->update(dt);
			else if(spawnCount>0)
			{
				it->spawn();
				spawnCount--;
			}
		}
	}

	void paint()
	{
		if(!visible) return;

		// ��������� ��� ���������� ��������� OpenGL
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		applyTexture();
		startTexturization();
		glEnable(GL_COLOR_MATERIAL);
		// �������� ���������� ������
		glEnable(GL_BLEND);
		// ����� ������ ���������� ������ ���, ����� ������� ���� ���������������
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		// ���� Z-Buffer'� ��������
		glEnable(GL_DEPTH_TEST);
		// ������ � Z-Buffer ��������� - ����� ������� "��������" ����� � ������� ����
		// �� ����� ��� ���������
		glDepthMask(GL_FALSE);

		// ������� ������� ������� �������� ��������������
		float modelviewMatrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, modelviewMatrix);
		// �������� ������� ����� � ������
		Point4f Right((float)modelviewMatrix[0], (float)modelviewMatrix[4], (float)modelviewMatrix[8]);
		Point4f Up((float)modelviewMatrix[1], (float)modelviewMatrix[5], (float)modelviewMatrix[9]);
		//Right.normalize();
		//Up.normalize();
		// �������� ������ ���������� �������
		Point4f Coo[4];
		Coo[0] = (-Right-Up)*size; Coo[1] = (Right-Up)*size;
		Coo[2] = (Right+Up)*size; Coo[3] = (-Right+Up)*size;

		// �������� ��������� ����������������
		glBegin(GL_QUADS);
		// ���������� �������� �� ����� ������� ���������� �� �������

		for (cont_t::const_iterator it = particles.begin(); it!=particles.end(); it++)
		{
			if(!it->isAlive()) continue;
			// ����� ���� ���, ����� ������ ��� ��������������
			//glColor4f(1.0f, 1.0f, 1.0f, /*1.0f*/it->getLife());

			material->setAlpha(it->getLife());
			applyMaterial();
			// ������ � �������� ���������� ��������� ������ ������� ���������������
			glTexCoord2f(1.0f,1.0f);
			//glVertex3f((P->getPos()+Coo[0]).x(),(P->getPos()+Coo[0]).y(), (P->getPos()+Coo[0]).z());
			(getPos() + it->getPos()+Coo[0]).Vertex3f();

			glTexCoord2f(0.0f,1.0f);
			//glVertex3f((P->getPos()+Coo[1]).x(),(P->getPos()+Coo[1]).y(), (P->getPos()+Coo[1]).z());
			(getPos() + it->getPos()+Coo[1]).Vertex3f();

			glTexCoord2f(0.0f,0.0f);
			//glVertex3f((P->getPos()+Coo[2]).x(),(P->getPos()+Coo[2]).y(), (P->getPos()+Coo[2]).z());
			(getPos() + it->getPos()+Coo[2]).Vertex3f();

			glTexCoord2f(1.0f,0.0f);
			//glVertex3f((P->getPos()+Coo[3]).x(),(P->getPos()+Coo[3]).y(), (P->getPos()+Coo[3]).z());
			(getPos() + it->getPos()+Coo[3]).Vertex3f();
		}	// for (unsigned int i=0; i<_PS->GetParticlesCount(); i++)
		// ����������� ��������� ����������������
		glEnd();	// glBegin(GL_QUADS);

		// ������������ ��� ���������� ��������� OpenGL
		glPopAttrib();
	}


	float getTime() { return timeCounter; }

	void start()
	{
		timeCounter=0;
		setVisible(true);
		for (cont_t::iterator it=particles.begin();  it!=particles.end();  it++)
		{
			it->spawn();
		}
	}

	void stop()
	{
		setVisible(false);
	}

	Point4f genPos()
	{
		return random(positionMin, positionMax);
	}

	Point4f genVel()
	{
		return random(velocityMin, velocityMax);
	}

	Point4f genAcc()
	{
		return random(accelerationMin, accelerationMax);
	}

	float genLifeFade()
	{
		return random(lifeFadeMin, lifeFadeMax);
	}

	float getInitLife()
	{
		return initLife;
	}

	protected:
	// ����� ������������� ������ ������� ������ � ���������� ������� ������
	bool looped;

	unsigned int particlesCount;
	// ���������� ������ ����������� �� ���������� ������� SpawnTime, ����
	// ����� ���������� ������� � ������� ������� ��� ��������� ������
	unsigned int spawnRate;
	float spawnTime, spawnTimeCounter, timeCounter;
	// ������ ������������� ������� ������ �������
	float size;
	// ���������� � ����������� ��������� �������� �������� ������
	float initLife, lifeFadeMin, lifeFadeMax;
	// ���������� � ����������� ��������� �������� ������� ����� ������������
	// ������� ������������ ������� ��������� ������� ������
	Point4f positionMin, positionMax;
	// ���������� � ����������� ��������� �������� ����� ������
	Point4f velocityMin, velocityMax;
	// ���������� � ����������� ��������� ��������� ����� ������
	Point4f accelerationMin, accelerationMax;

	typedef std::vector<Particle> cont_t;
	// ������ ���� ������������ (�����) ������
	cont_t particles;

	private:
};

#endif /* PARTICLESYSTEM_H_ */
