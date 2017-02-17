#pragma once
#include "Engine/Math/IntVector2.hpp"

struct IntRect
	{
	IntRect(int left,int top,int right,int bottom);
	IntVector2 calcSize();

	IntVector2 nw;
	IntVector2 se;
	};

