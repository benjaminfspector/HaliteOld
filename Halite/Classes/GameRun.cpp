#include "GameRun.h"

using namespace std;

//Here are the player objects


Hybrid_AI Bob;
TestHybrid_AI Alice; //Jim, Fred, Tim;
//STD_AI_1 ;
//TestDiffusionAI2 Sam, John, Kate, Sara, Melissa;
//YOURNAME


// 5, 11, 5, 4, 4


//End here

void initColorCodes()
{
    srand(time(NULL));
	colorCodes = map<short, color>();
	colorCodes.insert(pair<short, color>(0, { 100, 100, 100 }));
	colorCodes.insert(pair<short, color>(1, { 255, 0, 0 }));
	colorCodes.insert(pair<short, color>(2, { 0, 255, 0 }));
	colorCodes.insert(pair<short, color>(3, { 0, 0, 255 }));
	colorCodes.insert(pair<short, color>(4, { 255, 255, 0 }));
	colorCodes.insert(pair<short, color>(5, { 255, 0, 255 }));
	colorCodes.insert(pair<short, color>(6, { 0, 255, 255 }));
	colorCodes.insert(pair<short, color>(7, { 255, 255, 255 }));
	colorCodes.insert(pair<short, color>(8, { 222, 184, 135 }));
	colorCodes.insert(pair<short, color>(9, { 255, 128, 128 }));
	colorCodes.insert(pair<short, color>(10, { 255, 165, 0 }));
}
void outputPlayerColorCodes()
{
	cout << "Here are the " << int(playerNames.size()) << " players' color codes: \n";
	for(short a = 0; a < playerNames.size(); a++) cout << "Player " << playerNames[a] << " has rgb { " << int(colorCodes[a+1].r) << ", " << int(colorCodes[a+1].g) << ", " << int(colorCodes[a+1].b) << " }\n";
	system("pause");
}

mapDimensions initPast()
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
	input >> mapWidth >> mapHeight >> numPlayers;
	playerNames = std::vector<string>(0);
	getline(input, in);
	for(short a = 0; a < numPlayers; a++) //Maybe still on last line? Check that as a possible error:
	{
		getline(input, in);
		playerNames.push_back(in);
	}
	outputPlayerColorCodes();
	theseMaps = vector< vector<torender> >(0, vector<torender>());
	return { mapWidth, mapHeight };
}
void getPast()
{
	bool answer;
	input >> answer;
	short gotten = 1;
	while(answer)
	{
		//if(gotten % 10 == 0)
		std::cout << "Gotten frame number " << gotten << "!\n";
		list<torender> thisMap = list<torender>();
		int runningTotalGotten = 0;
		while(runningTotalGotten < mapWidth * mapHeight)
		{
			int numberToRender;
			short value;
			bool sentience;
			input >> numberToRender >> value >> sentience;
			thisMap.push_back({ numberToRender, char(value), sentience });
			runningTotalGotten += numberToRender;
		}
		std::vector<torender> thisMapV = std::vector<torender>(thisMap.size());
		int aD = 0;
		for(auto a = thisMap.begin(); a != thisMap.end(); a++)
		{
			thisMapV[aD] = *a;
			aD++;
		}
		theseMaps.push_back(thisMapV);
		input >> answer;
		gotten++;
	}
}
void renderPast()
{
	if(moveNumber < 0)
	{
		moveNumber = 0;
	}
	else if(moveNumber > theseMaps.size() - 1)
	{
		moveNumber = theseMaps.size() - 1;
	}

	//Clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	//Take saved matrix off the stack and reset it
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glLoadIdentity();

	float xPointSize = glutGet(GLUT_WINDOW_WIDTH) / mapWidth;
	float yPointSize = glutGet(GLUT_WINDOW_HEIGHT) / mapHeight;
	const float decrementFactor = 0.6;
	float pointSize = std::min(xPointSize, yPointSize) * decrementFactor;
	glPointSize(pointSize);

	glBegin(GL_POINTS);
	short xLoc = 0, yLoc = 0;
	const GLubyte DIMMING_FACTOR = 3;
	for(auto a = theseMaps[moveNumber].begin(); a != theseMaps[moveNumber].end(); a++)
	{
		torender b = *a;
		color c = colorCodes[b.value];
		if(!b.sentience)
		{
			c.r /= DIMMING_FACTOR;
			c.g /= DIMMING_FACTOR;
			c.b /= DIMMING_FACTOR;
		}
		glColor3ub(c.r, c.g, c.b);
		while(b.num > 0)
		{
			glVertex2f(xLoc + 0.5, yLoc + 0.5);
			xLoc++;
			if(xLoc >= mapWidth)
			{
				xLoc = 0;
				yLoc++;
			}
			b.num--;
		}
	}
	glEnd();

	//Save default matrix again
	glPushMatrix();

	//Move to center of the screen
	glTranslatef(0.f, 0.f, 0.f);

	glutSwapBuffers();
}

