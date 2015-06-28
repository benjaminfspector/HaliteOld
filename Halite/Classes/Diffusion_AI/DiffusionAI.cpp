#include "DiffusionAI.h"

DiffusionAI::DiffusionAI(short givenTag, HaliteMap initialMap)
{
    myTag = givenTag;
}

//Random move AI
void DiffusionAI::getMoves(HaliteMap presentMap)
{
	moves.clear();

	//Find list of HaliteLocations we need to move
	struct loc { short x, y; };
	std::list<loc> toMove;
	for(short y = 0; y < presentMap.hMap.size(); y++)
	{
		for(short x = 0; x < presentMap.hMap[y].size(); x++)
		{
			if(presentMap.hMap[y][x].owner == myTag && presentMap.hMap[y][x].isSentient)
			{
               toMove.push_back({ x, y });
               presentMap.hMap[y][x].isSentient = false;
			}
		}
	}

	//Move them:
	enum goodness { BEST, MID, BAD };
	for(auto a = toMove.begin(); a != toMove.end(); a++)
	{
		HaliteLocation around[4];
		around[0] = presentMap.getNorthern(a->x, a->y);
		around[1] = presentMap.getEastern(a->x, a->y);
		around[2] = presentMap.getSouthern(a->x, a->y);
		around[3] = presentMap.getWestern(a->x, a->y);
		
        
        bool foundBest = false;
       
        
		for(short b = 0; b < 4; b++)
		{
			if(around[b].owner != myTag) {
                foundBest = true;
                break;
			}
		}
        if(foundBest == true) {
           lastDirection++;
           if(lastDirection >= 4) lastDirection = 0;
           while(around[lastDirection].owner == myTag)
           {
              lastDirection++;
              if(lastDirection >= 4) lastDirection = 0;
           }

            if(lastDirection == 0)
        	{
    			moves.push_back(HaliteMove(HaliteMove::NORTH, a->x, a->y));
    			if(a->y != 0) presentMap.hMap[a->y - 1][a->x] = HaliteLocation(myTag, true);
    			else presentMap.hMap[presentMap.mapHeight - 1][a->x] = HaliteLocation(myTag, true);
    		}
    		else if(lastDirection == 1)
    		{
    			moves.push_back(HaliteMove(HaliteMove::EAST, a->x, a->y));
    			if(a->x != presentMap.mapWidth - 1) presentMap.hMap[a->y][a->x + 1] = HaliteLocation(myTag, true);
    			else presentMap.hMap[a->y][0] = HaliteLocation(myTag, true);
    		}
    		else if(lastDirection == 2)
    		{
    			moves.push_back(HaliteMove(HaliteMove::SOUTH, a->x, a->y));
    			if(a->y != presentMap.mapHeight - 1) presentMap.hMap[a->y + 1][a->x] = HaliteLocation(myTag, true);
    			else presentMap.hMap[0][a->x] = HaliteLocation(myTag, true);
    		}
    		else if(lastDirection == 3)
    		{
    			moves.push_back(HaliteMove(HaliteMove::WEST, a->x, a->y));
    			if(a->x != 0) presentMap.hMap[a->y][a->x - 1] = HaliteLocation(myTag, true);
    			else presentMap.hMap[a->y][presentMap.mapWidth - 1] = HaliteLocation(myTag, true);
    		}
    		
        } else {
            float fieldX = 0;
            float fieldY = 0;
            
            for(auto b = toMove.begin(); b != toMove.end(); b++)
            {
                int x = b->x;
				int y = b->y;
                if(a->x == x && a->y == y) continue;
                
                float distance = presentMap.getDistance(x, y, a->x, a->y);
                if(distance > presentMap.mapHeight/4.0f) continue;
                
                
                float angle = presentMap.getAngle(a->x, a->y, x, y);
    	        float mag = 1.0f / pow(distance, 2);
               
               //if(a->x == 0) std::cout << "angle on the side " << angle << std::endl;
               
		        fieldX += mag * cos(angle);
		        fieldY += mag * sin(angle);
            }
            float finalAngle = atan2(fieldY, fieldX);
            float section = round(finalAngle / (3.141593f*0.5f));
           //if(a->x == 0) std::cout << "section on the side " << section << std::endl;
            if(section == 0 || section == -0) {
                //if(a->x == 0) std::cout << "section on the side west \n";
                moves.push_back(HaliteMove(HaliteMove::WEST, a->x, a->y));
                if(a->x != 0) presentMap.hMap[a->y][a->x - 1] = HaliteLocation(myTag, true);
                else presentMap.hMap[a->y][presentMap.mapWidth - 1] = HaliteLocation(myTag, true);
            } else if(section == 1) {
                //if(a->x == 0) std::cout << "section on the side north \n";
                moves.push_back(HaliteMove(HaliteMove::NORTH, a->x, a->y));
                if(a->y != 0) presentMap.hMap[a->y - 1][a->x] = HaliteLocation(myTag, true);
                else presentMap.hMap[presentMap.mapHeight - 1][a->x] = HaliteLocation(myTag, true);
            } else if(section == 2 || section == -2) {
                //if(a->x == 0) std::cout << "section on the side east \n";
                moves.push_back(HaliteMove(HaliteMove::EAST, a->x, a->y));
                if(a->x != presentMap.mapWidth - 1) presentMap.hMap[a->y][a->x + 1] = HaliteLocation(myTag, true);
                else presentMap.hMap[a->y][0] = HaliteLocation(myTag, true);
            } else if(section == -1) {
                //if(a->x == 0) std::cout << "section on the side south \n";
                moves.push_back(HaliteMove(HaliteMove::SOUTH, a->x, a->y));
                if(a->y != presentMap.mapHeight - 1) presentMap.hMap[a->y + 1][a->x] = HaliteLocation(myTag, true);
                else presentMap.hMap[0][a->x] = HaliteLocation(myTag, true);
            }
        }
		
	}
}


//DO NOT TOUCH THESE
DiffusionAI::DiffusionAI()
{
	myTag = 0;
}