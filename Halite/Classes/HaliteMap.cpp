#include "HaliteMap.h"

using namespace std;

void HaliteMap::clearSentient()
{
	for(auto a = hMap.begin(); a != hMap.end(); a++)
	{
		for(auto b = a->begin(); b != a->end(); b++)
		{
			b->isSentient = false;
		}
	}
}

bool HaliteMap::inBounds(unsigned char xx, unsigned char yy)
{
	return xx >= 0 && yy >= 0 && xx < mapWidth && yy < mapHeight;
}

void HaliteMap::outputToFile(string fileName)
{
	std::fstream out;
	out.open(fileName, ios_base::app);
	out << "\n1\n";
	struct vecLoc { unsigned char x, y; };
	vecLoc presentPosition = { 0, 0 };
	unsigned char lastValue = 255, numWrite = 0;
	bool goOn = true, isSentient = false;
	while(goOn)
	{
		if(numWrite == 0)
		{
			numWrite = 1;
			lastValue = hMap[presentPosition.y][presentPosition.x].owner;
			isSentient = hMap[presentPosition.y][presentPosition.x].isSentient;
		}
		else if(hMap[presentPosition.y][presentPosition.x].owner == lastValue && isSentient == hMap[presentPosition.y][presentPosition.x].isSentient && numWrite != 255) numWrite++;
		else
		{
			out << numWrite << lastValue << isSentient;
			numWrite = 1;
			lastValue = hMap[presentPosition.y][presentPosition.x].owner;
			isSentient = hMap[presentPosition.y][presentPosition.x].isSentient;
		}

		presentPosition.x++;
		if(presentPosition.x >= mapWidth)
		{
			presentPosition.x = 0;
			presentPosition.y++;
			if(presentPosition.y >= mapHeight) goOn = false;
		}
	}
	if(numWrite != 0)
	{
		out << numWrite << lastValue << isSentient;
	}
	out.close();
}

float HaliteMap::getDistance(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2)
{
	unsigned char dx = abs(x2 - x1), dy = abs(y2 - y1);
	if(dx > mapWidth / 2) dx = mapWidth - dx;
	if(dy > mapHeight / 2) dy = mapHeight - dy;
	return sqrt((dx*dx) + (dy*dy));
}

HaliteLocation HaliteMap::getNorthern(unsigned char xx, unsigned char yy)
{
	if(yy != 0) return hMap[yy - 1][xx];
	return hMap[mapHeight - 1][xx];
}

HaliteLocation HaliteMap::getEastern(unsigned char xx, unsigned char yy)
{
	if(xx != mapWidth - 1) return hMap[yy][xx + 1];
	return hMap[yy][0];
}

HaliteLocation HaliteMap::getSouthern(unsigned char xx, unsigned char yy)
{
	if(yy != mapHeight - 1) return hMap[yy + 1][xx];
	return hMap[0][xx];
}

HaliteLocation HaliteMap::getWestern(unsigned char xx, unsigned char yy)
{
	if(xx != 0) return hMap[yy][xx - 1];
	return hMap[yy][mapWidth - 1];
}

bool HaliteMap::haveNeighbors(unsigned char x, unsigned char y, unsigned char comparisonTag)
{
	HaliteLocation n = getNorthern(x, y), e = getEastern(x, y), s = getSouthern(x, y), w = getWestern(x, y);
	return (n.isSentient && n.owner != comparisonTag) || (e.isSentient && e.owner != comparisonTag) || (s.isSentient && s.owner != comparisonTag) || (w.isSentient && w.owner != comparisonTag);

}

bool HaliteMap::haveNeighbors(unsigned char x, unsigned char y)
{
	HaliteLocation n = getNorthern(x, y), e = getEastern(x, y), s = getSouthern(x, y), w = getWestern(x, y);
	unsigned char comparisonTag = hMap[y][x].owner;
	return (n.isSentient && n.owner != comparisonTag) || (e.isSentient && e.owner != comparisonTag) || (s.isSentient && s.owner != comparisonTag) || (w.isSentient && w.owner != comparisonTag);
}

