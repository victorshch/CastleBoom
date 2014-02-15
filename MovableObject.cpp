/*
 * MovableObject.cpp
 *
 *  Created on: 29.10.2008
 *      Author: Wictor
 */

#include "MovableObject.h"

std::ostream & operator << (std::ostream &o, const MovableObject &m)
{
	o<<m.name<<' '<<m.pos<<' '<<m.rotation<<' '<<m.h_rotation<<' '<<m.z_rotation;
	return o;
}
