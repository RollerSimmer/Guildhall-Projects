#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include <cmath>

AABB2 AABB2::createdWorldExpandedBy(float radius)
	{
	AABB2 newbox=*this;
	Vector2 offset(radius,radius);
	newbox.m_mins-=offset;
	newbox.m_maxs+=offset;	
	return newbox;
	}

void AABB2::getBounds(float& l, float& t, float& r, float& b)
	{
	m_mins.GetXY(l,t);
	m_maxs.GetXY(r,b);
	}

BoxEdge AABB2::getIntersectionEdge(const LineSegment2& line)
	{
	BoxEdge edges[]={BOX_EDGE_TOP,BOX_EDGE_LEFT,BOX_EDGE_BOTTOM,BOX_EDGE_RIGHT};
	int count=sizeof(edges)/sizeof(edges[0]);
	for (int i = 0; i < count; i++)
		{
		if (doesIntersectWithEdge(line,edges[i]))
			{
			return edges[i];
			}
		}
	return BOX_EDGE_NONE;
	}

bool AABB2::doesIntersectWithEdge(const LineSegment2& line, BoxEdge edge)
	{
	if (edge == BOX_EDGE_TOP)
		return doesIntersectWithTopEdge(line);
	else if (edge == BOX_EDGE_BOTTOM)
		return doesIntersectWithBottomEdge(line);
	else if (edge == BOX_EDGE_LEFT)
		return doesIntersectWithLeftEdge(line);
	else if (edge == BOX_EDGE_RIGHT)
		return doesIntersectWithRightEdge(line);
	else
		return false;
	}

AABB2::AABB2()
	: m_mins()
	, m_maxs()
	{
	
	}

AABB2::AABB2(const Vector2& newmins, const Vector2& newmaxs)
	: m_mins(newmins)
	, m_maxs(newmaxs)
	{
	
	}

AABB2::AABB2(float xmin, float ymin, float xmax, float ymax)
	: m_mins(xmin,ymin)
	, m_maxs(xmax,ymax)
	{
	
	}

AABB2::AABB2(const Vector2& center, float xradius, float yradius)
	: m_mins(center.x - xradius, center.y - yradius)
	, m_maxs(center.x + xradius, center.y + yradius)
	{

	}

bool AABB2::doesOverlapWith(const AABB2& otherbox) const
	{
	if (m_maxs.x < otherbox.m_mins.x)
		return false;
	if (otherbox.m_maxs.x < m_mins.x)
		return false;
	if (m_maxs.y < otherbox.m_mins.y)
		return false;
	if (otherbox.m_maxs.y < m_mins.y)
		return false;
	return true;
	}

bool AABB2::doesOverlapWith(const Disc2& disc) const
	{
	Axis2 axisOfHit;
	return doesOverlapWith(disc,axisOfHit);
	}

bool AABB2::doesOverlapWith(const Disc2& disc, Axis2& axisOfHit) const
	{
	float x, y;
	disc.m_center.GetXY(x, y);
	float radius = disc.m_radius;
	float l = m_mins.x;
	float r = m_maxs.x;
	float t = m_mins.y;
	float b = m_maxs.y;
	int xzone, yzone, zone;
	if (x < l) xzone = 0; else if (x > r) xzone = 2; else xzone = 1;  //  0 1 2
	if (y < t) yzone = 0; else if (y > b) yzone = 2; else yzone = 1;  //  3 4 5
	zone = yzone * 3 + xzone;                                         //  6 7 8
	float xdist = __min(abs(x - l), abs(x - r));
	float ydist = __min(abs(y - t), abs(y - b));
	switch (zone)
		{
		case 4:
			return true;
		case 0: case 2: case 6: case 8:
			if(xdist>ydist) 
				axisOfHit=Y_AXIS_2D;
			else
				axisOfHit=X_AXIS_2D;
			return radius*radius > xdist*xdist + ydist*ydist;
		case 1: case 7:
			axisOfHit=X_AXIS_2D;
			return radius > ydist;
		case 3: case 5:
			axisOfHit=Y_AXIS_2D;
			return radius > xdist;
		default:
			return false;
		}
	}