HaliteMap HaliteMap::calculateResults(vector< list<HaliteMove> * > * playerMoves)
{
	//List of squares to be deleted.
	list< pair<unsigned char, unsigned char> > toDelete(0); //std::pair<x, y>

	//List of punishments to dole out.
	vector<unsigned short> punishments(numberOfPlayers, 0);

	//Create second map. No soldiers on it, but colored tiles remain.
	HaliteMap myDual = *this;
	myDual.clearSentient();

	//HaliteMove pieces around.
	for(auto a = playerMoves->begin(); a != playerMoves->end(); a++)
	{
		for(auto b = (*a)->begin(); b != (*a)->end(); b++)
		{
			unsigned char thisOwner = distance(playerMoves->begin(), a) + 1;
			if(inBounds(b->x, b->y) && hMap[b->y][b->x].owner == thisOwner && hMap[b->y][b->x].isSentient)
			{
				if(b->moveType == HaliteMove::STILL)
				{
					if(!myDual.hMap[b->y][b->x].isSentient)
					{
						myDual.hMap[b->y][b->x] = hMap[b->y][b->x];
					}
					else if(myDual.hMap[b->y][b->x].owner != thisOwner)
					{
						//Mark square for deletion.
						toDelete.push_back(pair<unsigned char, unsigned char>(b->x, b->y));
						punishments[thisOwner - 1]++;
					}
				}
				else if(b->moveType == HaliteMove::NORTH)
				{
					if(b->y != 0)
					{
						if(!myDual.hMap[b->y - 1][b->x].isSentient)
						{
							myDual.hMap[b->y - 1][b->x] = hMap[b->y][b->x];
						}
						else if(myDual.hMap[b->y - 1][b->x].owner != thisOwner)
						{
							//Mark square for deletion.
							toDelete.push_back(pair<unsigned char, unsigned char>(b->x, b->y - 1));
							punishments[thisOwner - 1]++;
						}
					}
					else
					{
						if(!myDual.hMap[mapHeight - 1][b->x].isSentient)
						{
							myDual.hMap[mapHeight - 1][b->x] = hMap[b->y][b->x];
						}
						else if(myDual.hMap[mapHeight - 1][b->x].owner != thisOwner)
						{
							//Mark square for deletion.
							toDelete.push_back(pair<unsigned char, unsigned char>(b->x, mapHeight - 1));
							punishments[thisOwner - 1]++;
						}
					}
				}
				else if(b->moveType == HaliteMove::EAST)
				{
					if(b->x != mapWidth - 1)
					{
						if(!myDual.hMap[b->y][b->x + 1].isSentient)
						{
							myDual.hMap[b->y][b->x + 1] = hMap[b->y][b->x];
						}
						else if(myDual.hMap[b->y][b->x + 1].owner != thisOwner)
						{
							//Mark square for deletion.
							toDelete.push_back(pair<unsigned char, unsigned char>(b->x + 1, b->y));
							punishments[thisOwner - 1]++;
						}
					}
					else
					{
						if(!myDual.hMap[b->y][0].isSentient)
						{
							myDual.hMap[b->y][0] = hMap[b->y][b->x];
						}
						else if(myDual.hMap[b->y][0].owner != thisOwner)
						{
							//Mark square for deletion.
							toDelete.push_back(pair<unsigned char, unsigned char>(0, b->y));
							punishments[thisOwner - 1]++;
						}
					}
				}
				else if(b->moveType == HaliteMove::SOUTH)
				{
					if(b->y != mapHeight - 1)
					{
						if(!myDual.hMap[b->y + 1][b->x].isSentient)
						{
							myDual.hMap[b->y + 1][b->x] = hMap[b->y][b->x];
						}
						else if(myDual.hMap[b->y + 1][b->x].owner != thisOwner)
						{
							//Mark square for deletion.
							toDelete.push_back(pair<unsigned char, unsigned char>(b->x, b->y + 1));
							punishments[thisOwner - 1]++;
						}
					}
					else
					{
						if(!myDual.hMap[0][b->x].isSentient)
						{
							myDual.hMap[0][b->x] = hMap[b->y][b->x];
						}
						else if(myDual.hMap[0][b->x].owner != thisOwner)
						{
							//Mark square for deletion.
							toDelete.push_back(pair<unsigned char, unsigned char>(b->x, 0));
							punishments[thisOwner - 1]++;
						}
					}
				}
				else if(b->moveType == HaliteMove::WEST)
				{
					if(b->x != 0)
					{
						if(!myDual.hMap[b->y][b->x - 1].isSentient)
						{
							myDual.hMap[b->y][b->x - 1] = hMap[b->y][b->x];
						}
						else if(myDual.hMap[b->y][b->x - 1].owner != thisOwner)
						{
							//Mark square for deletion.
							toDelete.push_back(pair<unsigned char, unsigned char>(b->x - 1, b->y));
							punishments[thisOwner - 1]++;
						}
					}
					else
					{
						if(!myDual.hMap[b->y][mapWidth - 1].isSentient)
						{
							myDual.hMap[b->y][mapWidth - 1] = hMap[b->y][b->x];
						}
						else if(myDual.hMap[b->y][mapWidth - 1].owner != thisOwner)
						{
							//Mark square for deletion.
							toDelete.push_back(pair<unsigned char, unsigned char>(mapWidth - 1, b->y));
							punishments[thisOwner - 1]++;
						}
					}
				}
				else
				{
					//http://c11.img-up.net/jackie-cha18c3.jpg
				}
				hMap[b->y][b->x] = HaliteLocation();
			}
		}
	}

	//Delete squares which have been marked for deletion
	if(toDelete.size() != 0) for(auto a = toDelete.begin(); a != toDelete.end(); a++)
	{
		if(myDual.hMap[a->second][a->first].owner != 0) punishments[myDual.hMap[a->second][a->first].owner - 1]++;
		myDual.hMap[a->second][a->first] = HaliteLocation();
	}

	//Mark adjacent tiles for deletion. Make checkerboard pattern.
	for(unsigned char a = 0; a < myDual.hMap.size(); a++)
	{
		unsigned char b;
		if(a % 2 == 0) b = 0;
		else b = 1;
		while(b < myDual.hMap[a].size())
		{
			if(myDual.hMap[a][b].isSentient)
			{
				unsigned char thisOwner = myDual.hMap[a][b].owner;
				if(a == mapHeight - 1)
				{
					if(myDual.hMap[a - 1][b].owner !=  thisOwner && myDual.hMap[a - 1][b].isSentient)
					{
						toDelete.push_back(pair<unsigned char, unsigned char>(b, a));
						toDelete.push_back(pair<unsigned char, unsigned char>(b, a - 1));
					}
					if(myDual.hMap[0][b].owner !=  thisOwner && myDual.hMap[0][b].isSentient)
					{
						toDelete.push_back(pair<unsigned char, unsigned char>(b, a));
						toDelete.push_back(pair<unsigned char, unsigned char>(b, 0));
					}
				}
				else if(a == 0)
				{
					if(myDual.hMap[a + 1][b].owner !=  thisOwner && myDual.hMap[a + 1][b].isSentient)
					{
						toDelete.push_back(pair<unsigned char, unsigned char>(b, a));
						toDelete.push_back({ b, a + 1 });
					}
					if(myDual.hMap[mapHeight - 1][b].owner !=  thisOwner && myDual.hMap[mapHeight - 1][b].isSentient)
					{
						toDelete.push_back(pair<unsigned char, unsigned char>(b, a));
						toDelete.push_back(pair<unsigned char, unsigned char>(b, mapHeight - 1));
					}
				}
				else
				{
					if(myDual.hMap[a + 1][b].owner !=  thisOwner && myDual.hMap[a + 1][b].isSentient)
					{
						toDelete.push_back(pair<unsigned char, unsigned char>(b, a));
						toDelete.push_back(pair<unsigned char, unsigned char>(b, a + 1));
					}
					if(myDual.hMap[a - 1][b].owner !=  thisOwner && myDual.hMap[a - 1][b].isSentient)
					{
						toDelete.push_back(pair<unsigned char, unsigned char>(b, a));
						toDelete.push_back(pair<unsigned char, unsigned char>(b, a - 1));
					}
				}

				if(b == mapWidth - 1)
				{
					if(myDual.hMap[a][b - 1].owner !=  thisOwner && myDual.hMap[a][b - 1].isSentient)
					{
						toDelete.push_back(pair<unsigned char, unsigned char>(b, a));
						toDelete.push_back(pair<unsigned char, unsigned char>(b - 1, a));
					}
					if(myDual.hMap[a][0].owner !=  thisOwner && myDual.hMap[a][0].isSentient)
					{
						toDelete.push_back(pair<unsigned char, unsigned char>(b, a));
						toDelete.push_back(pair<unsigned char, unsigned char>(0, a));
					}
				}
				else if(b == 0)
				{
					if(myDual.hMap[a][b + 1].owner !=  thisOwner && myDual.hMap[a][b + 1].isSentient)
					{
						toDelete.push_back(pair<unsigned char, unsigned char>(b, a));
						toDelete.push_back(pair<unsigned char, unsigned char>(b + 1, a));
					}
					if(myDual.hMap[a][mapWidth - 1].owner !=  thisOwner && myDual.hMap[a][mapWidth - 1].isSentient)
					{
						toDelete.push_back(pair<unsigned char, unsigned char>(b, a));
						toDelete.push_back(pair<unsigned char, unsigned char>(mapWidth - 1, a));
					}
				}
				else
				{
					if(myDual.hMap[a][b - 1].owner !=  thisOwner && myDual.hMap[a][b - 1].isSentient)
					{
						toDelete.push_back(pair<unsigned char, unsigned char>(b, a));
						toDelete.push_back(pair<unsigned char, unsigned char>(b - 1, a));
					}
					if(myDual.hMap[a][b + 1].owner !=  thisOwner && myDual.hMap[a][b + 1].isSentient)
					{
						toDelete.push_back(pair<unsigned char, unsigned char>(b, a));
						toDelete.push_back(pair<unsigned char, unsigned char>(b + 1, a));
					}
				}
			}
			b += 2;
		}
	}

	//Delete squares which have been marked for deletion
	for(auto a = toDelete.begin(); a != toDelete.end(); a++)
	{
		if(myDual.hMap[a->second][a->first].owner != 0) punishments[myDual.hMap[a->second][a->first].owner - 1]++;
		myDual.hMap[a->second][a->first] = HaliteLocation();
	}

	//Generate new soldiers
	const unsigned short SPAWN_PROBABILITY = 200; // 1/SPAWN_PROBABILITY
	for(auto a = myDual.hMap.begin(); a != myDual.hMap.end(); a++)
	{
		for(auto b = a->begin(); b != a->end(); b++)
		{
			if(!b->isSentient && b->owner != 0)
			{
				if(rand() % SPAWN_PROBABILITY == 0)
				{
					b->isSentient = true;
				}
			}
		}
	}

	return myDual;
}

