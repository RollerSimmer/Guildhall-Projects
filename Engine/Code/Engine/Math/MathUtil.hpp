
#pragma once

#include "Engine/Core/EngineBase.hpp"
#include <string>

const float M_PI = 3.14159265358979f;
const float fSQRT_3_OVER_3 = 0.57735026918963f;

float ConvertRadiansToDegrees( float radians );
float ConvertDegreesToRadians( float degrees );
int GetRandomIntLessThan( int maxValueNotInclusive );
int GetRandomIntInRange( int minValueInclusive, int maxValueInclusive );
float GetRandomFloatZeroToOne();
float GetRandomFloatInRange( float minimumInclusive, float maximumInclusive );
bool isPercentChance(int pct);
float int2float(int i);
bool IsMostlyEqual(float a, float b, float epsilon = 0.001f);
bool IsFloatInRange(float v, float lo, float hi);
int getBellRandInt(int lo, int hi, int amtsamples);
float getBellRandFloat(float lo,float hi,int amtsamples);
bool isBetweenFloat(float val, float boundA, float boundB);
float rangeMapFloat(float v,float oldlo,float oldhi,float newlo,float newhi);
float rangeMapNormalize(float v,float lo,float hi);
float clampFloat(float v, float lo, float hi);
int clampInt(int v, int lo, int hi);
bool isNan(float n);
float cosDegrees(float angleDegrees);
float sinDegrees(float angleDegrees);
float getRandomSine();
float fixDegrees180(float degrees);
float getSign(float val);
float atan2Degrees(float y, float x);
float asinfDegrees(float x);
template<class T> bool isInRange(T v,T a,T b);
float calcNormalizedSineWaver(float radians);

template<class T> T minOf(T a, T b) { return a < b ? a : b; }
template<class T> T maxOf(T a, T b) { return a > b ? a : b; }

template <class T>
T absval(T val)
	{
	return val>=0?val:-val;
	}
template<class T>
bool isInRange(T v, T lo, T hi)
	{
	if(lo>hi)
		return isInRange(v,hi,lo);
	return v >= lo && v <= hi;
	}

// easing functions
float integralPositivePower(float x,int exp);
float smootherStepN(float x, int startExp, int stopExp, int amtRecursions);
float smoothStopN(float x, int exp);
float smoothStartN(float x,int exp);
float smoothStepN(float x,int startExp,int endExp);
float smoothStep3(float x);
float smoothDropN(float x,int exp);
float bellCurveN(float x,int exp);


// general interpolate template function
template <class T>
T interpolate(T a,T b,float scale)
	{
	float t = clampFloat(scale,0.f,1.f);
	float tinv = 1.f-t;
	T result = a*tinv + b*t;
	return result;
	}

#define BIT(place) (1<<(place))
#define MASK(place) ((1<<(place))-1)