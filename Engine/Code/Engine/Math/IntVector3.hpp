#pragma once

class Vector3;

struct IntVector3
	{
	//public:
	int x;
	int y;
	int z;
	static const IntVector3 ZERO;
	IntVector3(): x(0),y(0),z(0)		{}
	IntVector3(int setx,int sety,int setz): x(setx),y(sety),z(setz)		{}
	IntVector3(const Vector3& floatVector);
	IntVector3 operator+(const IntVector3& other);
	IntVector3 operator-(const IntVector3& other);
	void operator+=(const IntVector3& other);
	void operator-=(const IntVector3& other);
	void scaleBy(const IntVector3& axisScales);
	IntVector3 getScaledBy(const IntVector3& axisScales);
	friend IntVector3 operator+(const IntVector3& one,const IntVector3& other);
	};

