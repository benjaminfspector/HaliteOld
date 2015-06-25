#include "LEIGON_AI.h"

LEIGON_AI::LEIGON_AI(short givenTag, HaliteMap initialMap)
{
	myTag = givenTag;
}

//Random move AI
void LEIGON_AI::getMoves(HaliteMap presentMap)
{
	moves = std::list<HaliteMove>(0);
	for(short y = 0; y < presentMap.hMap.size(); y++)
	{
		for(short x = 0; x < presentMap.hMap[y].size(); x++)
		{
			//Sample program - simply HaliteMoves them all upwards. Not very effective.
			if(presentMap.hMap[y][x].owner == myTag && presentMap.hMap[y][x].isSentient)
			{
				short m = rand() % 5;
				switch(m)
				{
				case 0:
					moves.push_back(HaliteMove(HaliteMove::STILL, x, y));
					break;
				case 1:
					moves.push_back(HaliteMove(HaliteMove::NORTH, x, y));
					break;
				case 2:
					moves.push_back(HaliteMove(HaliteMove::EAST, x, y));
					break;
				case 3:
					moves.push_back(HaliteMove(HaliteMove::SOUTH, x, y));
					break;
				case 4:
					moves.push_back(HaliteMove(HaliteMove::WEST, x, y));
				}
			}
		}
	}
}


//DO NOT TOUCH THESE
LEIGON_AI::LEIGON_AI()
{
	myTag = 0;
}