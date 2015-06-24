#ifndef GAMERUN_H
#define GAMERUN_H

#include "HaliteMap.h"
#include "LUtil.h"

#include "AI_Template\YourName.h"
#include "STD_AI_1\STD_AI_1.h"
//#include "Diffusion_AI\DiffusionAI.h"

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
static unsigned short mapWidth, mapHeight, numPlayers;

//Variables for game creation

static HaliteMap myMap;
static std::vector< std::list<HaliteMove> * > playerMoves;
static std::fstream output;


/////////////////////////////////////////////////////////////////////////////////////////////

void initColorCodes();
void outputPlayerColorCodes();

struct mapDimensions { unsigned short w, h; };
mapDimensions initPast();
bool getPast();
void renderPast();

void init(unsigned short width, unsigned short height);
void close();

void initOutput();
void doOutput(unsigned char lastResult);

void runPlayers();
unsigned char calculateResults(); /* 0 indicates continuation; other indicates playerTag */

void renderGame();

void runPresentAnalysis();
void runPastAnalysis();

#endif