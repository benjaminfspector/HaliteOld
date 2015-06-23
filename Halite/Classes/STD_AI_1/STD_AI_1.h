#ifndef STD_AI_1_H
#define STD_AI_1_H

#include "HaliteLocation.h"
#include "HaliteMove.h"
#include "HaliteMap.h"

#include <vector>
#include <list>

//DO NOT TOUCH THESE
#include <thread>
//END

class STD_AI_1
{

private:
	unsigned short myTag;
	unsigned char presentDirection;
public:
	STD_AI_1(unsigned short givenTag, HaliteMap initialMap);
	STD_AI_1();

	void getMoves(HaliteMap presentMap);

	std::list<HaliteMove> moves;
};

#endif