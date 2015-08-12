#ifndef NETWORKING_H
#define NETWORKING_H

#include <SFML\Network.hpp>
#include <time.h>
#include <set>

#include "GameLogic\hlt.h"

static sf::Packet& operator<<(sf::Packet& p, const hlt::Map& m)
{
	p << m.map_width << m.map_height;
	for(auto a = m.contents.begin(); a != m.contents.end(); a++) for(auto b = a->begin(); b != a->end(); b++) p << b->owner << b->age;
	return p;
}

static double handleInitNetworking(sf::TcpSocket * s, unsigned char playerTag, unsigned char ageOfSentient, std::string name, hlt::Map& m)
{
	sf::Packet p;
	p << playerTag << ageOfSentient << m;
	s->send(p);
	std::string str = "Init Message sent to player " + name + "\n";
	std::cout << str;
	clock_t initialTime = clock();
	sf::Packet r;
	s->receive(r);
	str = "Init Message received from player " + name + "\n";
	std::cout << str;
	clock_t finalTime = clock() - initialTime;
	double timeElapsed = float(finalTime) / CLOCKS_PER_SEC;
	str = ""; unsigned char readChar;
	while(!r.endOfPacket())
	{
		r >> readChar;
		str += readChar;
	}
	if(str != "Done") return FLT_MAX;
	return timeElapsed;
}

static double handleFrameNetworking(sf::TcpSocket * s, hlt::Map& m, std::set<hlt::Move> * moves)
{
	sf::Packet p;
	p << m;
	s->send(p);
	clock_t initialTime = clock();
	sf::Packet r;
	s->receive(r);
	clock_t finalTime = clock() - initialTime;
	double timeElapsed = float(finalTime) / CLOCKS_PER_SEC;
	moves->clear();
	unsigned short x, y; unsigned char d;
	while(!r.endOfPacket())
	{
		r >> x >> y >> d;
		moves->insert({ { x, y }, d });
	}
	return timeElapsed;
}

#endif