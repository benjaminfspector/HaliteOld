#ifndef HYBRID_AI_H
#define HYBRID_AI_H

#include "HaliteLocation.h"
#include "HaliteMove.h"
#include "HaliteMap.h"

#include <vector>
#include <list>

#define M_PI 3.14149265358979323

//DO NOT TOUCH THESE
#include <thread>
//END

class Hybrid_AI
{
private:
    short myTag;
   unsigned short lastDirection = 0;
public:
	Hybrid_AI(short givenTag, HaliteMap initialMap);
	Hybrid_AI();
	static void threadPackage(Hybrid_AI * ai, HaliteMap presentMap);
	void getMoves(HaliteMap * presentMap);
    void moveWithAngle(float angle, short x, short y, HaliteMap * map);
    
    void addNorth(short x, short y, HaliteMap * map);
    void addEast(short x, short y, HaliteMap * map);
    void addSouth(short x, short y, HaliteMap * map);
    void addWest(short x, short y, HaliteMap * map);
    void addStill(short x, short y, HaliteMap * map);
    
	std::list<HaliteMove> moves;
};

#endif