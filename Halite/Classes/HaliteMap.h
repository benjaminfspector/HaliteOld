#ifndef HALITEMAP_H
#define HALITEMAP_H

#include <time.h>
#include <math.h>
#include <list>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include "HaliteLocation.h"
#include "HaliteMove.h"

class HaliteMap
{
protected:
	void clearSentient();
public:
	std::vector< std::vector<HaliteLocation> > hMap;
	std::vector<std::string> playerNames;
	short mapWidth, mapHeight, numberOfPlayers; //# of rows and columns; NOT maximum index.

	bool inBounds(short xx, short yy);

	void outputToFile(std::string fileName);

	//To aid with speed, these are NOT safe. Check with inBounds before using them.

	float getDistance(short x1, short y1, short x2, short y2);
	float getAngle(short x1, short y1, short x2, short y2);
	HaliteLocation getNorthern(short xx, short yy);
	HaliteLocation getEastern(short xx, short yy);
	HaliteLocation getSouthern(short xx, short yy);
	HaliteLocation getWestern(short xx, short yy);
	bool haveNeighbors(short x, short y, short comparisonTag); //uses given comparisonTag as ownerTag; useful for checking squares which aren't your own.
	bool haveNeighbors(short x, short y); //uses ownerTag of the sqyare.

	//End unsafe

	void punishPlayers(std::vector<short> puns);
	HaliteMap calculateResults(std::vector< std::list<HaliteMove> * > * playerMoves);
	short findWinner(); //0 means keep going; other return means there is a definite winner.

	HaliteMap();
	HaliteMap(std::vector<std::string> pNames, short mWidth, short mHeight);
};

#endif
