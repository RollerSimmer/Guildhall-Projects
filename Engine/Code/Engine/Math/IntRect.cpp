#include "Engine/Math/IntRect.hpp"

IntRect::IntRect(int left,int top,int right,int bottom)
	:	nw(left,top)
	,	se(right,bottom)
	{

	}

IntVector2 IntRect::calcSize()
	{
	IntVector2 result(nw-se);
	return result;
	}
