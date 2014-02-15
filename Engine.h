/*
 * Engine.h
 *
 *  Created on: 17.11.2008
 *      Author: Wictor
 */

#ifndef ENGINE_H_
#define ENGINE_H_

#include <QObject>
#include <QTimer>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtGlobal>
#include <QTime>

#include <fstream>
using std::ofstream;
using std::endl;

#include "Point4f.h"
#include "MovableObject.h"
#include "ConcreteObjects.h"
#include "GameSettings.h"

enum State {Waiting, Aiming, BallFlying, Won, Lost, Error, WaitingForData};

const Point4f g(0, -10, 0);
const float maxTime=10.0;
extern float timeStep;
extern int interval;

const float minV0 = 7.0;
const float maxV0 = 10.0;
const float stepV0 = 0.5;

const float minW = 0.1;
const float maxW = 10;
const int numSlicesW = 10;

const int initPlayer = 1;

const float timePrec = 0.001;
const float groundLevel = 0.1;

class Engine: public QObject
{
	Q_OBJECT

private:
	QTimer timer;
	Point4f v0, ballCoord, w;
	GLfloat V0;
	Point4f ballInitCoord[2];
	MovableObject *redCastle, *blueCastle;
	Arrow *arrow[2];
	float time, boomTime, dmg, wr, wphi;
	int health[2], nextHealth[2], initHealth;
	int currPlayer, self;
	bool isServer, settingsSet, wind, resDataReceived;

	ofstream log;

	QTcpServer server;

	QTcpSocket *sock;

	QByteArray readData;

	State state;

	bool boomed;

	void initTurn()
	{

		state = self==currPlayer ? Aiming : Waiting;

		emit setStatus(self==currPlayer ? "Aim and shoot" : "Wait for the other player to shoot");

		ballCoord = ballInitCoord[currPlayer];

		updateHealth();

		resDataReceived = false;

		for(int i=0; i<2; i++)
		{
			if(health[i]<=0)
			{
				if(i==self)
				{
					state = Lost;
					emit setStatus("The game is over. You lost :-P");
				}
				else
				{
					state = Won;
					emit setStatus("The game is over. You won 8-)");
				}
				arrow[i]->setVisible(false);
			}
			else
				if(i==currPlayer && state==Aiming) arrow[i]->setVisible(true);
				else arrow[i]->setVisible(false);
		}
	}

	void processData();

	void disconnectWithError(const QString &str)
	{
		sock->disconnectFromHost();
		emit error(str);
		emit setStatus("Engine error: "+str);
	}

	void write(const QByteArray &str)
	{
		int r = sock->write(str + '\n');
		if(r==-1)
		{
			disconnectWithError("Error writing to socket");
		}
	}

	void genWind()
	{
		if(!wind) return;
		wphi = qrand()%360;
		wr = minW + ((qrand()%numSlicesW)*(maxW - minW)/float(numSlicesW));
		calcWind();
	}

	void calcWind()
	{
		if(wind) w = Point4f(0, 0, wr).rotate(0, wphi, 0);
	}

	void initHP()
	{
		for(int i=0; i<2; i++)
		{
			health[i]=nextHealth[i]=initHealth;
		}
	}

	void updateHealth()
	{
		for(int i=0; i<2; i++)
		{
			health[i]=nextHealth[i];
		}
	}

	void sendTurnData();
	void sendResData();
	void sendInitData();
public:
	Engine():timer(this), boomed(0), time(0),
	state(Waiting), V0(minV0), dmg(20), initHealth(100), isServer(true), sock(0),
	settingsSet(false), readData(""), w(0, 0, 0), resDataReceived(false), wind(false)
	{
		timer.setSingleShot(false);
		timer.setInterval(interval);

		QObject::connect(&timer, SIGNAL(timeout(void)), this, SLOT(updateCoords(void)));
		QObject::connect(&server, SIGNAL(newConnection(void)), this, SLOT(newConnection(void)));

		qsrand(QTime::currentTime().elapsed());
	}
	~Engine() {}

	State getState() { return state; }

	bool getBoomed() { return boomed; }

	int getCurrPlayer() { return currPlayer; }

	Arrow *getArrow(int player = -1) { return arrow[player == -1? self : (player%2)]; }

	int getHealth(int n)
	{
		return health[n%2];
	}

	const Point4f &getCurrInitCoord() { return ballInitCoord[currPlayer]; }

	const Point4f &getBallCoords() { return ballCoord; }

	void init(Point4f redBallInitCoord_, Point4f blueBallInitCoord_,
			MovableObject *redCastle_, MovableObject *blueCastle_,
			Arrow *redArrow_, Arrow *blueArrow_)
	{
		ballInitCoord[0]=redBallInitCoord_;
		ballInitCoord[1]=blueBallInitCoord_;
		redCastle=redCastle_;
		blueCastle=blueCastle_;
		arrow[0]=redArrow_;
		arrow[1]=blueArrow_;

		currPlayer=initPlayer;

		ballCoord = ballInitCoord[initPlayer];

		initHP();

		//initTurn();
	}

	void shoot()
	{
		state=BallFlying;
		boomed=false;

		emit setStatus("The ball is flying...");

		getArrow()->setVisible(false);

		if(self == currPlayer)
		{
			v0=Point4f(0, V0, 0).rotate(getArrow()->getAbsRotation());
			genWind();
			sendTurnData();
		}

		calcBoomTime();

		if(self!=currPlayer) sendResData();

		time = 0.0;

		timer.start();
	}

	void calcBoomTime()
	{
		Point4f b;
		for(float t = timePrec; t<=maxTime; t+=timePrec)
		{
			b=getCurrInitCoord() + v0*t + (g + w)*(t*t/2.0);
			if(redCastle->test(b))
			{
				if(self != currPlayer)
				{
					nextHealth[0]=health[0]-dmg;
					resDataReceived = true;
				}
				boomTime=t;
				return;
			}
			else if(blueCastle->test(b))
			{
				if(self != currPlayer)
				{
					nextHealth[1]=health[1]-dmg;
					resDataReceived = true;
				}
				boomTime=t;
				return;
			}
			else if(b.y()<=groundLevel)
			{
				boomTime=t;
				if(self != currPlayer)
				{
					resDataReceived = true;
				}
				return;
			}
		}
		boomTime = maxTime;
	}




	void stop()
	{
		if(!boomed) return;

		boomed=false;

		state = Waiting;

		timer.stop();

		if(resDataReceived)
			completeStop();
		else
		{
			state = WaitingForData;
			emit setStatus("Waiting for the result...");
		}

	}

	void completeStop()
	{
		currPlayer = (currPlayer+1)%2;

		initTurn();

		emit nextPlayerTurn(currPlayer);
		emit needRedisplay();
	}

	bool incrV0()
	{
		if(V0 + stepV0 <= maxV0)
		{
			V0+=stepV0;
			return true;
		}
		return false;
	}

	bool decrV0()
	{
		if(V0 - stepV0 >= minV0)
		{
			V0-=stepV0;
			return true;
		}
		return false;
	}

signals:
	void needRedisplay();

	// 0 - red, 1 - blue
	void nextPlayerTurn(int);

	// "" - ok, otherwise - error
	void settingsResult(const QString &str);

	void setStatus(const QString &str);

	void error(const QString &s);

	void wait(bool);

public slots:
	// a slot for timer
	void updateCoords();

	void setSettings(const QString &, int, const GameSettings &);

	void connectError();

	void allOK();

	void newConnection();

	void readyRead();

	void disconnected();
};

#endif /* ENGINE_H_ */

