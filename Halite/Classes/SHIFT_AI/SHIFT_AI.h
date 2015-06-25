#ifndef SHIFT_AI_H
#define SHIFT_AI_H

#include "HaliteLocation.h"
#include "HaliteMove.h"
#include "HaliteMap.h"

#include <vector>
#include <list>

//DO NOT TOUCH THESE
#include <thread>
//END

class SHIFT_AI
{
private:
	const float MAX_TURTLE_FACTOR = 0.4;
	unsigned short myTag;
	short presentDirection;
public:
	SHIFT_AI(unsigned short givenTag, HaliteMap initialMap);
	SHIFT_AI();

	void getMoves(HaliteMap presentMap);

	std::list<HaliteMove> moves;
};

#endif