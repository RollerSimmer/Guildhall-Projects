#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/AABB2.hpp"


LineSegment2::LineSegment2(const Vector2& startpoint, const Vector2& endpoint)
	: m_start(startpoint)
	, m_end(endpoint)
	{
	
	}

LineSegment2::LineSegment2()
	: m_start(Vector2::ZERO)
	, m_end(Vector2::ONE)
	{

	}

LineSegment2::LineSegment2(float x1, float y1, float x2, float y2)
	: m_start(x1,y1)
	, m_end(x2,y2)
	{
	
	}

AABB2 LineSegment2::findBoundingBox() const 
	{
	Vector2 m_mins;
	Vector2 m_maxs;
	if (m_start.x < m_end.x)
		{
		m_mins.x=m_start.x;
		m_maxs.x=m_end.x;
		}
	else
		{
		m_mins.x=m_end.x;
		m_maxs.x=m_start.x;
		}
	if (m_start.y < m_end.y)
		{
		m_mins.y = m_start.y;
		m_maxs.y = m_end.y;
		}
	else
		{
		m_mins.y = m_end.y;
		m_maxs.y = m_start.y;
		}
		
	AABB2 result(m_mins,m_maxs);
	return result;
	}

bool LineSegment2::doesIntersectWith(Vector2& poi, const LineSegment2& otherline) const 
	{
	poi.SetXY(0.f,0.f);
	if (!doesAABBIntersectWithAABBOf(otherline))
		return false;
	float x1 = m_start.x;
	float y1 = m_start.y;
	float x2 = m_end.x;
	float y2 = m_end.y;
	float x3 = otherline.m_start.x;
	float y3 = otherline.m_start.y;
	float x4 = otherline.m_end.x;
	float y4 = otherline.m_end.y;
	float X1Y2subY1X2 = x1*y2 - y1*x2;
	float X3Y4subY3X4 = x3*y4 - y3*x4;
	float X1subX2 = x1 - x2;
	float X3subX4 = x3 - x4;
	float Y1subY2 = y1 - y2;
	float Y3subY4 = y3 - y4;
	float denom = X1subX2*Y3subY4 - Y1subY2*X3subX4;
	float numerX = X1Y2subY1X2*X3subX4 - X1subX2*X3Y4subY3X4;
	float numerY = X1Y2subY1X2*Y3subY4 - Y1subY2*X3Y4subY3X4;
	bool areParallelOrCoincident = denom == 0.f;
	if(areParallelOrCoincident)
		return false;
	float denomInverse=1.f/denom;
	poi.x = denomInverse*numerX;
	poi.y = denomInverse*numerY;
	bool xInLines = isBetweenFloat(poi.x, m_start.x, m_end.x) && isBetweenFloat(poi.x,otherline.m_start.x,otherline.m_end.x);
	bool yInLines = isBetweenFloat(poi.y, m_start.y, m_end.y) && isBetweenFloat(poi.y,otherline.m_start.y,otherline.m_end.y);
	return xInLines && yInLines;
	}

bool LineSegment2::doesAABBIntersectWithAABBOf(const LineSegment2& otherline) const
	{
	AABB2 myBoundingBox=findBoundingBox();
	AABB2 theirBoundingBox=otherline.findBoundingBox();
	return myBoundingBox.doesOverlapWith(theirBoundingBox);
	}

void LineSegment2::swapStartAndEnd()
	{
	Vector2 temp(m_start);
	m_start=m_end;
	m_end=temp;
	}

void LineSegment2::rotateAroundStart(float angleDegrees)
	{
	Vector2 heading(m_end-m_start);
	heading.RotateDegrees(angleDegrees);
	m_end=m_start+heading;
	}

Vector2 LineSegment2::makeIntoVector() const
	{
	Vector2 result(m_end-m_start);
	return result;
	}

float LineSegment2::dotProduct(const LineSegment2& other)
	{
	Vector2 thisvector = makeIntoVector();
	return thisvector.dotProduct(other.makeIntoVector());
	}

float LineSegment2::calcHeadingDegrees()
	{
	Vector2 thisvector = makeIntoVector();
	return thisvector.calcHeadingDegrees();
	}

float LineSegment2::calcLength() const
	{
	return makeIntoVector().calcLength();
	}

float LineSegment2::calcLengthSquared()
	{
	return makeIntoVector().calcLengthSquared();
	}

void LineSegment2::translate(Vector2 offset)
	{
	m_start+=offset;
	m_end+=offset;
	}

void LineSegment2::scaleFromCenter(float scale)
	{
	Vector2 center=calcMidpoint();
	m_start = center + (m_start - center)*scale;
	m_end = center + (m_end - center)*scale;
	}

void LineSegment2::scaleFromStart(float scale)
	{
	m_end = m_start + (m_end - m_start)*scale;
	}

Vector2 LineSegment2::calcMidpoint()
	{
	return (m_start+m_end)*0.5f;
	}

Vector2 LineSegment2::calcVector() const
	{
	return m_end - m_start;
	}
