#include "Engine/Math/Randomizer.hpp"
#include <ctime>

#include <cmath>

Randomizer g_theRandomizer;

Randomizer::Randomizer()
	{
	int timeAsInt = (int)time(nullptr);
	srand(timeAsInt);
	}

int Randomizer::nextInt()
	{
	return rand();
	}

