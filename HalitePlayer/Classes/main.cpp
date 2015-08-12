#include "Agents\Random\Random.h"
#include "Agents\Basic\Basic.h"

int main()
{
	bool r;
	std::cin >> r;
	if(r)
	{
		Random r = Random();
		r.run();
	}
	else
	{
		Basic b = Basic();
		b.run();
	}
	return 0;
}