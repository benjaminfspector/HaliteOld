#ifndef TESTDIFFUSIONAI2_H
#define TESTDIFFUSIONAI2_H

#include "HaliteLocation.h"
#include "HaliteMove.h"
#include "HaliteMap.h"

#include <vector>
#include <list>

//DO NOT TOUCH THESE
#include <thread>
//END

class TestDiffusionAI2
{
private:
    short myTag;
   unsigned short lastDirection = 0;
public:
	TestDiffusionAI2(short givenTag, HaliteMap initialMap);
	TestDiffusionAI2();
	static void threadPackage(TestDiffusionAI2 * ai, HaliteMap presentMap);
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