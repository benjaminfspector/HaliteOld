#include "LEIGON_AI.h"

LEIGON_AI::LEIGON_AI(short givenTag, HaliteMap initialMap)
{
	lastDirection = rand() % 5;
	myTag = givenTag;
	probGoodOverBad = pow((initialMap.mapWidth * initialMap.mapHeight), 0.4);
	if(probGoodOverBad < 2) probGoodOverBad = 2;
}

//Random move AI
void LEIGON_AI::getMoves(HaliteMap * presentMap)
{
	moves.clear();

	//Find list of locations we need to move
	struct loc { short x, y; };
	std::list<loc> toMove;
	std::list<loc> nearestLocs;
	std::list<loc> singleSquares;
	for(short y = 0; y < presentMap->hMap.size(); y++)
	{
		for(short x = 0; x < presentMap->hMap[y].size(); x++)
		{
			if(presentMap->hMap[y][x].owner == myTag && presentMap->hMap[y][x].isSentient)
			{
				toMove.push_back({ x, y });
				presentMap->hMap[y][x].isSentient = false;
			}
			else if(presentMap->hMap[y][x].owner != myTag)
			{
				HaliteLocation n = presentMap->getNorthern(x, y), e = presentMap->getEastern(x, y), s = presentMap->getSouthern(x, y), w = presentMap->getWestern(x, y);
				if(n.owner == myTag || e.owner == myTag || s.owner == myTag || w.owner == myTag)
				{
					if(n.owner == myTag && e.owner == myTag && s.owner == myTag && w.owner == myTag)
					{
						if(!(n.isSentient || e.isSentient || s.isSentient || w.isSentient)) singleSquares.push_back({ x, y });
					}
					else nearestLocs.push_back({ x, y });
				}
			}
		}
	}

	for(auto a = toMove.begin(); !toMove.empty() && a != toMove.end();)
	{
		enum Goodness { BEST, GOOD, BAD };
		HaliteLocation around[8];
		around[0] = presentMap->getNorthern(a->x, a->y);
		around[1] = presentMap->getNortheastern(a->x, a->y);
		around[2] = presentMap->getEastern(a->x, a->y);
		around[3] = presentMap->getSoutheastern(a->x, a->y);
		around[4] = presentMap->getSouthern(a->x, a->y);
		around[5] = presentMap->getSouthwestern(a->x, a->y);
		around[6] = presentMap->getWestern(a->x, a->y);
		around[7] = presentMap->getNorthwestern(a->x, a->y);
		Goodness possibilities[4];
		for(short b = 0; b < 4; b++)
		{
			short bA = 2 * b;
			if(around[bA].owner != myTag && around[nextAround(bA)].owner != myTag && around[lastAround(bA)].owner != myTag) possibilities[b] = BEST;
			else if(around[bA].owner != myTag) possibilities[b] = GOOD;
			else possibilities[b] = BAD;
		}
		Goodness bestPossible = BAD;
		for(short b = 0; b < 4; b++)
		{
			if(possibilities[b] == GOOD)
			{
				bestPossible = GOOD;
			}
			else if(possibilities[b] == BEST)
			{
				bestPossible = BEST;
			}
		}

		bool goodOverBest = rand() % probGoodOverBad == 0;
		bool doIncrement = true;
		if(bestPossible == GOOD || (goodOverBest && bestPossible == BEST))
		{
			short initial = lastDirection;
			lastDirection++;
			if(lastDirection > 3) lastDirection = 0;
			while(possibilities[lastDirection] != GOOD)
			{
				if(lastDirection == initial) break;
				lastDirection++;
				if(lastDirection > 3) lastDirection = 0;
			}

			if(lastDirection == 0)
			{
				addNorth(a->x, a->y, presentMap);
			}
			else if(lastDirection == 1)
			{
				addEast(a->x, a->y, presentMap);
			}
			else if(lastDirection == 2)
			{
				addSouth(a->x, a->y, presentMap);
			}
			else if(lastDirection == 3)
			{
				addWest(a->x, a->y, presentMap);
			}
		}
		else if(bestPossible == BEST)
		{
			lastDirection++;
			if(lastDirection > 3) lastDirection = 0;
			while(possibilities[lastDirection] != BEST)
			{
				lastDirection++;
				if(lastDirection > 3) lastDirection = 0;
			}

			if(lastDirection == 0)
			{
				addNorth(a->x, a->y, presentMap);
			}
			else if(lastDirection == 1)
			{
				addEast(a->x, a->y, presentMap);
			}
			else if(lastDirection == 2)
			{
				addSouth(a->x, a->y, presentMap);
			}
			else if(lastDirection == 3)
			{
				addWest(a->x, a->y, presentMap);
			}
		}

		if(bestPossible == GOOD || bestPossible == BEST)
		{
			auto b = a;
			if(distance(toMove.begin(), a) != 0) a--;
			else doIncrement = false;
			toMove.erase(b);
		}

		if(doIncrement) a++;
		else a = toMove.begin();
	}

	for(auto a = singleSquares.begin(); a != singleSquares.end() && !toMove.empty(); a++)
	{
		double bestDistance = 1000000;
		std::list<loc>::iterator location;
		for(auto b = toMove.begin(); b != toMove.end(); b++)
		{
			double thisDist = presentMap->getDistance(a->x, a->y, b->x, b->y);
			if(thisDist < bestDistance)
			{
				location = b;
				bestDistance = thisDist;
			}
		}
		float angle = 2 * presentMap->getAngle(location->x, location->y, a->x, a->y) / M_PI;
		HaliteLocation around[5];
		around[0] = presentMap->getNorthern(location->x, location->y);
		around[1] = presentMap->getEastern(location->x, location->y);
		around[2] = presentMap->getSouthern(location->x, location->y);
		around[3] = presentMap->getWestern(location->x, location->y);
		around[4] = presentMap->hMap[location->y][location->x];
		int answer = round(angle);
		if(answer == 0 || answer == -0) answer = 1;
		else if(answer == 1) answer = 2;
		else if(answer == 2 || answer == -2) answer = 3;
		else if(answer == -1) answer = 0;
		else answer = 5;
		lastDirection = answer;
		while(around[lastDirection].isSentient)
		{
			lastDirection++;
			if(lastDirection > 4) lastDirection = 0;
			if(answer == lastDirection) break;
		}
		if(lastDirection == 0)
		{
			addNorth(location->x, location->y, presentMap);
		}
		else if(lastDirection == 1)
		{
			addEast(location->x, location->y, presentMap);
		}
		else if(lastDirection == 2)
		{
			addSouth(location->x, location->y, presentMap);
		}
		else if(lastDirection == 3)
		{
			addWest(location->x, location->y, presentMap);
		}
		else
		{
			addStill(location->x, location->y, presentMap);
		}

		toMove.erase(location);
	}

	for(auto a = toMove.begin(); a != toMove.end(); a++)
	{
		HaliteLocation around[5];
		around[0] = presentMap->getNorthern(a->x, a->y);
		around[1] = presentMap->getEastern(a->x, a->y);
		around[2] = presentMap->getSouthern(a->x, a->y);
		around[3] = presentMap->getWestern(a->x, a->y);
		around[4] = presentMap->hMap[a->y][a->x];
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
		float angle = 2 * presentMap->getAngle(a->x, a->y, location.x, location.y) / M_PI;
		int answer = round(angle);
		if(answer == 0 || answer == -0) answer = 1;
		else if(answer == 1) answer = 2;
		else if(answer == 2 || answer == -2) answer = 3;
		else if(answer == -1) answer = 0;
		else answer = 5;
		lastDirection = answer;
		while(around[lastDirection].isSentient)
		{
			lastDirection++;
			if(lastDirection > 4) lastDirection = 0;
			if(answer == lastDirection) break;
		}
		if(lastDirection == 0)
		{
			addNorth(a->x, a->y, presentMap);
		}
		else if(lastDirection == 1)
		{
			addEast(a->x, a->y, presentMap);
		}
		else if(lastDirection == 2)
		{
			addSouth(a->x, a->y, presentMap);
		}
		else if(lastDirection == 3)
		{
			addWest(a->x, a->y, presentMap);
		}
		else
		{
			addStill(a->x, a->y, presentMap);
		}
	}
}

