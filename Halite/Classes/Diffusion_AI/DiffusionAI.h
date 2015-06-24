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
    unsigned short myTag;
public:
	DiffusionAI(unsigned short givenTag, HaliteMap initialMap);
	DiffusionAI();

	void getMoves(HaliteMap presentMap);

	std::list<HaliteMove> moves;
};

#endif