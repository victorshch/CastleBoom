/*
 * Engine.cpp
 *
 *  Created on: 18.11.2008
 *      Author: Wictor
 */

#include <QHostAddress>

#include <QMessageBox>

#include "GameSettings.h"

#include "Engine.h"

float timeStep = 0.04;
int interval = 20;

void Engine::updateCoords()
{
	time+=timeStep;

	if(time>boomTime) time = boomTime;

	if(boomed)
	{
		emit setStatus("The ball is exploding...");
		emit needRedisplay();
		return;
	}

	ballCoord = getCurrInitCoord() + v0*time + (g + w)*(time*time/2.0);

	if(time>=boomTime)
	{
		updateHealth();
		boomed=true;
	}

	emit needRedisplay();
}

void Engine::setSettings(const QString &addrStr, int port, const GameSettings &settings)
{
	isServer = settings.isServer;

	self = isServer ? 0 : 1;

	if(isServer)
	{
		dmg = settings.dmg;
		initHealth = settings.maxHp;
		wind = settings.wind;
	}

	QHostAddress addr;
	if(!addr.setAddress(addrStr))
	{
		emit settingsResult("Couldn't parse address");
		return;
	}

	if(isServer)
	{
		if(!server.listen(addr, port))
		{
			emit settingsResult("Couldn't listen - bad address or port");
		}
		else
		{
			emit setStatus("Listening to "+addr.toString()+":"+QString::number(port));
			allOK();
		}
		return;
	}
	else
	{
		sock = new QTcpSocket();
		sock->connectToHost(addr, port);
		QObject::connect(sock, SIGNAL(error(void)), this, SLOT(connectError(void)));
		QObject::connect(sock, SIGNAL(connected(void)), this, SLOT(allOk(void)));
		QObject::connect(sock, SIGNAL(readyRead(void)), this, SLOT(readyRead(void)));
		QObject::connect(sock, SIGNAL(disconnected(void)), this, SLOT(connectError(void)));

		emit wait(true);
		if(sock->waitForConnected())
		{
			emit wait(false);
			allOK();
			return;
		}
		else
		{
			emit wait(false);
			emit settingsResult("Couldn't connect to specified host");
		}
	}
	return;
}

void Engine::allOK()
{
	self = isServer ? 0 : 1;

	for(int i=0; i<2; i++)
	{
		health[i]=initHealth;
		nextHealth[i]=initHealth;
	}

	emit settingsResult("");
	settingsSet = true;
	if(!isServer)
	{
		emit setStatus("Connected");
		initTurn();
	}
	//else sendInitData();
}

void Engine::connectError()
{
	if(!settingsSet)
		emit settingsResult("Couldn't connect to specified host");
	else
	{
		//disconnectWithError("Connection error");
		state = Error;
		emit setStatus("Connection error");
	}
}

void Engine::disconnected()
{
	emit error("Socket disconnected");
}

void Engine::newConnection()
{
	QTcpSocket *s = server.nextPendingConnection();
	if(sock) s->abort();
	else
	{
		sock = s;
		QObject::connect(sock, SIGNAL(error(void)), this, SLOT(connectError(void)));
		QObject::connect(sock, SIGNAL(readyRead(void)), this, SLOT(readyRead(void)));
		emit setStatus("Connected");
		sendInitData();
		initTurn();
	}
}

void Engine::readyRead()
{
	readData += sock->readLine();
	if(readData[readData.length()-1] == '\n')
	{
		readData.chop(1);
		processData();
		readData.clear();
	}
	if(sock->bytesAvailable()>0)
	{
		readyRead();
	}
}

