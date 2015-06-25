#include "SHIFT_AI.h"

SHIFT_AI::SHIFT_AI(short givenTag, HaliteMap initialMap)
{
	presentDirection = rand()%5;
	myTag = givenTag;
}

//Random move AI
void SHIFT_AI::getMoves(HaliteMap presentMap)
{
	moves.clear();
	
	short numberOfSquares = 0;

	//Find list of locations we need to move
	struct loc { short x, y; };
	std::list<loc> toMove;
	for(short y = 0; y < presentMap.hMap.size(); y++)
	{
		for(short x = 0; x < presentMap.hMap[y].size(); x++)
		{
			if(presentMap.hMap[y][x].owner == myTag)
			{
				if(presentMap.hMap[y][x].isSentient)
				{
					toMove.push_back({ x, y });
					presentMap.hMap[y][x].isSentient = false;
				}
				numberOfSquares++;
			}
		}
	}
	
	double numOccupied = numberOfSquares/(mapWidth * mapHeight);
	double turtlefactor = 4*MAX_TURTLE_FACTOR*(numOccupied - (numOccupied * numOccupied));
	
	if(toMove.size() / turtlefactor < numberOfSquares)
	{
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
			for(short b = 0; b < 5; b++)
			{
				if(around[b].owner != myTag) possibilities[b] = BEST;
				else if(!around[b].isSentient) possibilities[b] = MID;
				else possibilities[b] = BAD;
			}
	
			goodness bestPossible = BAD;
			for(short b = 0; b < 5; b++)
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
	else
	{
		for(auto a = toMove.begin(); a != toMove.end(); a++)
		{
			HaliteLocation around[5];
			around[0] = presentMap.getNorthern(a->x, a->y);
			around[1] = presentMap.getEastern(a->x, a->y);
			around[2] = presentMap.getSouthern(a->x, a->y);
			around[3] = presentMap.getWestern(a->x, a->y);
			around[4] = presentMap.hMap[a->y][a->x];
			
	        
	        bool foundBest = false;
	       
	        
			for(short b = 0; b < 5; b++)
			{
				if(around[b].owner != myTag) {
	                foundBest = true;
	                break;
				}
			}
	        if(foundBest == true) {
	           lastDirection++;
	           if(lastDirection >= 5) lastDirection = 0;
	           while(around[lastDirection].owner == myTag)
	           {
	              lastDirection++;
	              if(lastDirection >= 5) lastDirection = 0;
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
	               
	                float angle = presentMap.getAngle(a->x, a->y, x, y);
	    	        float mag = 1.0f / pow(presentMap.getDistance(x, y, a->x, a->y), 2);
	               
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
}


//DO NOT TOUCH THESE
SHIFT_AI::SHIFT_AI()
{
	myTag = 0;
}