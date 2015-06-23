#include "STD_AI_1.h"

STD_AI_1::STD_AI_1(unsigned short givenTag, HaliteMap initialMap)
{
	presentDirection = rand()%5;
	myTag = givenTag;
}

//Random move AI
void STD_AI_1::getMoves(HaliteMap presentMap)
{
	moves.clear();

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

		goodness bestPossible = BAD;
		for(unsigned short b = 0; b < 5; b++)
		{
			if(possibilities[b] == MID)
			{
				bestPossible = MID;
			}
			else if(possibilities[b] == BEST)
			{
				bestPossible = BEST;
				break;
			}
		}

		presentDirection++;
		if(presentDirection == 5) presentDirection = 0;
		while(possibilities[presentDirection] != bestPossible)
		{
			presentDirection++;
			if(presentDirection == 5) presentDirection = 0;
		}

		if(presentDirection == 0)
		{
			moves.push_back(HaliteMove(HaliteMove::NORTH, a->x, a->y));
			if(a->y != 0) presentMap.hMap[a->y - 1][a->x] = HaliteLocation(myTag, true);
			else presentMap.hMap[presentMap.mapHeight - 1][a->x] = HaliteLocation(myTag, true);
		}
		else if(presentDirection == 1)
		{
			moves.push_back(HaliteMove(HaliteMove::EAST, a->x, a->y));
			if(a->x != presentMap.mapWidth - 1) presentMap.hMap[a->y][a->x + 1] = HaliteLocation(myTag, true);
			else presentMap.hMap[a->y][0] = HaliteLocation(myTag, true);
		}
		else if(presentDirection == 2)
		{
			moves.push_back(HaliteMove(HaliteMove::SOUTH, a->x, a->y));
			if(a->y != presentMap.mapHeight - 1) presentMap.hMap[a->y + 1][a->x] = HaliteLocation(myTag, true);
			else presentMap.hMap[0][a->x] = HaliteLocation(myTag, true);
		}
		else if(presentDirection == 3)
		{
			moves.push_back(HaliteMove(HaliteMove::WEST, a->x, a->y));
			if(a->x != 0) presentMap.hMap[a->y][a->x - 1] = HaliteLocation(myTag, true);
			else presentMap.hMap[a->y][presentMap.mapWidth - 1] = HaliteLocation(myTag, true);
		}
		else
		{
			moves.push_back(HaliteMove(HaliteMove::STILL, a->x, a->y));
			presentMap.hMap[a->y][a->x] = HaliteLocation(myTag, true);
		}
	}
}


//DO NOT TOUCH THESE
STD_AI_1::STD_AI_1()
{
	myTag = 0;
}