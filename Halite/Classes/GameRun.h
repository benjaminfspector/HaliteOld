#ifndef GAMERUN_H
#define GAMERUN_H

#include "HaliteMap.h"
#include "LUtil.h"

#include "YourName.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <time.h>
#include <math.h>
#include <thread>
#include <algorithm>
#include <map>

//Temporary Constants
static const unsigned char MAP_HEIGHT = 50;
static const unsigned char MAP_WIDTH = 50;

struct color
{
	GLubyte r, g, b;
};

//Variables for everything
static int moveNumber = 0;
static std::vector<std::string> playerNames;
static std::string filename;
static std::map<unsigned char, color> colorCodes; //Make 0.8 times as bright when unoccupied.

//Variables for reading in file
struct torender { unsigned short num; char value; bool sentience; };
static std::list<torender> thisMap;
static std::fstream input;
static unsigned char mapWidth, mapHeight, numPlayers;

//Variables for game creation

static HaliteMap myMap;
static std::vector< std::list<HaliteMove> * > playerMoves;
static std::fstream output;


/////////////////////////////////////////////////////////////////////////////////////////////

void initColorCodes();
void outputPlayerColorCodes();

void initPast();
bool getPast();
void renderPast();

void init();
void close();

void initOutput();
void doOutput(unsigned char lastResult);

void runPlayers();
unsigned char calculateResults(); /* 0 indicates continuation; other indicates playerTag */

void renderGame();

void runPresentAnalysis();
void runPastAnalysis();

#endif