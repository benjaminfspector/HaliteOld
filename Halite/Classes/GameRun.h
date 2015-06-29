#ifndef GAMERUN_H
#define GAMERUN_H

#include "HaliteMap.h"
#include "LUtil.h"

#include "AI_Template/YourName.h"
#include "STD_AI_1/STD_AI_1.h"
#include "Diffusion_AI/DiffusionAI.h"
#include "Hybrid_AI/Hybrid_AI.h"
#include "Hybrid_AI/TestHybrid_AI.h"
#include "Leigon_AI/Leigon_AI.h"

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
static std::map<short, color> colorCodes; //Make 0.8 times as bright when unoccupied.

//Variables for reading in file
struct torender { int num; char value; bool sentience; };
static std::vector< std::vector<torender> > theseMaps;
static std::fstream input;
static short mapWidth, mapHeight, numPlayers;

//Variables for game creation

static HaliteMap myMap;
static std::vector< std::list<HaliteMove> * > playerMoves;
static std::fstream output;


/////////////////////////////////////////////////////////////////////////////////////////////

void initColorCodes();
void outputPlayerColorCodes();

struct mapDimensions { short w, h; };
mapDimensions initPast();
void getPast();
void renderPast();

void initPlayerNames();
void init(short width, short height);
void close();

void initOutput();
void doOutput(short lastResult);

void runPlayers();
short calculateResults(); /* 0 indicates continuation; other indicates playerTag */

void renderGame();

void runPresentAnalysis();
void runPastAnalysis();


//shh
int * getMoveNumberP();
std::vector<std::string> * getPlayerNames();
//endshh

#endif