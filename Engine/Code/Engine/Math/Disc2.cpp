#include "Disc2.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/AABB2.hpp"

const Disc2 Disc2::UNIT_CIRCLE(Vector2::ZERO,1.f);



Disc2::Disc2()
	: m_center(0.f,0.f)
	, m_radius(0.f)
	{
	
	}

Disc2::Disc2(const Disc2& copy)
	: m_center(copy.m_center)
	, m_radius(copy.m_radius)
	{

	}

Disc2::Disc2(float initialX, float initialY, float initialRadius)
	: m_center(initialX,initialY)
	, m_radius(initialRadius)
	{

	}


void Disc2::setXYsAndRadius(float xcenter, float ycenter, float radius)
	{
	m_center.x = xcenter;
	m_center.y = ycenter;
	m_radius = radius;
	}

Disc2::Disc2(const Vector2& initialCenter, float initialRadius)
	: m_center(initialCenter)
	, m_radius(initialRadius)
	{

	}

AABB2 Disc2::makeBoxAroundDisc() const 
	{
	Vector2 radii(m_radius,m_radius);
	AABB2 newbox;
	newbox.m_mins = this->m_center - radii;
	newbox.m_maxs = this->m_center + radii;
	return newbox;
	}

void Disc2::StretchToIncludePoint(const Vector2& point)
	{
	float prospectiveRadius=CalcDistance(m_center,point);
	if(prospectiveRadius>m_radius)
		m_radius=prospectiveRadius;
	}

void Disc2::AddPadding(float paddingRadius)
	{
	m_radius+=paddingRadius;
	}

void Disc2::Translate(const Vector2& translation)
	{
	m_center+=translation;
	}

bool Disc2::IsPointInside(const Vector2& point) const
	{
	float dist = CalcDistance(point, m_center);
	return dist < m_radius;
	}

void Disc2::operator-=(const Vector2& antiTranslation)
	{
	m_center-=antiTranslation;
	}

void Disc2::operator+=(const Vector2& translation)
	{
	m_center+=translation;
	}

const Disc2 Disc2::operator-(const Vector2& antiTranslation) const
	{
	return (*this)+(-antiTranslation);
	}

const Disc2 Disc2::operator+(const Vector2& translation) const
	{
	Disc2 result = *this;
	result += translation;
	return result;
	}

bool Disc2::doesOverlapWith(const Disc2& other) const 
	{
	Vector2 deltaVector = m_center - other.m_center;
	float distSquared = deltaVector.calcLengthSquared();
	float radiusSum = m_radius+other.m_radius;
	float radiusSumSquared = radiusSum*radiusSum;
	bool result = radiusSumSquared>distSquared;
	return result;
	}

bool Disc2::doesOverlapWith(const AABB2& other) const
	{
	return other.doesOverlapWith(*this);
	}

bool Disc2::doesOverlapWith(const AABB2& other, Axis2& axisOfHit) const 
	{
	return other.doesOverlapWith(*this,axisOfHit);
	}

void Disc2::scaleBy(float scale)
	{
	m_radius*=scale;
	}

bool DoDiscsOverlap(const Disc2& a, const Disc2& b)
	{
	return a.doesOverlapWith(b);
	}

bool DoDiscsOverlap(const Vector2& aCenter, float aRadius,
	const Vector2& bCenter, float bRadius)
	{
	Disc2 a(aCenter,aRadius);
	Disc2 b(bCenter,bRadius);
	return a.doesOverlapWith(b);
	}

Disc2 interpolate(float scaleA, const Disc2& a, float scaleB, const Disc2& b)
	{
	Disc2 result,acopy(a),bcopy(b);
	result.m_center=acopy.m_center*scaleA+bcopy.m_center*scaleB;
	result.m_radius=acopy.m_radius*scaleA+bcopy.m_radius*scaleB;	
	return result;
	}