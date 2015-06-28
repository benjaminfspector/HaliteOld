#include "TestHybrid_AI.h"

TestHybrid_AI::TestHybrid_AI(short givenTag, HaliteMap initialMap)
{
    myTag = givenTag;
}

void TestHybrid_AI::getMoves(HaliteMap * presentMap)
{
	moves.clear();

	//Find list of HaliteLocations we need to move
	struct loc { short x, y; };
	std::list<loc> mySentients;
    std::list<loc> nearestLocs;
    std::list<loc> singleSquares;
	for(short y = 0; y < presentMap->hMap.size(); y++) {
		for(short x = 0; x < presentMap->hMap[y].size(); x++) {
			if(presentMap->hMap[y][x].owner == myTag && presentMap->hMap[y][x].isSentient) {
               mySentients.push_back({ x, y });
               presentMap->hMap[y][x].isSentient = false;
            } else if(presentMap->hMap[y][x].owner != myTag) {
                HaliteLocation n = presentMap->getNorthern(x, y), e = presentMap->getEastern(x, y), s = presentMap->getSouthern(x, y), w = presentMap->getWestern(x, y);
                if(n.owner == myTag || e.owner == myTag || s.owner == myTag || w.owner == myTag){
                    if(n.owner == myTag && e.owner == myTag && s.owner == myTag && w.owner == myTag) {
                        if(!(n.isSentient || e.isSentient || s.isSentient || w.isSentient)) singleSquares.push_back({ x, y });
                    } else nearestLocs.push_back({ x, y });
                }
            }
		}
	}
    
    for(auto a = singleSquares.begin(); a != singleSquares.end() && !mySentients.empty(); a++)
    {
        double bestDistance = 1000000;
        std::list<loc>::iterator location;
        for(auto b = mySentients.begin(); b != mySentients.end(); b++)
        {
            double thisDist = presentMap->getDistance(a->x, a->y, b->x, b->y);
            if(thisDist < bestDistance)
            {
                location = b;
                bestDistance = thisDist;
            }
        }
        if(bestDistance != 1000000) {
            float angle = presentMap->getAngle(location->x, location->y, a->x, a->y);
            moveWithAngle(angle, location->x, location->y, presentMap);
            mySentients.erase(location);
        }
    }
    
    
	for(auto a = mySentients.begin(); a != mySentients.end(); a++) {
		HaliteLocation around[4];
		around[0] = presentMap->getNorthern(a->x, a->y);
		around[1] = presentMap->getEastern(a->x, a->y);
		around[2] = presentMap->getSouthern(a->x, a->y);
		around[3] = presentMap->getWestern(a->x, a->y);
        
        
        bool foundBest = false;
		for(short b = 0; b < 4; b++) {
			if(around[b].owner != myTag) {
                foundBest = true;
                break;
			}
		}
        if(foundBest == true) {
            if(mySentients.size() < 2) {
                lastDirection++;
                if(lastDirection >= 4) lastDirection = 0;
                while(around[lastDirection].owner == myTag) {
                    lastDirection++;
                    if(lastDirection >= 4) lastDirection = 0;
                }
                
                if(lastDirection == 0) {
                    addNorth(a->x, a->y, presentMap);
                } else if(lastDirection == 1) {
                    addEast(a->x, a->y, presentMap);
                } else if(lastDirection == 2) {
                    addSouth(a->x, a->y, presentMap);
                } else if(lastDirection == 3) {
                    addWest(a->x, a->y, presentMap);
                }
            } else {
                
                float fieldX = 0;
                float fieldY = 0;
                
                for(auto b = mySentients.begin(); b != mySentients.end(); b++) {
                    int x = b->x;
                    int y = b->y;
                    if(a->x == x && a->y == y) continue;
                    
                    float distance = presentMap->getDistance(x, y, a->x, a->y);
                    if(distance > presentMap->mapHeight/4.0f) continue;
                    
                    float angle = presentMap->getAngle(a->x, a->y, x, y);
                    float mag = 1.0f / pow(distance, 2);
                    
                    fieldX += mag * cos(angle);
                    fieldY += mag * sin(angle);
                }
                float finalAngle = atan2(fieldY, fieldX);
                
                
                
                int answer = round(finalAngle / (3.141593f*0.5f));
                if(answer == 0 || answer == -0) answer = 3;
                else if(answer == 1) answer = 0;
                else if(answer == 2 || answer == -2) answer = 1;
                else if(answer == -1) answer = 2;
                int direction = answer;
                while(around[direction].owner == myTag)
                {
                    direction++;
                    if(direction >= 4) direction = 0;
                    if(answer == direction) break;
                }
                if(direction == 0)
                {
                    addNorth(a->x, a->y, presentMap);
                }
                else if(direction == 1)
                {
                    addEast(a->x, a->y, presentMap);
                }
                else if(direction == 2)
                {
                    addSouth(a->x, a->y, presentMap);
                }
                else if(direction == 3)
                {
                    addWest(a->x, a->y, presentMap);
                }
            }
        } else {
            double bestDistance = 1000000;
            loc location;
            for(auto b = nearestLocs.begin(); b != nearestLocs.end(); b++)
            {
                double thisDist = presentMap->getDistance(a->x, a->y, b->x, b->y);
                if(thisDist < bestDistance)
                {
                    location = *b;
                    bestDistance = thisDist;
                }
            }
            float angleToBoarder = presentMap->getAngle(a->x, a->y, location.x, location.y);
            moveWithAngle(angleToBoarder, a->x, a->y, presentMap);
            
            
            /*float section = round(finalAngle / (3.141593f*0.5f));
            if(section == 0 || section == -0) {
                addWest(a->x, a->y, presentMap);
            } else if(section == 1) {
                addNorth(a->x, a->y, presentMap);
            } else if(section == 2 || section == -2) {
                addEast(a->x, a->y, presentMap);
            } else if(section == -1) {
                addSouth(a->x, a->y, presentMap);
            }*/
        }
		
	}
}