void LEIGON_AI::addNorth(short x, short y, HaliteMap * map)
{
	moves.push_back(HaliteMove(HaliteMove::NORTH, x, y));
	if(y != 0) map->hMap[y - 1][x] = HaliteLocation(myTag, true);
	else map->hMap[map->mapHeight - 1][x] = HaliteLocation(myTag, true); 
}

void LEIGON_AI::addEast(short x, short y, HaliteMap * map)
{
	moves.push_back(HaliteMove(HaliteMove::EAST, x, y));
	if(x != map->mapWidth - 1) map->hMap[y][x + 1] = HaliteLocation(myTag, true);
	else map->hMap[y][0] = HaliteLocation(myTag, true);
}

void LEIGON_AI::addSouth(short x, short y, HaliteMap * map)
{
	moves.push_back(HaliteMove(HaliteMove::SOUTH, x, y));
	if(y != map->mapHeight - 1) map->hMap[y + 1][x] = HaliteLocation(myTag, true);
	else map->hMap[0][x] = HaliteLocation(myTag, true);
}

void LEIGON_AI::addWest(short x, short y, HaliteMap * map)
{
	moves.push_back(HaliteMove(HaliteMove::WEST, x, y));
	if(x != 0) map->hMap[y][x - 1] = HaliteLocation(myTag, true);
	else map->hMap[y][map->mapWidth - 1] = HaliteLocation(myTag, true);
}

void LEIGON_AI::addStill(short x, short y, HaliteMap * map)
{
	moves.push_back(HaliteMove(HaliteMove::STILL, x, y));
	map->hMap[y][x] = HaliteLocation(myTag, true);
}

short LEIGON_AI::nextAround(short num)
{
	if(num < 0 || num >= 7) return 0;
	return num + 1;
}

short LEIGON_AI::lastAround(short num)
{
	if(num <= 0 || num > 7) return 7;
	return num - 1;
}


//DO NOT TOUCH THESE
LEIGON_AI::LEIGON_AI()
{
	myTag = 0;
}

void LEIGON_AI::threadPackage(LEIGON_AI * ai, HaliteMap presentMap)
{
	ai->getMoves(&presentMap);
}