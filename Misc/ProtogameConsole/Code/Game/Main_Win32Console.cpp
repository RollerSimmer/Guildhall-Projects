#include <iostream>
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/LineSegment3.hpp"
#include "Engine/Math/Matrix4.hpp"
using namespace std;


void validate(char* message, bool isSuccessful)
{
	cout << "Validating " << message << "." << endl;
	if (isSuccessful)
		printf("Success.\n");
	else
		printf("FAILED!\n");
}

void bellDrawBottomRows(int bellHi)
{
	// this draws the bottom row
	for (int row = 0; row < 2; row++)
	{
		for (int i = 0; i < bellHi+1; i++)
		{
			int digitToPrint;
			if (row == 0)
				digitToPrint = (i / 10) % 10;
			else
				digitToPrint = i % 10;
			cout << digitToPrint;
		}
		cout << endl;
	}
}

void bellRandomTest()
{
	cout << "Normal-distribution random test" << endl;
	// bell random test
	const int bellHi = 100;
	int bellcount[bellHi + 1];
	for (int i = 0; i < bellHi + 1; i++)
		bellcount[i] = 0;
	const int amtBellTestRuns = 500;
	const int amtBellSamples = 4;
	for (int j = 0; j < amtBellTestRuns; j++)
	{
		int bellRandVal = getBellRandInt(0, bellHi, amtBellSamples);
		if (bellRandVal >= 0 && bellRandVal <= bellHi)
			bellcount[bellRandVal]++;
	}
	int maxcount = 0;
	for (int i = 0; i < bellHi+1; i++)
	{
		if (bellcount[i] > maxcount)
			maxcount = bellcount[i];
	}
	// this draws a bell curve
	for (int row = maxcount; row > 0; row--)
	{
		for (int i = 0; i < bellHi+1; i++)
		{
			int count = bellcount[i];
			if (count >= row)
				cout << "*";
			else
				cout << " ";
		}
		cout << endl;
	}
	bellDrawBottomRows(bellHi);
}

void smoothTestN(int exp)
	{
	for (float x = 0.f; x <= 1.f+0.025f; x+=0.05f)
		{
		float tstep = smoothStepN(x,exp,exp);
		float tstart = smoothStartN(x,exp);
		float tstop = smoothStopN(x,exp);
		float tsmootherStep = smootherStepN(x, exp, exp, 2);
		float tevenSmootherStep = smootherStepN(x, exp, exp, 3);
		float tbell = bellCurveN(x,exp);
		cout << x << "|" << tstart << "|" << tstop << "|" << tstep << "|" << tsmootherStep << "|" << tevenSmootherStep << "|" << tbell << endl;
		}
	}

const float LENGTH45 = sqrt(2.f) / 2.f;

void testMathUtilBasics()
	{
	// mathutils
	validate("IsMostlyEqual", IsMostlyEqual(1.f, 1.000001f));
	validate("ConvertRadiansToDegrees", IsMostlyEqual(ConvertRadiansToDegrees(M_PI), 180.f));
	validate("ConvertDegreesToRadians", IsMostlyEqual(ConvertDegreesToRadians(180.f), M_PI));
	int max = 25;
	int randint = GetRandomIntLessThan(max);
	validate("GetRandomIntLessThan", randint < max && randint >= 0);
	bool outofbounds = false;
	for (int i = 0; i < 100 && !outofbounds; i++)
		{
		randint = GetRandomIntInRange(0, 25);
		outofbounds = randint < 0 || randint > 25;
		}
	validate("GetRandomIntLessThan", !outofbounds);
	float randfloat = GetRandomFloatZeroToOne();
	validate("GetRandomFloatZeroToOne", randfloat >= 0.f && randfloat <= 1.f);
	validate("IsFloatInRange", IsFloatInRange(0.5, 0.0, 1.0) && IsFloatInRange(1.0, 0.0, 1.0) && IsFloatInRange(0.0, 0.0, 1.0));
	float minfloat = 0.1f;
	float maxfloat = 0.2f;
	randfloat = GetRandomFloatInRange(minfloat, maxfloat);
	validate("GetRandomFloatInRange", IsFloatInRange(randfloat, minfloat, maxfloat));
	int truecount = 0;
	int percent = 20;
	for (int i = 0; i < 1000; i++)
		{
		if (isPercentChance(percent))
			truecount++;
		}
	validate("isPercentChance", truecount >= 195);
	int intval = 3;
	float convertedFloat = int2float(intval);
	validate("int2float", IsMostlyEqual(static_cast<float>(intval), convertedFloat));
	}