void Engine::processData()
{
	QList<QByteArray> parsed = readData.split('|');
	if(parsed[0]=="INITDATA")
	{
		if(parsed.size()!=4)
		{
			disconnectWithError("Invalid socket output length: "+readData+
					"\nExpected: INITDATA|int dmg|int maxHP|bool wind");
			return;
		}

		bool b;
		int d = parsed[1].toInt(&b);
		if(!b)
		{
			disconnectWithError("Invalid socket output arg1: "+readData+
					"\nExpected: INITDATA|int dmg|int maxHP|bool wind");
			return;
		}
		dmg = d;

		d = parsed[2].toInt(&b);
		if(!b)
		{
			disconnectWithError("Invalid socket output arg2: "+readData+
					"\nExpected: INITDATA|int dmg|int maxHP|bool wind");
			return;
		}
		initHealth = d;

		if(parsed[3]!= "true" && parsed[3] != "false")
		{
			disconnectWithError("Invalid socket output arg3: `"+parsed[3]+"' of "+readData+
					"\nExpected: INITDATA|int dmg|int maxHP|bool wind");
			return;
		}


		wind = parsed[3]=="true";

		//emit setStatus("Ready");
		return;
	}
	if(parsed[0]=="RECVINITDATA")
	{
		if(parsed.size()!=1)
		{
			disconnectWithError("Invalid socket output: "+readData+
					"\nExpected: RECVINITDATA");
			return;
		}

		sendInitData();

		return;
	}
	if(parsed[0]=="TURNDATA")
	{
		QByteArray errStr("Invalid socket output: "+readData+
				"\nExpected: TURNDATA|float v0x|float v0y|float v0z|float wphi|float wr");

		if(parsed.size()!=6)
		{
			disconnectWithError(errStr + "\n(error in length)");
			return;
		}

		bool b;

		float f = parsed[1].toFloat(&b);
		if(!b)
		{
			disconnectWithError(errStr+ "\n(error in arg1)");
			return;
		}
		v0.x() = f;

		f = parsed[2].toFloat(&b);
		if(!b)
		{
			disconnectWithError(errStr+ "\n(error in arg2)");
			return;
		}
		v0.y() = f;

		f = parsed[3].toFloat(&b);
		if(!b)
		{
			disconnectWithError(errStr+ "\n(error in arg3)");
			return;
		}
		v0.z() = f;

		f = parsed[4].toFloat(&b);
		if(!b)
		{
			disconnectWithError(errStr+ "\n(error in arg4)");
			return;
		}
		wphi = f;

		f = parsed[5].toFloat(&b);
		if(!b)
		{
			disconnectWithError(errStr+ "\n(error in arg5)");
			return;
		}
		wr = f;

		if(state==Waiting)
		{
			calcWind();
			shoot();
		}
		else
		disconnectWithError("Got TURNDATA in wrong state");

		return;
	}
	if(parsed[0]=="RECVTURNDATA")
	{
		if(parsed.size()!=1)
		{
			disconnectWithError("Invalid socket output: "+readData+
					"\nExpected: RECVTURNDATA");
			return;
		}

		sendTurnData();

		return;
	}
	if(parsed[0]=="RESDATA")
	{
		QByteArray errStr("Invalid socket output: "+readData+
				"\nExpected: RESDATA|int nextHP0|int nextHP1");

		if(parsed.size()!=3)
		{
			disconnectWithError(errStr);
			return;
		}

		bool b;
		int d = parsed[1].toInt(&b);
		if(!b)
		{
			disconnectWithError(errStr);
			return;
		}
		nextHealth[0] = d;


		d = parsed[2].toInt(&b);
		if(!b)
		{
			disconnectWithError(errStr);
			return;
		}
		nextHealth[1] = d;

		resDataReceived = true;

		//QMessageBox::information(0,"Debug", "Receiving RESDATA, state = "+QString::number(state), QMessageBox::Ok, QMessageBox::Ok);

		if(state==WaitingForData)
			completeStop();

		return;
	}
}

void Engine::sendTurnData()
{
	write("TURNDATA|"
			+QByteArray::number(v0.x())
			+"|"
			+QByteArray::number(v0.y())
			+"|"
			+QByteArray::number(v0.z())
			+"|"
			+QByteArray::number(wphi)
			+"|"
			+QByteArray::number(wr));
}

void Engine::sendResData()
{
	write("RESDATA|"
			+QByteArray::number(nextHealth[0])
			+"|"
			+QByteArray::number(nextHealth[1])
			);
}

void Engine::sendInitData()
{
	write(
		"INITDATA|"
		+QByteArray::number(dmg)
		+"|"
		+QByteArray::number(initHealth)
		+"|"
		+QByteArray(wind ? "true" : "false")
		);
}