void init(short width, short height)
{
    moveNumber = 0;
	srand(rand());

	playerNames = vector<string>(0);
	playerNames.push_back("Bob");
	playerNames.push_back("Alice");
	/*playerNames.push_back("Jim");
	playerNames.push_back("Fred");
	playerNames.push_back("Tim");
	playerNames.push_back("Sam");
	playerNames.push_back("John");
	playerNames.push_back("Kate");
	playerNames.push_back("Sara");
	playerNames.push_back("Melissa");*/

	mapWidth = width;
	mapHeight = height;
	myMap = HaliteMap(playerNames, mapWidth, mapHeight);

	filename = "";
	for(auto a = playerNames.begin(); a != playerNames.end(); a++) filename += (*a)[0];
	filename += "_";
	filename += to_string(mapWidth);
	filename += "_";
	filename += to_string(mapHeight);
	filename += ".hlt";

	//Put in player objects here:


	Bob = Hybrid_AI(1, myMap);
	Alice = TestHybrid_AI(2, myMap);
	/*Jim = YOURNAME(3, myMap);
	Fred = DiffusionAI(4, myMap);
	Tim = Hybrid_AI(5, myMap);
	Sam = TestDiffusionAI2(6, myMap);
	John = DiffusionAI(7, myMap);
	Kate = DiffusionAI(8, myMap);
	Sara = DiffusionAI(9, myMap);
	Melissa = DiffusionAI(10, myMap);*/
}
void close()
{
	if(output.is_open()) output.close();
	if(input.is_open()) input.close();
}

void initOutput()
{
	output.open(filename, ios_base::out);
	output << "Ht\n";
	output << int(myMap.mapWidth) << " " << int(myMap.mapHeight) << " " << int(myMap.numberOfPlayers) << "\n";
	for(auto a = playerNames.begin(); a != playerNames.end(); a++)
	{
		output << *a << "\n";
	}
	output.close();
}
void doOutput(short lastResult)
{
	myMap.outputToFile(filename);
	output.open(filename, ios_base::app);
	if(lastResult != 0) output << "0";
	output.close();
}

