#include "STD_AI_1.h"

STD_AI_1::STD_AI_1(unsigned short givenTag, HaliteMap initialMap)
{
	myTag = givenTag;
}

//Random move AI
void STD_AI_1::getMoves(HaliteMap presentMap)
{
	moves = std::list<HaliteMove>(0);

	//Find list of locations we need to move
	struct loc { unsigned short x, y; };
	std::list<loc> toMove;
	for(unsigned char y = 0; y < presentMap.hMap.size(); y++)
	{
		for(unsigned char x = 0; x < presentMap.hMap[y].size(); x++)
		{
			if(presentMap.hMap[y][x].owner == myTag && presentMap.hMap[y][x].isSentient)
			{
				toMove.push_back({ x, y });
			}
		}
	}

	for(unsigned char y = 0; y < presentMap.hMap.size(); y++)
	{
		for(unsigned char x = 0; x < presentMap.hMap[y].size(); x++)
		{
			presentMap.hMap[y][x].isSentient = false;
		}
	}

	//Move them:
	enum goodness { BEST, MID, BAD };
	for(auto a = toMove.begin(); a != toMove.end(); a++)
	{
		HaliteLocation around[5];
		around[0] = presentMap.getNorthern(a->x, a->y);
		around[1] = presentMap.getEastern(a->x, a->y);
		around[2] = presentMap.getSouthern(a->x, a->y);
		around[3] = presentMap.getWestern(a->x, a->y);
		around[4] = presentMap.hMap[a->y][a->x];
		goodness possibilities[5];
		for(unsigned short b = 0; b < 5; b++)
		{
			if(around[b].owner != myTag) possibilities[b] = BEST;
			else if(!around[b].isSentient) possibilities[b] = MID;
			else possibilities[b] = BAD;
		}

		goodness best;

	}
}


//DO NOT TOUCH THESE
STD_AI_1::STD_AI_1()
{
	myTag = 0;
}