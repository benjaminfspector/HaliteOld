#ifndef DIFFUSIONAI2_H
#define DIFFUSIONAI2_H

#include "HaliteLocation.h"
#include "HaliteMove.h"
#include "HaliteMap.h"

#include <vector>
#include <list>

//DO NOT TOUCH THESE
#include <thread>
//END

class DiffusionAI2
{
private:
    short myTag;
   unsigned short lastDirection = 0;
public:
	DiffusionAI2(short givenTag, HaliteMap initialMap);
	DiffusionAI2();
	static void threadPackage(DiffusionAI2 * ai, HaliteMap presentMap);
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