unsigned char HaliteMap::findWinner()
{
	std::vector<bool> stillAlive(numberOfPlayers);
	for(auto a = hMap.begin(); a != hMap.end(); a++)
	{
		for(auto b = a->begin(); b != a->end(); b++)
		{
			if(b->owner != 0) stillAlive[b->owner - 1] = true;
		}
	}

	unsigned char aliveCount = 0, aliveTag = 0;
	for(auto a = stillAlive.begin(); a != stillAlive.end(); a++)
	{
		if(*a)
		{
			aliveCount++;
			if(aliveCount >= 2) return 0;
			aliveTag = distance(stillAlive.begin(), a) + 1;
		}
	}
	return aliveTag;
}

HaliteMap::HaliteMap()
{
	mapWidth = 0;
	mapHeight = 0;
	hMap = vector< vector<HaliteLocation> >(mapHeight, vector<HaliteLocation>(mapWidth, HaliteLocation()));
	playerNames = vector<string>(0);
	numberOfPlayers = 0;
}

HaliteMap::HaliteMap(vector<string> pNames, unsigned char mWidth, unsigned char mHeight)
{
	playerNames = pNames;
	numberOfPlayers = playerNames.size();
	mapWidth = mWidth;
	mapHeight = mHeight;
	hMap = vector< vector<HaliteLocation> >(mapHeight, vector<HaliteLocation>(mapWidth, HaliteLocation()));

	//Scatter some colored squares.
	list<pair<unsigned char, unsigned char> > takenSpots(0);
	float minDistance = sqrt(mapHeight*mapWidth) / 2;
	for(int a = 1; a <= numberOfPlayers; a++)
	{
		bool bad = true;
		int counter = 0;
		unsigned char xPos, yPos;
		while(bad)
		{
			bad = false;
			xPos = rand() % mapWidth;
			yPos = rand() % mapHeight;
			for(auto b = takenSpots.begin(); b != takenSpots.end(); b++)
			{
				if(getDistance(xPos, yPos, b->first, b->second) <= minDistance)
				{
					bad = true;
					break;
				}
			}
			counter++;
			if(counter > 150)
			{
				counter = 0;
				minDistance *= 0.85;
			}
		}
		hMap[yPos][xPos] = HaliteLocation(a, true);
		takenSpots.push_back(pair<unsigned char, unsigned char>(xPos, yPos));
	}
}