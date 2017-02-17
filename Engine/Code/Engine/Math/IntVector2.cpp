#include "Engine/Math/IntVector2.hpp"
#include <sstream>

const IntVector2 IntVector2::ZERO(0, 0);


IntVector2::IntVector2(int newx, int newy)
	{
	x=newx;
	y=newy;
	}

IntVector2 IntVector2::operator-(const IntVector2& other)
	{
	IntVector2 result = *this;
	result.x-=other.x;
	result.y-=other.y;
	return result;
	}

IntVector2 IntVector2::operator+(const IntVector2& other)
	{
	IntVector2 result = *this;
	result.x+=other.x;
	result.y+=other.y;
	return result;
	}

void IntVector2::setXY(int newx, int newy)
	{
	x=newx;
	y=newy;
	}

IntVector2 IntVector2::operator*(const IntVector2& other)
	{
	IntVector2 result(*this);
	result.x*=other.x;
	result.y*=other.y;
	return result;
	}

void IntVector2::operator*=(const IntVector2& other)
	{
	x*=other.x;
	y*=other.y;
	}

IntVector2 IntVector2::operator*(int scale)
	{
	IntVector2 result(*this);
	result*=scale;
	return result;
	}

void IntVector2::operator*=(int scale)
	{
	x*=scale;
	y*=scale;
	}

std::string IntVector2::asString()
	{
	std::stringstream ss;
	ss.str("");
	ss<<"("<<x<<","<<y<<")";
	std::string result = ss.str();
	return result;
	}

int IntVector2::calcDistanceSquaredTo(const IntVector2& other) const
	{
	IntVector2 copy(*this);
	IntVector2 displacement = copy-other;
	int result=displacement.calcLengthSquared();
	return result;
	}

int IntVector2::calcLengthSquared() const
{
	return x*x + y*y;
	}

IntVector2 interpolate(int scaleA, const IntVector2& a, int scaleB, const IntVector2& b)
	{
	if (scaleA+scaleB==0)
		return IntVector2::ZERO;
	IntVector2 result;
	int totalScale=scaleA + scaleB;
	result.x=(scaleA*a.x+scaleB*b.x)/totalScale;
	result.y=(scaleA*a.y+scaleB*b.y)/totalScale;
	return result;
	}