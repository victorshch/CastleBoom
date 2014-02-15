/*
 * GameSettings.h
 *
 *  Created on: 23.11.2008
 *      Author: Wictor
 */

#ifndef GAMESETTINGS_H_
#define GAMESETTINGS_H_

struct GameSettings
{
	bool isServer, wind;
	int dmg, maxHp;
	GameSettings()
	{}
};

#endif /* GAMESETTINGS_H_ */