Vector2 AABB2::GenerateEdgePoint(float objradius)
	{
	float x = GetRandomFloatInRange(m_mins.x, m_maxs.x);
	float y = GetRandomFloatInRange(m_mins.y, m_maxs.y);
	Vector2 result(x, y);
	enum Edges { LEFT = 0, TOP, RIGHT, BOTTOM };
	int edge = GetRandomIntInRange(0, 3);
	switch (edge)
		{
		case LEFT:   result.x = m_mins.x - objradius; break;
		case RIGHT:  result.x = m_maxs.x + objradius; break;
		case TOP:    result.y = m_mins.y - objradius; break;
		case BOTTOM: result.y = m_maxs.y + objradius; break;
		}
	return result;
	}

void AABB2::stretchToIncludePoint(const Vector2& point)
	{
	if (point.x > m_maxs.x)
		m_maxs.x = point.x;
	else if (point.x < m_mins.x)
		m_mins.x = point.x;
	if (point.y > m_maxs.y)
		m_maxs.y = point.y;
	else if (point.y < m_mins.y)
		m_mins.y = point.y;
	}

void AABB2::addPaddingToSides(float xPaddingRadius, float yPaddingRadius)
	{
	Vector2 padVector(xPaddingRadius, yPaddingRadius);
	m_mins -= padVector;
	m_maxs += padVector;
	}

void AABB2::translate(const Vector2& translation)
	{
	m_mins += translation;
	m_maxs += translation;
	}

bool AABB2::IsPointInside(const Vector2& point) const
	{
	return (   point.x>=m_mins.x && point.y>=m_mins.y
		    && point.x<=m_maxs.x && point.y<=m_maxs.y);   
	}

const Vector2 AABB2::CalcSize() const
	{
	Vector2 result=m_maxs-m_mins;
	return result;
	}

const Vector2 AABB2::CalcCenter() const
	{
	Vector2 result = (m_mins+m_maxs)*0.5;
	return result;
	}

float AABB2::calcWidth()
	{
	return abs(m_maxs.x - m_mins.x);
	}

float AABB2::calcHeight()
	{
	return abs(m_maxs.y - m_mins.y);
	}

Vector2 AABB2::getRandomPointInBox()
	{
	float x = GetRandomFloatInRange(m_mins.x, m_maxs.x);
	float y = GetRandomFloatInRange(m_mins.y, m_maxs.y);
	Vector2 result(x,y);
	return result;
	}

Vector2 AABB2::getClosestEdgePointTo(float x, float y) const
	{
	float closestX=clampFloat(x,m_mins.x,m_maxs.x);
	float closestY=clampFloat(y,m_mins.y,m_maxs.y);
	float toLeftEdge=absval(closestX-m_mins.x);
	float toRightEdge=absval(closestX-m_maxs.x);
	float toTopEdge=absval(closestY-m_mins.y);
	float toBottomEdge=absval(closestY-m_maxs.y);
	float mindist=minOf(minOf(toLeftEdge,toRightEdge),minOf(toTopEdge,toBottomEdge));
	if(mindist==toLeftEdge)
		closestX=m_mins.x;
	else if(mindist==toRightEdge)
		closestX=m_maxs.x;
	if(mindist==toTopEdge)
		closestY=m_mins.y;
	else if(mindist==toBottomEdge)
		closestY=m_maxs.y;
	Vector2 result(closestX,closestY);
	return result;
	}

Vector2 AABB2::getClosestEdgePointTo(const Vector2& pos) const
	{
	return getClosestEdgePointTo(pos.x,pos.y);
	}

Vector2 AABB2::getClosestInteriorPointTo(const Vector2& pos) const
	{
	float closestX=clampFloat(pos.x,m_mins.x,m_maxs.x);
	float closestY=clampFloat(pos.y,m_mins.y,m_maxs.y);
	Vector2 result(closestX,closestY);
	return result;
	}

bool AABB2::doesContainPoint(const Vector2& point) const
	{
	return isInRange(point.x,m_mins.x,m_maxs.x) && isInRange(point.y,m_mins.y,m_maxs.y);
	}

