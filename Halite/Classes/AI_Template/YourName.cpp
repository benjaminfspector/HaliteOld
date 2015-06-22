#include "YourName.h"

YOURNAME::YOURNAME(unsigned short givenTag, HaliteMap initialMap)
{
	myTag = givenTag;
}

//Random move AI
void YOURNAME::getMoves(HaliteMap presentMap)
{
	moves = std::list<HaliteMove>(0);
	for(unsigned char y = 0; y < presentMap.hMap.size(); y++)
	{
		for(unsigned char x = 0; x < presentMap.hMap[y].size(); x++)
		{
			//Sample program - simply HaliteMoves them all upwards. Not very effective.
			if(presentMap.hMap[y][x].owner == myTag && presentMap.hMap[y][x].isSentient)
			{
				unsigned char m = rand() % 5;
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
YOURNAME::YOURNAME()
{
	myTag = 0;
}