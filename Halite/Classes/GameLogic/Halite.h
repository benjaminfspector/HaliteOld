#ifndef HALITE_H
#define HALITE_H

//#include <boost\asio.hpp>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <iostream>
#include <thread>
#include <future>

//For the time being we'll use sfml sockets, and then switch to asio later once I understand it.
#include <C:\freeglutMSVC\SFML-2.3.1\include\SFML\Network.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>

#include "hlt.h"
#include "OpenGL.h"
#include "Networking.h"

class Halite
{
private:
	hlt::Map game_map;
	unsigned short turn_number, number_of_players, last_turn_output;
	std::vector<std::string> player_names;
	std::map<unsigned char, hlt::Color> color_codes;
	std::vector<hlt::Map * > full_game;
	char age_of_sentient;
	std::vector<boost::interprocess::message_queue *> mapQueues;
	std::vector<boost::interprocess::message_queue *> movesQueues;
	std::vector< std::set<hlt::Move> > player_moves;

	unsigned char getNextFrame();
	void connectToPlayers();
	unsigned char getAgeOfSentient(unsigned short w, unsigned short h) { return pow(int(w)*h, 0.4); }
public:
	Halite();
	Halite(unsigned short w, unsigned short h);
	void init();
	std::string runGame();
	void confirmWithinGame(signed short& turnNumber);
	void render(short& turnNumber);
	void output();
	bool input(std::string filename, unsigned short& width, unsigned short& height);
	void getColorCodes();
};

#endif