AABB2 AABB2::operator+(const AABB2& other) const
	{
	AABB2 result(*this);
	result.m_mins+=other.m_mins;
	result.m_maxs+=other.m_maxs;
	return result;
	}

AABB2 AABB2::operator*(float scale)
	{
	AABB2 result(*this);
	result.m_mins*=scale;
	result.m_maxs*=scale;
	return result;
	}

AABB2 interpolate(float scaleA, const AABB2& A, float scaleB, const AABB2& B)
	{
	AABB2 Acopy(A);
	AABB2 Bcopy(B);
	AABB2 result= Acopy*scaleA+Bcopy*scaleB;
	return result;
	}

bool AABB2::doesIntersectWithTopEdge(const LineSegment2& line)
	{
	return doesIntersectWithAxialLine(m_mins.y,m_mins.x,m_maxs.x,line.m_start.y,line.m_end.y,line.m_start.x,line.m_end.x);
	}

bool AABB2::doesIntersectWithBottomEdge(const LineSegment2& line)
	{
	return doesIntersectWithAxialLine(m_maxs.y, m_mins.x, m_maxs.x, line.m_start.y, line.m_end.y, line.m_start.x, line.m_end.x);
	}

bool AABB2::doesIntersectWithLeftEdge(const LineSegment2& line)
	{
	return doesIntersectWithAxialLine(m_mins.x, m_mins.y, m_maxs.y, line.m_start.x, line.m_end.x, line.m_start.y, line.m_end.y);
	}

bool AABB2::doesIntersectWithRightEdge(const LineSegment2& line)
	{
	return doesIntersectWithAxialLine(m_maxs.x, m_mins.y, m_maxs.y, line.m_start.x, line.m_end.x, line.m_start.y, line.m_end.y);
	}

bool AABB2::doesIntersectWithAxialLine(float a, float bstart, float bend, float lineAStart, float lineAEnd, float lineBStart, float lineBEnd)
	{
	if(lineAStart==lineAEnd)
		return false;
	if(!isInRange(a,lineAStart,lineAEnd))
		return false;
	float scaleAtXing=(a-lineAStart)/(lineAEnd-lineAStart);
	float bAtXing=scaleAtXing*(lineBEnd-lineBStart)+lineBStart;
	return isInRange(bAtXing,bstart,bend);
	}

bool AABB2::operator==(const AABB2& other) const
	{
	return m_mins==other.m_mins && m_maxs==other.m_maxs;
	}

AABB2 makeAABB2(const Vector2& newMins, const Vector2& newMaxs)
	{
	AABB2 result;
	result.setMinMax(newMins,newMaxs);
	return result;
	}

AABB2 makeAABB2(float x1, float y1, float x2, float y2)
	{
	AABB2 result;
	result.setXYs(x1,y1,x2,y2);
	return result;
	}

void AABB2::setMinMax(const Vector2& newMins, const Vector2& newMaxs)
	{
	m_mins=newMins;
	m_maxs=newMaxs;
	}

void AABB2::setXYs(float x1, float y1, float x2, float y2)
	{
	m_mins.x = x1 < x2 ? x1 : x2;
	m_maxs.x = x1 > x2 ? x1 : x2;
	m_mins.y = y1 < y2 ? y1 : y2;
	m_maxs.y = y1 > y2 ? y1 : y2;
	}

const AABB2& AABB2::operator-=(const Vector2& antiTranslation)
	{
	return (*this)+=(-antiTranslation);
	}

const AABB2& AABB2::operator+=(const Vector2& translation)
	{
	m_mins+=translation;
	m_maxs+=translation;
	return *this;
	}

const AABB2 AABB2::operator-(const Vector2& antiTranslation) const
	{
	return (*this)+(-antiTranslation);
	}

const AABB2 AABB2::operator+(const Vector2& translation) const
	{
	AABB2 result;
	result.m_mins+=translation;
	result.m_maxs+=translation;
	return result;
	}

bool DoAABBsOverlap(const AABB2& a, const AABB2& b)
	{
	return a.doesOverlapWith(b);
	}

