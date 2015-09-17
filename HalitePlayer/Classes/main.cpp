<<<<<<< HEAD
#include "Agents\Random\Random.h"
#include "Agents\Basic\Basic.h"
#include "Agents\Still\Still.h"
#include "Test.h"
=======
#include <stdlib.h>
#include <time.h> 
#include <cstdlib>
#include <ctime>

#include "Agents/Random/Random.h"
#include "Agents/Basic/Basic.h"
>>>>>>> origin/master

int main()
{
	srand(time(NULL));

	std::string in;
	std::cout << "What type of agent would you like to play? Enter r for a random agent, b for a basic agent, and to for a test agent: ";
	while(true)
	{
		std::getline(std::cin, in);
		std::transform(in.begin(), in.end(), in.begin(), ::tolower);
		if(in == "r" || in == "b" || in == "t" || in == "s") break;
	}
	if(in == "r")
	{
		Random r = Random();
		r.run();
	}
	else if(in == "b")
	{
		Basic b = Basic();
		b.run();
	}
<<<<<<< HEAD
	else if(in == "t")
	{
		Test b = Test();
		b.run();
	}
	else if(in == "s")
	{
		Still s{};
		s.run();
	}
=======

>>>>>>> origin/master
	return 0;
}