void testVector2Basics()
	{
	// vector2 basics
	Vector2 a, b, e, f;
	a.SetXY(1.f, 2.f);
	b.SetXY(1.0000000001f, 2.000000001f);
	validate("IsMostlyEqual", IsMostlyEqual(a, b));
	Vector2 c(3.f, 4.f);
	validate("value constructor", c.x == 3.f && c.y == 4.f);
	Vector2 d(c);
	validate("copy constructor", d.x == 3.f && d.y == 4.f);
	float x, y;
	c.GetXY(x, y);
	validate("getxy", x == 3.f&&y == 4.f);
	c.SetXY(7.f, 21.f);
	validate("setxy", c.x == 7.f && c.y == 21.f);
	// mutators
	c.SetXY(3.f, 4.f);
	float l = c.calcLength();
	validate("calcLength", l == 5.f);
	l = c.calcLengthSquared();
	validate("calcLengthSquared", l == 25.f);
	c.SetXY(3.f, 4.f);
	float headingDegrees = c.calcHeadingDegrees();
	validate("CalcHeadingDegrees", IsMostlyEqual(headingDegrees, 53.1301024f));
	float headingRadians = c.CalcHeadingRadians();
	validate("CalcHeadingRadians", IsMostlyEqual(headingRadians, 0.927295218f));
	c.SetXY(1.f, 2.f);
	c.Rotate90Degrees();
	validate("Rotate90Degrees", c.x == -2.f && c.y == 1.f);
	c.RotateNegative90Degrees();
	validate("RotateNegative90Degrees", c.x == 1.f && c.y == 2.f);
	e.SetXY(1.f, 1.f);
	e.Normalize();
	validate("Normalize", IsMostlyEqual(e.calcLength(), 1.f));
	e.SetXY(LENGTH45, LENGTH45);
	e.RotateDegrees(-45.f);
	validate("RotateDegrees", IsMostlyEqual(e.x, 1.f) && IsMostlyEqual(e.y, 0.f));
	e.SetXY(1.f, 0.f);
	e.RotateRadians(static_cast<float>(M_PI / 4.f));
	validate("RotateRadians", IsMostlyEqual(e.x, LENGTH45) && IsMostlyEqual(e.y, LENGTH45));
	e.SetXY(3.f, 1.f);
	e.setLength(1.f);
	validate("SetLength", IsMostlyEqual(e.x*e.x + e.y*e.y, 1.f));
	e.SetXY(5.f, 10.f);
	e.ScaleUniform(0.2f);
	validate("ScaleUniform", e.x == 1.f && e.y == 2.f);
	e.SetXY(5.f, 10.f);
	Vector2 escale(2.f, 0.5f);
	e.ScaleNonUniform(escale);
	validate("ScaleNonUniform", e.x == 10.f && e.y == 5.f);
	e.InverseScaleNonUniform(escale);
	validate("InverseScaleNonUniform", e.x == 5.f && e.y == 10.f);
	e.SetXY(-2.f, 0.f);
	e.SetHeadingDegrees(45.f);
	validate("SetHeadingDegrees", IsMostlyEqual(e.x, sqrt(2.f)) && IsMostlyEqual(e.y, sqrt(2.f)));
	e.SetXY(0.f, -1.f);
	e.SetHeadingRadians(M_PI / 4.0f);
	validate("SetHeadingRadians", IsMostlyEqual(e.x, LENGTH45) && IsMostlyEqual(e.y, LENGTH45));

	e.SetXY(1.f, 0.f);
	e.SetUnitLengthAndHeadingDegrees(45.f);
	validate("SetUnitLengthAndHeadingDegrees", IsMostlyEqual(e.calcLength(), 1.f) && IsMostlyEqual(e.x, LENGTH45) && IsMostlyEqual(e.y, LENGTH45));
	e.SetXY(1.f, 0.f);
	e.SetUnitLengthAndHeadingRadians(M_PI / 4.f);
	validate("SetUnitLengthAndHeadingRadians", IsMostlyEqual(e.calcLength(), 1.f) && IsMostlyEqual(e.x, LENGTH45) && IsMostlyEqual(e.y, LENGTH45));
	e.SetXY(1.f, 0.f);
	e.SetLengthAndHeadingDegrees(5.f, 45.f);
	validate("SetLengthAndHeadingDegrees", IsMostlyEqual(e.calcLength(), 5.f) && IsMostlyEqual(e.x, 5.f*LENGTH45) && IsMostlyEqual(e.y, 5.f*LENGTH45));
	e.SetXY(1.f, 0.f);
	e.SetLengthAndHeadingRadians(5.f, M_PI / 4.f);
	validate("SetLengthAndHeadingRadians", IsMostlyEqual(e.calcLength(), 5.f) && IsMostlyEqual(e.x, 5.f*LENGTH45) && IsMostlyEqual(e.y, 5.f*LENGTH45));

	// operators
	c.SetXY(3.f, 4.f);
	c = d;
	validate("== operator", c == d);
	validate("!= operator", !(c != d));
	e = c + d;
	validate("+ operator", e.x == 6.f && e.y == 8.f);
	d = d + d;
	e = d - c;
	validate("- operator", e.x == 3.f && e.y == 4.f);
	d = c;
	e = d*c;
	validate("* operator (non-uniform)", e.x == 9.f && e.y == 16.f);
	d.SetXY(3.f, 4.f);
	e = d*5.f;
	validate("scalar multiply", e.x == 15.f && e.y == 20.f);
	e = d / d;
	validate("/ operator ", e.x == 1.f && e.y == 1.f);
	e.SetXY(3.f, 4.f);
	e *= 5.f;
	validate("*= operator", e.x == 15.f && e.y == 20.f);
	c.SetXY(3.f, 4.f);
	d.SetXY(3.f, 4.f);
	e = d;
	e *= c;
	validate("*= operator (non-uniform)", e.x == 9.f && e.y == 16.f);
	c.SetXY(3.f, 4.f);
	d.SetXY(3.f, 4.f);
	e = d;
	e += c;
	validate("+= operator", e.x == 6.f && e.y == 8.f);
	d.x = 4.f;
	c.x = 2.f;
	d.y = 3.f;
	c.y = 1.5f;
	e = d;
	e -= c;
	validate("-= operator", e.x == 2.f && e.y == 1.5f);
	c.SetXY(4.f, 5.f);
	d.SetXY(7.f, 9.f);
	float dist = CalcDistance(c, d);
	validate("CalcDistance", IsMostlyEqual(dist, 5.f));
	float distSquared = CalcDistanceSquared(c, d);
	validate("CalcDistanceSquared", IsMostlyEqual(distSquared, 25.f));
	}

