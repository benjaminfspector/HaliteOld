#include "GameRun.h"

using namespace std;

list<HaliteMove> * getplayerMoves(unsigned int playerTag, HaliteMap m);

void initPast()
{
	cout << "What is the name of your file? Please include the file extension: ";
	cin >> filename;
	input.open(filename, ios_base::in);
	while(!input.is_open())
	{
		cout << "No file could be found with that name. Please that it is in the proper directory and reenter its name: ";
		cin >> filename;
		input.open(filename, ios_base::in);
	}
	string in;
	getline(input, in);
	if(in != "Ht")
	{
		cout << "There was a problem with the file the file.\n";
		close();
		system("pause");
		exit(0);
	}
	
}
bool renderPast()
{
	return false;
}

void init()
{
	playerNames = vector<string>(0);
	playerNames.push_back("Bob");
	playerNames.push_back("Alice");
	playerNames.push_back("Jim");
	playerNames.push_back("Fred");
	playerNames.push_back("Tim");
	playerNames.push_back("Jane");
	playerNames.push_back("Sam");

	filename = "";
	for(auto a = playerNames.begin(); a != playerNames.end(); a++) filename += *a;
	filename += ".hlt";

	myMap = HaliteMap(playerNames, MAP_WIDTH, MAP_HEIGHT);

	colorCodes = map<unsigned char, color>();
	colorCodes.insert(pair<unsigned char, color>(0, { 100, 100, 100 }));
	colorCodes.insert(pair<unsigned char, color>(1, { 255, 0, 0 }));
	colorCodes.insert(pair<unsigned char, color>(2, { 0, 255, 0 }));
	colorCodes.insert(pair<unsigned char, color>(3, { 0, 0, 255 }));
	colorCodes.insert(pair<unsigned char, color>(4, { 255, 255, 0 }));
	colorCodes.insert(pair<unsigned char, color>(5, { 255, 0, 255 }));
	colorCodes.insert(pair<unsigned char, color>(6, { 0, 255, 255 }));
	colorCodes.insert(pair<unsigned char, color>(7, { 255, 255, 255 }));
}
void close()
{
	if(output.is_open()) output.close();
	if(input.is_open()) input.close();
	for(unsigned int a = 0; a < playerMoves.size(); a++)
	{
		delete playerMoves[a];
	}
}

void initOutput()
{
	output.open(filename, ios_base::out);
	output << "Ht\n";
	output << myMap.mapWidth << " " << myMap.mapHeight << " " << myMap.numberOfPlayers << "\n";
	for(auto a = playerNames.begin(); a != playerNames.end(); a++)
	{
		output << *a << "\n";
	}
	output.close();
}
void doOutput(unsigned char lastResult)
{
	myMap.outputToFile(filename);
	output.open(filename, ios_base::app);
	if(lastResult != 0) output << "\n0";
}

void runPlayers()
{
	std::vector< future< list<HaliteMove> * > > moveThreads(myMap.numberOfPlayers);
	for(unsigned int a = 0; a < playerMoves.size(); a++)
	{
		delete playerMoves[a];
	}
	playerMoves.clear();
	for(unsigned int a = 0; a < moveThreads.size(); a++)
	{
		moveThreads[a] = async(&getplayerMoves, a+1, myMap);
	}
	for(unsigned int a = 0; a < moveThreads.size(); a++)
	{
		playerMoves.push_back(moveThreads[a].get());
	}
	std::cout << "At runPlayers, time #" << moveNumber << "\n";
	moveNumber++;
}
unsigned char calculateResults()
{
	myMap = myMap.calculateResults(&playerMoves);
	return myMap.findWinner();
}

void renderGame()
{
	glPointSize(10);
	glBegin(GL_POINTS);
	unsigned int aD = 0;
	for(auto a = myMap.hMap.begin(); a != myMap.hMap.end(); a++)
	{
		unsigned int bD = 0;
		for(auto b = a->begin(); b != a->end(); b++)
		{
			//if(b->owner != 0)
			{
				color c = colorCodes[b->owner];
				const GLubyte DIMMING_FACTOR = 3;
				if(!b->isSentient)
				{
					c.r /= DIMMING_FACTOR;
					c.g /= DIMMING_FACTOR;
					c.b /= DIMMING_FACTOR;
				}
				glColor3ub(c.r, c.g, c.b);
				glVertex2f(bD+0.5, aD+0.5);
			}
			bD++;
		}
		aD++;
	}
	glEnd();
}

list<HaliteMove> * getplayerMoves(unsigned int playerTag, HaliteMap m)
{
	list<HaliteMove> * answer = new list<HaliteMove> (0);
	for(unsigned char y = 0; y < m.hMap.size(); y++)
	{
		for(unsigned char x = 0; x < m.hMap[y].size(); x++)
		{
			//Sample program - simply HaliteMoves them all upwards. Not very effective.
			if(m.hMap[y][x].owner == playerTag && m.hMap[y][x].isSentient)
			{
				unsigned char m = rand() % 5;
				switch(m)
				{
				case 0:
					answer->push_back(HaliteMove(HaliteMove::STILL, x, y));
					break;
				case 1:
					answer->push_back(HaliteMove(HaliteMove::NORTH, x, y));
					break;
				case 2:
					answer->push_back(HaliteMove(HaliteMove::EAST, x, y));
					break;
				case 3:
					answer->push_back(HaliteMove(HaliteMove::SOUTH, x, y));
					break;
				case 4:
					answer->push_back(HaliteMove(HaliteMove::WEST, x, y));
				}
			}
		}
	}

	return answer;
}