void runPlayers()
{
	vector<thread> moveThreads (myMap.numberOfPlayers);
	playerMoves.clear();

	//Create moveThreads: 

	moveThreads[0] = thread(&Hybrid_AI::threadPackage, &Bob, myMap);
	moveThreads[1] = thread(&TestHybrid_AI::threadPackage, &Alice, myMap);
	/*moveThreads[2] = thread(&YOURNAME::threadPackage, &Jim, myMap);
	moveThreads[3] = thread(&DiffusionAI::threadPackage, &Fred, myMap);
	moveThreads[4] = thread(&Hybrid_AI::threadPackage, &Tim, myMap);
	moveThreads[5] = thread(&Hybrid_AI::threadPackage, &Sam, myMap);
	moveThreads[6] = thread(&DiffusionAI::threadPackage, &John, myMap);
    moveThreads[7] = thread(&DiffusionAI::threadPackage, &Kate, myMap);
	moveThreads[8] = thread(&DiffusionAI::threadPackage, &Sara, myMap);
	moveThreads[9] = thread(&DiffusionAI::threadPackage, &Melissa, myMap);*/

	//Wait for completion.
	for(short a = 0; a < myMap.numberOfPlayers; a++)
	{
		moveThreads[a].join();
	}
	moveThreads.clear();

	//Add player's moves back to playerMoves
	playerMoves.push_back(&Bob.moves);
	playerMoves.push_back(&Alice.moves);
	/*playerMoves.push_back(&Jim.moves);
	playerMoves.push_back(&Fred.moves);
	playerMoves.push_back(&Tim.moves);
	playerMoves.push_back(&Sam.moves);
	playerMoves.push_back(&John.moves);
	playerMoves.push_back(&Kate.moves);
	playerMoves.push_back(&Sara.moves);
	playerMoves.push_back(&Melissa.moves);*/
	

	//if(moveNumber % 20 == 1)
		std::cout << "At runPlayers, time #" << moveNumber << "\n";
	moveNumber++;
}
short calculateResults()
{
	myMap = myMap.calculateResults(&playerMoves);
	return myMap.findWinner();
}

void renderGame()
{
	float xPointSize = glutGet(GLUT_WINDOW_WIDTH) / myMap.mapWidth;
	float yPointSize = glutGet(GLUT_WINDOW_HEIGHT) / myMap.mapWidth;
	const float decrementFactor = 0.6;
	float pointSize = std::min(xPointSize, yPointSize) * decrementFactor;
	glPointSize(pointSize);
	glBegin(GL_POINTS);
	unsigned int aD = 0;
	for(auto a = myMap.hMap.begin(); a != myMap.hMap.end(); a++)
	{
		unsigned int bD = 0;
		for(auto b = a->begin(); b != a->end(); b++)
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
			bD++;
		}
		aD++;
	}
	glEnd();
}

void runPresentAnalysis()
{
	struct capability { short numSentient, numFactory; };
	vector<capability> playerCapabilities(myMap.numberOfPlayers, { 0, 0 });
	for(auto a = myMap.hMap.begin(); a != myMap.hMap.end(); a++)
	{
		for(auto b = a->begin(); b != a->end(); b++)
		{
			if(b->owner != 0)
			{
				if(b->isSentient)
				{
					playerCapabilities[b->owner - 1].numSentient++;
				}
				else
				{
					playerCapabilities[b->owner - 1].numFactory++;
				}
			}
		}
	}

	for(short a = 0; a < myMap.numberOfPlayers; a++)
	{
		std::cout << "Player " << playerNames[a] << " has " << playerCapabilities[a].numSentient << " sentient crystals and " << playerCapabilities[a].numFactory << " factory crystals.\n";
	}

	system("pause");
}
void runPastAnalysis()
{
	struct capability { short numSentient, numFactory; };
	vector<capability> playerCapabilities(playerNames.size(), { 0, 0 });
	for(auto a = theseMaps[moveNumber].begin(); a != theseMaps[moveNumber].end(); a++)
	{
		if(a->value != 0)
		{
			if(a->sentience)
			{
				playerCapabilities[a->value - 1].numSentient += a->num;
			}
			else
			{
				playerCapabilities[a->value - 1].numFactory += a->num;
			}
		}
	}

	for(short a = 0; a < playerNames.size(); a++)
	{
		std::cout << "Player " << playerNames[a] << " has " << playerCapabilities[a].numSentient << " sentient crystals and " << playerCapabilities[a].numFactory << " factory crystals.\n";
	}

	system("pause");
}


//shh
int * getMoveNumberP() { return &moveNumber; };
std::vector<std::string> * getPlayerNames() { return &playerNames; };
//endshh
