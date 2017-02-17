#pragma once
#include <string>

class IntVector2
	{
	public:
		int x;
		int y;
	public:
		IntVector2(int newx = 0,int newy = 0);
		void setXY(int newx,int newy);
	public:
		static const IntVector2 ZERO;
		inline bool operator == (const IntVector2& other) { return x==other.x && y==other.y; }
		inline bool operator != (const IntVector2& other) { return !(*this==other); }
		inline void operator += (const IntVector2& other) { x+=other.x; y+= other.y; }
		IntVector2 operator - (const IntVector2& other);
		IntVector2 operator + (const IntVector2& other);
		IntVector2 operator * (const IntVector2& other);
		void operator *= (const IntVector2& other);
		IntVector2 operator * (int scale);
		void operator *= (int scale);
		std::string asString();
		int calcDistanceSquaredTo(const IntVector2& other) const;
		int calcLengthSquared() const;
		friend IntVector2 interpolate(int scaleA,const IntVector2& a,int scaleB,const IntVector2& b);
	};