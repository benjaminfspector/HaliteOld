#ifndef STILL_H
#define STILL_H

#include <time.h>
#include <set>

#include "Networking.h"
#include "hlt.h"

class Still
{
private:
	unsigned char my_tag;
	unsigned char age_of_sentient;
	sf::TcpSocket * connection;
	hlt::Map present_map;
	std::set<hlt::Move> moves;
public:
	Still();
	void run();
};

#endif