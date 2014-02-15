/*
 * CompiledObject.h
 *
 *  Created on: 23.11.2008
 *      Author: Wictor
 */

#ifndef COMPILEDOBJECT_H_
#define COMPILEDOBJECT_H_

#include "ComplexObject.h"

#include <boost/unordered_map.hpp>

const int maxShadowLists = 10;

class CompiledObject: public ComplexObject
{
	static int maxNumList;
	int paintList;
	bool compileShadows;

	typedef boost::unordered_map<Point4f, int> map_t;
	map_t shadowListMap;
public:
	CompiledObject(bool compileShadows_ = true): compileShadows(compileShadows_)
	{
		paintList = maxNumList+1;
		maxNumList+=1;
	}

	void compile()
	{
		glNewList(paintList, GL_COMPILE);
		ComplexObject::paint();
		glEndList();
	}

	void paint()
	{
		glCallList(paintList);
	}

	void buildShadowVolumes(Point4f lightPos)
	{
		if(compileShadows)
		{
			map_t::iterator it = shadowListMap.find(lightPos);
			if(it==shadowListMap.end())
			{
				int newList = (maxNumList+1);
				maxNumList+=1;
				glNewList(newList, GL_COMPILE_AND_EXECUTE);
					ComplexObject::buildShadowVolumes(lightPos);
				glEndList();
				if(shadowListMap.size()>maxShadowLists-1)
				{
					glDeleteLists(shadowListMap.begin()->second, 1);
					shadowListMap.erase(shadowListMap.begin());
				}
				shadowListMap[lightPos]=newList;
			}
			else
			glCallList(it->second);
		}
		else
		{
			ComplexObject::buildShadowVolumes(lightPos);
		}
	}
};

#endif /* COMPILEDOBJECT_H_ */
