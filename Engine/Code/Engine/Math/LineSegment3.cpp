#include "Engine/Math/LineSegment3.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/AABB3.hpp"
#include <cmath>

LineSegment3::LineSegment3()
	: m_start(Vector3::ZERO)
	, m_end(Vector3::ZERO)
	{

	}

LineSegment3::LineSegment3(const Vector3& startpoint, const Vector3& endpoint)
	: m_start(startpoint)
	, m_end(endpoint)
	{

	}

AABB3 LineSegment3::findBoundingBox() const
	{
	AABB3 result;
	result.m_mins.x = minOf(m_start.x, m_end.x);
	result.m_mins.y = minOf(m_start.y, m_end.y);
	result.m_mins.z = minOf(m_start.z, m_end.z);
	result.m_maxs.x = maxOf(m_start.x, m_end.x);
	result.m_maxs.y = maxOf(m_start.y, m_end.y);
	result.m_maxs.z = maxOf(m_start.z, m_end.z);
	return result;
	}

void LineSegment3::swapStartAndEnd()
	{
	Vector3 tmp=m_start;
	m_start=m_end;
	m_end=tmp;
	}

void LineSegment3::rotateAroundStart(float angleDegrees, const Vector3& axis)
	{
	Vector3 rotvect= makeIntoVector();
	rotvect.rotateDegreesAroundCustomAxis(angleDegrees,axis);
	m_end=rotvect+m_start;
	}

Vector3 LineSegment3::makeIntoVector() const
	{
	Vector3 result(m_end-m_start);
	return result;
	}

float LineSegment3::dotProduct(const LineSegment3& other)
	{
	Vector3 thisVector=makeIntoVector();
	Vector3 otherVector=other.makeIntoVector();
	return thisVector.dotProduct(otherVector);
	}

float LineSegment3::calcHeadingDegreesCustomAxis(const Vector3& axis)
	{
	Vector3 thisVector = makeIntoVector();
	float result=thisVector.calcHeadingDegreesCustomAxis(axis);
	return result;
	}

float LineSegment3::calcLength() const
	{
	return sqrt(calcLengthSquared());
	}

float LineSegment3::calcLengthSquared() const
{
	Vector3 thisVector=makeIntoVector();
	float result=thisVector.calcLengthSquared();
	return result;
	}

Vector3 LineSegment3::calcCenter() const
	{
	Vector3 result=(m_start+m_end)*0.5f;
	return result;
	}

void LineSegment3::translate(Vector3 offset)
	{
	m_start+=offset;
	m_end+=offset;
	}

void LineSegment3::scaleFromCenter(float scale)
	{
	Vector3 center=calcCenter();
	Vector3 startFromCenter=m_start-center;
	Vector3 endFromCenter=m_end-center;
	startFromCenter*=scale;
	endFromCenter*=scale;
	m_end=center+endFromCenter;
	m_start=center+startFromCenter;
	}

void LineSegment3::scaleFromStart(float scale)
	{
	Vector3 displacement=makeIntoVector();
	displacement*=scale;
	m_end=m_start+displacement;
	}

Vector3 LineSegment3::calcMidpoint()
	{
	return calcCenter();
	}

Vector3 LineSegment3::calcVector() const
	{
	return makeIntoVector();
	}

void LineSegment3::scaleBy(float scale)
	{
	m_start.scaleUniform(scale);
	m_end.scaleUniform(scale);
	}

void LineSegment3::operator+=(const LineSegment3& other)
	{
	m_start+=other.m_start;
	m_end+=other.m_end;
	}

void LineSegment3::operator*=(float scale)
	{
	scaleBy(scale);
	}

LineSegment3 interpolate(float scaleA, const LineSegment3& lineA, float scaleB, const LineSegment3& lineB)
	{
	LineSegment3 la(lineA),lb(lineB);
	la*=scaleA;
	lb*=scaleB;
	LineSegment3 result(la);
	result+=lb;
	return result;
	}

