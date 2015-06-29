#ifndef LEIGON_AI_H
#define LEIGON_AI_H

#include "HaliteLocation.h"
#include "HaliteMove.h"
#include "HaliteMap.h"

#include <vector>
#include <list>

#define M_PI 3.14159265358979323;

//DO NOT TOUCH THESE
#include <thread>
//END

class LEIGON_AI
{

private:
	short myTag;
	short lastDirection;
	short probGoodOverBad;
public:
	LEIGON_AI(short givenTag, HaliteMap initialMap);
	LEIGON_AI();
	static void threadPackage(LEIGON_AI * ai, HaliteMap presentMap);
	void getMoves(HaliteMap * presentMap);


	void addNorth(short x, short y, HaliteMap * map);
	void addEast(short x, short y, HaliteMap * map);
	void addSouth(short x, short y, HaliteMap * map);
	void addWest(short x, short y, HaliteMap * map);
	void addStill(short x, short y, HaliteMap * map);
	short nextAround(short num);
	short lastAround(short num);
	short oppositeSquare(short num);


	std::list<HaliteMove> moves;
};

#endif
