#ifndef LEIGON_AI_H
#define LEIGON_AI_H

#include "HaliteLocation.h"
#include "HaliteMove.h"
#include "HaliteMap.h"

#include <vector>
#include <list>

//DO NOT TOUCH THESE
#include <thread>
//END

class LEIGON_AI
{

private:
	short myTag;
public:
	LEIGON_AI(short givenTag, HaliteMap initialMap);
	LEIGON_AI();

	void getMoves(HaliteMap presentMap);

	std::list<HaliteMove> moves;
};

#endif