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
	unsigned short myTag;
public:
	YOURNAME(unsigned short givenTag, HaliteMap initialMap);
	YOURNAME();

	void getMoves(HaliteMap presentMap);

	std::list<HaliteMove> moves;
};

#endif