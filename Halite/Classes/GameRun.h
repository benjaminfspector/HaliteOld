#ifndef GAMERUN_H
#define GAMERUN_H

#include "HaliteMap.h"
#include "LUtil.h"

#include <iostream> //Temporary, for debugging
#include <fstream> //Temporary, for debugging
#include <string> //Temporary, for debugging

#include <vector>
#include <list>
#include <time.h>
#include <math.h>
#include <thread>
#include <future>
#include <map>

//Temporary Constants
static const unsigned char MAP_HEIGHT = 20;
static const unsigned char MAP_WIDTH = 20;

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
static std::fstream input;

//Variables for game creation

static HaliteMap myMap;
static std::vector< std::list<HaliteMove> * > playerMoves;
static std::fstream output;


/////////////////////////////////////////////////////////////////////////////////////////////

void initPast();
bool renderPast();

void init();
void close();

void initOutput();
void doOutput(unsigned char lastResult);

void runPlayers();
unsigned char calculateResults(); /* 0 indicates continuation; other indicates playerTag */

void renderGame();

#endif