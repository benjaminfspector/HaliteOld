#ifndef YOURNAME_H
#define YOURNAME_H

#include "HaliteLocation.h"
#include "HaliteMove.h"
#include "HaliteMap.h"

#include <vector>
#include <list>

//DO NOT TOUCH THESE
#include <thread>
//END

class YOURNAME
{

private:
	short myTag;
public:
	YOURNAME(short givenTag, HaliteMap initialMap);
	YOURNAME();
	static void threadPackage(YOURNAME * ai, HaliteMap presentMap);
	void getMoves(HaliteMap * presentMap);

	std::list<HaliteMove> moves;
};

#endif