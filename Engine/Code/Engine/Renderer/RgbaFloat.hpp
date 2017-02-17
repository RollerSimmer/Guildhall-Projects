#pragma once
#include <string>
#include "Engine/Renderer/Rgba.hpp"

struct RgbaFloat
	{
	float r;
	float g;
	float b;
	float a;

	static float convertColorByteToFloat(unsigned char bytecolor)
		{
		float result = (float)bytecolor * (1.f/255.f);
		return result;
		}

	static RgbaFloat fromRgba(const Rgba& color)
		{
		RgbaFloat result;
		result.r=convertColorByteToFloat(color.r);
		result.g=convertColorByteToFloat(color.g);
		result.b=convertColorByteToFloat(color.b);
		result.a=convertColorByteToFloat(color.a);
		return result;
		}

	RgbaFloat()
		: r(0),g(0),b(0),a(0)
		{

		}

	RgbaFloat(const Rgba& byteColor)
		{
		*this=fromRgba(byteColor);
		}

	RgbaFloat(float setR,float setG,float setB,float setA)
		: r(setR)
		, g(setG)
		, b(setB)
		, a(setA)
		{
		
		}

	const float* asArray()	const	{ return &r; }
	};

