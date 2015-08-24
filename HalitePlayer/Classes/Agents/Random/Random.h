#ifndef RANDOM_H
#define RANDOM_H

#include <time.h>
#include <set>
#include <stdlib.h>

#include "../../Networking.h"
#include "hlt.h"

class Random
{
private:
	unsigned char my_tag;
	unsigned char age_of_sentient;
	hlt::Map present_map;
	MoveSet *moves;
public:
	Random();
	void run();
};

#endif