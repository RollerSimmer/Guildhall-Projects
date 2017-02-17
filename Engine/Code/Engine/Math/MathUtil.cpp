#include <cmath>
#include <cstdlib>
#include "Engine/Math/MathUtil.hpp"
#include <vector>
#include <sstream>
#include "Engine/Math/Randomizer.hpp"

using namespace std;

float ConvertRadiansToDegrees( float radians )
	{
	return 180.f/M_PI*radians;
	}

float ConvertDegreesToRadians( float degrees )
	{
	return M_PI/180.f*degrees;
	}

int GetRandomIntLessThan( int maxValueNotInclusive )
	{
	return g_theRandomizer.nextInt() % maxValueNotInclusive;
	}

int GetRandomIntInRange( int minValueInclusive, int maxValueInclusive )
	{
	if(maxValueInclusive<minValueInclusive)
		return GetRandomIntInRange(maxValueInclusive,minValueInclusive);
	int diff=maxValueInclusive-minValueInclusive;
	return GetRandomIntLessThan(diff+1) + minValueInclusive;
	}

float GetRandomFloatZeroToOne()
	{
	const int LARGE_INT_VALUE = 30000;
	const float LARGE_FLOAT_VALUE = (float)LARGE_INT_VALUE;
	const float INV_LARGE_FLOAT_VALUE = 1.f/LARGE_FLOAT_VALUE;
	int intrand=GetRandomIntInRange(0,LARGE_INT_VALUE);
	float floatrand=(float)intrand;
	return floatrand*INV_LARGE_FLOAT_VALUE;
	}

float GetRandomFloatInRange( float minimumInclusive, float maximumInclusive )
	{
	if(maximumInclusive<minimumInclusive)
		return GetRandomFloatInRange(maximumInclusive,minimumInclusive);
	float diff = maximumInclusive - minimumInclusive;
	return GetRandomFloatZeroToOne() * diff + minimumInclusive;
	}

bool isPercentChance(int pct)
	{
	return GetRandomIntInRange(1, 100) <= pct;
	}

float int2float(int i)
	{
	return static_cast<float>(i);
	}


bool IsMostlyEqual(float a, float b, float epsilon /*= 0.001f*/)
{
	return fabs(a - b) < epsilon;
}

bool IsFloatInRange(float v, float lo, float hi)
{
	if (lo > hi)
		return IsFloatInRange(v, hi, lo);
	return v >= lo && v <= hi;
}

int getBellRandInt(int lo, int hi, int amtsamples)
{
	if (lo > hi)
		return getBellRandInt(hi, lo, amtsamples);
	int sum = 0;
	for (int i = 0; i < amtsamples; i++)
		sum += GetRandomIntInRange(lo, hi);
	int result = sum / amtsamples;
	return result;
}


float getBellRandFloat(float lo, float hi, int amtsamples)
	{
	if (lo > hi)
		return getBellRandFloat(hi, lo, amtsamples);
	float sum = 0;
	for (int i = 0; i < amtsamples; i++)
		sum += GetRandomFloatInRange(lo, hi);
	float result = sum / (float)amtsamples;
	return result;
	}

bool isBetweenFloat(float val, float boundA, float boundB)
	{
	return (val>=boundA && val<=boundB) || (val>=boundB && val<=boundA);
	}

float rangeMapFloat(float v, float oldlo, float oldhi, float newlo, float newhi)
	{
	if(oldlo>oldhi)
		return rangeMapFloat(v,oldhi,oldlo,newlo,newhi);
	if(newlo>newhi)
		return rangeMapFloat(v,oldlo,oldhi,newhi,newlo);

	float oldspan=oldhi-oldlo;
	float newspan=newhi-newlo;
	float valueRangeScale=(v-oldlo)/oldspan;
	float remappedValue=(valueRangeScale*newspan)+newlo;
	return remappedValue;
	}


float rangeMapNormalize(float v,float lo,float hi)
	{
	return rangeMapFloat(v,lo,hi,0.f,1.f);
	}

float clampFloat(float v, float lo, float hi)
	{
	if(lo>hi) return clampFloat(v,hi,lo);
	float result = v;
	if(result<lo)
		result=lo;
	if(result>hi)
		result=hi;
	return result;
	}

int clampInt(int v, int lo, int hi)
	{
	if (lo > hi) 
		return clampInt(v, hi, lo);
	int result = v;
	if (result < lo)
		result = lo;
	if (result > hi)
		result = hi;
	return result;
	}

bool isNan(float n)
	{
	return n!=n;
	}

float cosDegrees(float angleDegrees)
	{
	return cos(ConvertDegreesToRadians(angleDegrees));
	}

float sinDegrees(float angleDegrees)
	{
	return sin(ConvertDegreesToRadians(angleDegrees));
	}


float getRandomSine()
	{
	float degrees=GetRandomFloatInRange(0.f, 360.f);
	float result=sinDegrees(degrees);
	return result;
	}


float fixDegrees180(float degrees)
	{
	float result=degrees;
	while(result<-180.f)
		result+=360.f;
	while(result>180.f)
		result-=360.f;
	return result;
	}

float getSign(float val)
	{
	if(val==0.f)
		return 0.f;
	return val>0.f? 1.f:-1.f;
	}

float atan2Degrees(float y, float x)
	{
	return ConvertRadiansToDegrees(atan2(y,x));
	}


float asinfDegrees(float x)
	{
	return ConvertRadiansToDegrees(asinf(x));
	}

float calcNormalizedSineWaver(float radians)
	{
	float result=0.5f*(sin(radians)+1.f);
	return result;
	}

float integralPositivePower(float x, int exp)
	{
	float result=1;
	int curexp=exp;
	while (curexp > 5)
		{
		result *= x*x*x*x*x;
		curexp-=5;
		}
	while (curexp > 0)
		{
		result *= x;
		curexp--;
		}
	return result;
	}

float smootherStepN(float x, int startExp, int stopExp, int amtRecursions)
	{
	float t=x;
	for (int i = 0; i < amtRecursions ; i++)
		{
		t=smoothStepN(t,startExp,stopExp);
		}
	float result=t;
	return result;
	}

float smoothStopN(float x, int exp)
	{
	float xinv=1.f-x;
	float sstart=smoothStartN(xinv,exp);
	float result=1.f-sstart;
	return result;
	}

float smoothStartN(float x, int exp)
	{
	return integralPositivePower(x,exp);
	}

float smoothStepN(float x, int startExp, int stopExp)
	{
	float startHalf=(1-x)*smoothStartN(x,startExp);
	float stopHalf=x*smoothStopN(x,stopExp);
	float result=startHalf+stopHalf;
	return result;
	}


float smoothStep3(float x)
	{
	return smoothStepN(x,3,3);
	}

float smoothDropN(float x, int exp)
	{
	float result=1.f-smoothStopN(x,exp);
	return result;
	}

float bellCurveN(float x, int exp)
	{
	float xinv=1.f-x;
	int upScale=1<<(exp*2);
	float xRaised=integralPositivePower(x,exp);
	float xinvRaised=integralPositivePower(xinv,exp);
	float result=xRaised*xinvRaised*(float)upScale;
	return result;
	}

