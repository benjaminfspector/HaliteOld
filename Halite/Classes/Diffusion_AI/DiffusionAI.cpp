#include "DiffusionAI.h"

DiffusionAI::DiffusionAI(unsigned short givenTag, HaliteMap initialMap)
{
    myTag = givenTag;
}

//Random move AI
void DiffusionAI::getMoves(HaliteMap presentMap)
{
	moves.clear();

	//Find list of HaliteLocations we need to move
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
		
        
        bool foundBest = false;
        int bestIndex = 0;
        
		for(unsigned short b = 0; b < 5; b++)
		{
			if(around[b].owner != myTag) {
                foundBest = true;
                bestIndex = b;
                break;
			}
		}
        if(foundBest == true) {
            if(bestIndex == 0)
        	{
    			moves.push_back(HaliteMove(HaliteMove::NORTH, a->x, a->y));
    			if(a->y != 0) presentMap.hMap[a->y - 1][a->x] = HaliteLocation(myTag, true);
    			else presentMap.hMap[presentMap.mapHeight - 1][a->x] = HaliteLocation(myTag, true);
    		}
    		else if(bestIndex == 1)
    		{
    			moves.push_back(HaliteMove(HaliteMove::EAST, a->x, a->y));
    			if(a->x != presentMap.mapWidth - 1) presentMap.hMap[a->y][a->x + 1] = HaliteLocation(myTag, true);
    			else presentMap.hMap[a->y][0] = HaliteLocation(myTag, true);
    		}
    		else if(bestIndex == 2)
    		{
    			moves.push_back(HaliteMove(HaliteMove::SOUTH, a->x, a->y));
    			if(a->y != presentMap.mapHeight - 1) presentMap.hMap[a->y + 1][a->x] = HaliteLocation(myTag, true);
    			else presentMap.hMap[0][a->x] = HaliteLocation(myTag, true);
    		}
    		else if(bestIndex == 3)
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
        } else {
            float fieldX = 0;
            float fieldY = 0;
            
            for(auto b = toMove.begin(); b != toMove.end(); b++)
            {
                int x = b->x;
				int y = b->y;
                if(a->x == x && a->y == y) continue;
                
                float angle = presentMap.getAngle(x, y, a->x, a->y);
    	        float mag = 1 / presentMap.getDistance(x, y, a->x, a->y);
		        fieldX += mag * cos(angle);
		        fieldY += mag * sin(angle);
            }
            float finalAngle = atan2(fieldY, fieldX);
            float section = round(finalAngle / (3.141593f*0.5));
            if(section == 0 || section == 4) {
                moves.push_back(HaliteMove(HaliteMove::EAST, a->x, a->y));
        		if(a->y != 0) presentMap.hMap[a->y - 1][a->x] = HaliteLocation(myTag, true);
    			else presentMap.hMap[presentMap.mapHeight - 1][a->x] = HaliteLocation(myTag, true);
            } else if(section == 1) {
                moves.push_back(HaliteMove(HaliteMove::NORTH, a->x, a->y));
            	if(a->y != 0) presentMap.hMap[a->y - 1][a->x] = HaliteLocation(myTag, true);
    			else presentMap.hMap[presentMap.mapHeight - 1][a->x] = HaliteLocation(myTag, true);
            } else if(section == 1) {
                moves.push_back(HaliteMove(HaliteMove::WEST, a->x, a->y));
                if(a->y != 0) presentMap.hMap[a->y - 1][a->x] = HaliteLocation(myTag, true);
    			else presentMap.hMap[presentMap.mapHeight - 1][a->x] = HaliteLocation(myTag, true);
            } else if(section == 1) {
                moves.push_back(HaliteMove(HaliteMove::SOUTH, a->x, a->y));
                if(a->y != 0) presentMap.hMap[a->y - 1][a->x] = HaliteLocation(myTag, true);
    			else presentMap.hMap[presentMap.mapHeight - 1][a->x] = HaliteLocation(myTag, true);
            } else {
                moves.push_back(HaliteMove(HaliteMove::STILL, a->x, a->y));
                if(a->y != 0) presentMap.hMap[a->y - 1][a->x] = HaliteLocation(myTag, true);
    			else presentMap.hMap[presentMap.mapHeight - 1][a->x] = HaliteLocation(myTag, true);
            }
        }
		
	}
}


//DO NOT TOUCH THESE
DiffusionAI::DiffusionAI()
{
	myTag = 0;
}