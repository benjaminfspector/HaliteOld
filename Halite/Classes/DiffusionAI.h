#ifndef DIFFUSIONAI_H
#define DIFFUSIONAI_H

#include "HaliteLocation.h"
#include "HaliteMove.h"
#include "HaliteMap.h"

#include <vector>
#include <list>

//DO NOT TOUCH THESE
#include <thread>
//END

class DiffusionAI
{
private:
    short myTag;
   unsigned short lastDirection = 0;
public:
	DiffusionAI(short givenTag, HaliteMap initialMap);
	DiffusionAI();

	void getMoves(HaliteMap presentMap);

	std::list<HaliteMove> moves;
};

#endif