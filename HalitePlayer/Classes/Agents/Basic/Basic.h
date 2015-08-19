#ifndef BASIC_H
#define BASIC_H

#include <time.h>
#include <set>
#include <stdlib.h>

#include "Networking.h"
#include "hlt.h"

class Basic
{
private:
	unsigned char my_tag;
	unsigned char age_of_sentient;
	hlt::Map present_map;
	std::set<hlt::Move> moves;
public:
	Basic();
	void run();
};

#endif