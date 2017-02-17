#include "Engine/Math/IntVector3.hpp"
#include "Engine/Math/Vector3.hpp"
#include <math.h>

IntVector3::IntVector3(const Vector3& floatVector)
	{
	x=(int)floor(floatVector.x);
	y=(int)floor(floatVector.y);
	z=(int)floor(floatVector.z);
	}

IntVector3 IntVector3::operator+(const IntVector3& other)
	{
	IntVector3 result=*this;
	result+=other;
	return result;
	}

void IntVector3::operator+=(const IntVector3& other)
	{
	x+=other.x;
	y+=other.y;
	z+=other.z;
	}

IntVector3 IntVector3::operator-(const IntVector3& other)
	{
	IntVector3 result=*this;
	result-=other;
	return result;
	}

void IntVector3::operator-=(const IntVector3& other)
	{
	x-=other.x;
	y-=other.y;
	z-=other.z;
	}

void IntVector3::scaleBy(const IntVector3& axisScales)
	{
	x*=axisScales.x;
	y*=axisScales.y;
	z*=axisScales.z;
	}

IntVector3 IntVector3::getScaledBy(const IntVector3& axisScales)
	{
	IntVector3 result=*this;
	result.scaleBy(axisScales);
	return result;
	}

IntVector3 operator+(const IntVector3& one,const IntVector3& other)
	{
	IntVector3 result=one;
	result+=other;
	return result;
	}