void testFloatRandom()
	{
	for (int i=0;i<100000;i++)
		{
		float v=GetRandomFloatZeroToOne();
		cout<<v<<std::endl;
		}
	}

void testAABB3LineIntersectionNormals()
	{
	AABB3 bounds(Vector3(-1.f,-1.f,-1.f),Vector3(1.f,1.f,1.f));
	float radius=5.f;
	Vector3 lineEnd(0.f,0.f,0.f);
	Vector3 lineStart(0.f,0.f,0.f);
	LineSegment3 line;
	Vector3 normal;
	float& u=lineStart.z;
	float& v=lineStart.x;
	float& w=lineStart.y;
	w=0.f;
	for (float angleDegrees=0.f;angleDegrees<=360.f;angleDegrees+=15.f)
		{
		u=radius*cosDegrees(angleDegrees);
		v=radius*sinDegrees(angleDegrees);
		line=LineSegment3(lineStart,lineEnd);
		normal=bounds.getFaceIntersectionNormal(line);
		cout<<"Line: (" << line.m_start.x << "," << line.m_start.y << "," << line.m_start.z << ")<-->";
		cout<<      "(" << line.m_end.x << ","   << line.m_end.y   << "," << line.m_end.z << "); ";
		cout<<"IntersectionFaceNormal: (" << normal.x << "," << normal.y << ","  << normal.z << ")"<<endl;
		}

	}

void testMatrix4QuaternionRotations()
	{
	Vector3 rotAxis(0.f,1.f,1.f);
	if(rotAxis.calcLengthSquared()!=1.f)
		rotAxis.normalize();
	Vector3 translation(0.f,0.f,0.f);
	float angleDegrees=45.f;
	
	Matrix4 quatRot=Matrix4::calcQuaternionRotationMatrix(rotAxis,translation,angleDegrees);
	Vector4 pos(1.f,0.f,0.f,0.f);

	Vector4 rotPos=quatRot.transformVector(pos);

	cout<<"pos="<<pos.asString()<<endl;
	cout<<"rotPos="<<rotPos.asString()<<endl;
	}

int main()
	{
// 	testMathUtilBasics();
// 	testVector2Basics();
// 	bellRandomTest();
// 	smoothTestN(3);
// 	testFloatRandom();

//	testAABB3LineIntersectionNormals();
	testMatrix4QuaternionRotations();

	cout<<(-16&0xf)<<" "<<(16&0xf)<<endl;

	system("pause");
	return 0;
	}