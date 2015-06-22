#ifndef HALITEMAP_H
#define HALITEMAP_H

#include <list>
#include <fstream>
#include <vector>
#include <string>
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
	unsigned char mapWidth, mapHeight, numberOfPlayers; //# of rows and columns; NOT maximum index.

	bool inBounds(unsigned char xx, unsigned char yy);

	void outputToFile(std::string fileName);

	//To aid with speed, these are NOT safe. Check with inBounds before using them.

	float getDistance(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2);
	HaliteLocation getNorthern(unsigned char xx, unsigned char yy);
	HaliteLocation getEastern(unsigned char xx, unsigned char yy);
	HaliteLocation getSouthern(unsigned char xx, unsigned char yy);
	HaliteLocation getWestern(unsigned char xx, unsigned char yy);
	bool haveNeighbors(unsigned char x, unsigned char y, unsigned char comparisonTag); //uses given comparisonTag as ownerTag; useful for checking squares which aren't your own.
	bool haveNeighbors(unsigned char x, unsigned char y); //uses ownerTag of the sqyare.

	//End unsafe

	void punishPlayer(std::vector<unsigned short> puns);
	HaliteMap calculateResults(std::vector< std::list<HaliteMove> * > * playerMoves);
	unsigned char findWinner(); //0 means keep going; other return means there is a definite winner.

	HaliteMap();
	HaliteMap(std::vector<std::string> pNames, unsigned char mWidth, unsigned char mHeight);
};

#endif