void TestHybrid_AI::moveWithAngle(float angle, short x, short y, HaliteMap * map) {
    HaliteLocation around[5];
    around[0] = map->getNorthern(x, y);
    around[1] = map->getEastern(x, y);
    around[2] = map->getSouthern(x, y);
    around[3] = map->getWestern(x, y);
    around[4] = map->hMap[y][x];
    
    int answer = round(2*angle/M_PI);
    if(answer == 0 || answer == -0) answer = 1;
    else if(answer == 1) answer = 2;
    else if(answer == 2 || answer == -2) answer = 3;
    else if(answer == -1) answer = 0;
    else answer = 5;
    int direction = answer;
    while(around[direction].isSentient)
    {
        direction++;
        if(direction > 4) direction = 0;
        if(answer == direction) break;
    }
    if(direction == 0)
    {
        addNorth(x, y, map);
    }
    else if(direction == 1)
    {
        addEast(x, y, map);
    }
    else if(direction == 2)
    {
        addSouth(x, y, map);
    }
    else if(direction == 3)
    {
        addWest(x, y, map);
    }
    else
    {
        addStill(x, y, map);
    }
}

void TestHybrid_AI::addNorth(short x, short y, HaliteMap * map)
{
    moves.push_back(HaliteMove(HaliteMove::NORTH, x, y));
    if(y != 0) map->hMap[y - 1][x] = HaliteLocation(myTag, true);
    else map->hMap[map->mapHeight - 1][x] = HaliteLocation(myTag, true);
}

void TestHybrid_AI::addEast(short x, short y, HaliteMap * map)
{
    moves.push_back(HaliteMove(HaliteMove::EAST, x, y));
    if(x != map->mapWidth - 1) map->hMap[y][x + 1] = HaliteLocation(myTag, true);
    else map->hMap[y][0] = HaliteLocation(myTag, true);
}

void TestHybrid_AI::addSouth(short x, short y, HaliteMap * map)
{
    moves.push_back(HaliteMove(HaliteMove::SOUTH, x, y));
    if(y != map->mapHeight - 1) map->hMap[y + 1][x] = HaliteLocation(myTag, true);
    else map->hMap[0][x] = HaliteLocation(myTag, true);
}

void TestHybrid_AI::addWest(short x, short y, HaliteMap * map)
{
    moves.push_back(HaliteMove(HaliteMove::WEST, x, y));
    if(x != 0) map->hMap[y][x - 1] = HaliteLocation(myTag, true);
    else map->hMap[y][map->mapWidth - 1] = HaliteLocation(myTag, true);
}

void TestHybrid_AI::addStill(short x, short y, HaliteMap * map)
{
    moves.push_back(HaliteMove(HaliteMove::STILL, x, y));
    map->hMap[y][x] = HaliteLocation(myTag, true);
}


//DO NOT TOUCH THESE
TestHybrid_AI::TestHybrid_AI()
{
	myTag = 0;
}

void TestHybrid_AI::threadPackage(TestHybrid_AI * ai, HaliteMap presentMap)
{
	ai->getMoves(&presentMap);
}