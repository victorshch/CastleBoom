/*
 * Point4f.cpp
 *
 *  Created on: 29.10.2008
 *      Author: Wictor
 */

#include "Point4f.h"

std::ostream & operator << (std::ostream &o, const Point4f &p)
{
	o<<'('<<p.x()<<", "<<p.y()<<", "<<p.z()<<')';
	return o;
}
