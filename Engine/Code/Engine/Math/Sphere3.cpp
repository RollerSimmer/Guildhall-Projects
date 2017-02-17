#include "Engine/Math/Sphere3.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/MathUtil.hpp"

const Sphere3 Sphere3::UNIT_SPHERE(Vector3::ZERO, 1.f);

Sphere3::Sphere3()
	: m_center(0.f, 0.f, 0.f)
	, m_radius(0.f) {
	}

Sphere3::Sphere3(const Sphere3& copy)
	: m_center(copy.m_center)
	, m_radius(copy.m_radius)
	{

	}

Sphere3::Sphere3(float initialX, float initialY, float initialZ, float initialRadius)
	: m_center(initialX,initialY,initialZ)
	, m_radius(initialRadius)
	{

	}

Sphere3::Sphere3(const Vector3& initialCenter, float initialRadius)
	: m_center(initialCenter)
	, m_radius(initialRadius)
	{

	}

void Sphere3::setXYsAndRadius(float xcenter, float ycenter, float zcenter, float radius)
	{
	m_center.x = xcenter;
	m_center.y = ycenter;
	m_center.z = zcenter;
	m_radius = radius;
	}

AABB3 Sphere3::makeCubeAroundDisc() const
	{
	Vector3 displacement(m_radius, m_radius, m_radius);
	AABB3 result(m_center - displacement, m_center + displacement);
	return result;
	}

void Sphere3::stretchToIncludePoint(const Vector3& point)
	{
	Vector3 displacement=point-m_center;
	float dist=displacement.calcLength();
	if(dist>m_radius)
		m_radius=dist;
	}

void Sphere3::addPadding(float paddingRadius)
	{
	if(paddingRadius<0.f)
		return;
	m_radius+=paddingRadius;
	}

void Sphere3::translate(const Vector3& translation)
	{
	m_center+=translation;
	}

bool Sphere3::isPointInside(const Vector3& point) const
	{
	Vector3 displacement = point - m_center;
	float distSquared = displacement.calcLengthSquared();
	bool result = distSquared < m_radius*m_radius;
	return result;
	}

bool Sphere3::doesOverlapWith(const Sphere3& other) const
	{
	float radiusSum=other.m_radius+m_radius;
	Vector3 displacement(m_center-other.m_center);
	float distSquared = displacement.calcLengthSquared();
	bool result=radiusSum*radiusSum > distSquared;
	return result;
	}

bool Sphere3::doesOverlapWith(const AABB3& other) const
	{
	Vector3 closestPointInBoxToCenter=other.getClosestPointInBoxTo(m_center);

	Vector3 disp=m_center-closestPointInBoxToCenter;
	float distSquared=disp.calcLengthSquared();
	if(m_radius*m_radius>distSquared)
		return true;
// 	if (m_center.x + m_radius <= other.m_mins.x)
// 		return false;
// 	if (m_center.x - m_radius >= other.m_maxs.x)
// 		return false;
// 	if (m_center.y + m_radius <= other.m_mins.y)
// 		return false;
// 	if (m_center.y - m_radius >= other.m_maxs.y)
// 		return false;
// 	if (m_center.z + m_radius <= other.m_mins.z)
// 		return false;
// 	if (m_center.z - m_radius >= other.m_maxs.z)
// // 		return false;
// 	Vector3 closestEdgePoint
// 
// 	return doesOverlapWithFaceChecks(other);
	return false;
	}


bool Sphere3::getIntersectionWithXPlane(float x, Disc2& retdisc) const
	{
	float dist = absval(x - m_center.x);
	Vector2 discCenter(m_center.y, m_center.z);
	bool result = dist > m_radius;
	if (result)
		{
		retdisc.m_radius = sqrt(m_radius*m_radius - dist*dist);
		retdisc.m_center = discCenter;
		}
	else
		retdisc = Disc2(discCenter, 0.f);
	return result;
	}

bool Sphere3::getIntersectionWithYPlane(float y, Disc2& retdisc) const
	{
	float dist = absval(y - m_center.y);
	Vector2 discCenter(m_center.x, m_center.z);
	bool result = dist > m_radius;
	if (result)
		{
		retdisc.m_radius = sqrt(m_radius*m_radius - dist*dist);
		retdisc.m_center = discCenter;
		}
	else
		retdisc = Disc2(discCenter, 0.f);
	return result;
	}

bool Sphere3::getIntersectionWithZPlane(float z, Disc2& retdisc) const
	{
	float dist = absval(z - m_center.z);
	Vector2 discCenter(m_center.x, m_center.y);
	bool result = dist > m_radius;
	if (result)
		{
		retdisc.m_radius = sqrt(m_radius*m_radius - dist*dist);
		retdisc.m_center = discCenter;
		}
	else
		retdisc = Disc2(discCenter, 0.f);
	return result;
	}

void Sphere3::scaleBy(float scale)
	{
	m_radius*=scale;
	}

// helper struct and func for doesOverlap

bool Sphere3::doesOverlapWithFaceChecks(const AABB3& other) const
	{
	DiscAndAABB2 left,right, front, back,top,bottom;
	calcFaceIntersectionRegions(other,top,bottom,left,right,front,back);
	if (top.doMembersOverlap())
		return true;
	if (bottom.doMembersOverlap())
		return true;
	if (left.doMembersOverlap())
		return true;
	if (right.doMembersOverlap())
		return true;
	if(front.doMembersOverlap())
		return true;
	if (back.doMembersOverlap())
		return true;
	return false;
	}

void Sphere3::calcFaceIntersectionRegions(const AABB3& box, 
	                                      DiscAndAABB2& top, DiscAndAABB2& bottom, 
	                                      DiscAndAABB2& left, DiscAndAABB2& right, 
	                                      DiscAndAABB2& front, DiscAndAABB2& back) const
	{
	AABB2 xbox(Vector2(box.m_mins.y, box.m_mins.z), Vector2(box.m_maxs.y, box.m_maxs.z));
	AABB2 ybox(Vector2(box.m_mins.x, box.m_mins.z), Vector2(box.m_maxs.x, box.m_maxs.z));
	AABB2 zbox(Vector2(box.m_mins.x, box.m_mins.y), Vector2(box.m_maxs.x, box.m_maxs.y));
	left.m_box = right.m_box = xbox;
	front.m_box = back.m_box = ybox;
	top.m_box = bottom.m_box = zbox;
	front.m_discInit = getIntersectionWithYPlane(box.m_mins.y, front.m_disc);
	back.m_discInit = getIntersectionWithYPlane(box.m_maxs.y, back.m_disc);
	top.m_discInit = getIntersectionWithZPlane(box.m_mins.z, top.m_disc);
	bottom.m_discInit = getIntersectionWithZPlane(box.m_maxs.z, bottom.m_disc);
	left.m_discInit = getIntersectionWithXPlane(box.m_mins.x, left.m_disc);
	right.m_discInit = getIntersectionWithXPlane(box.m_maxs.x, right.m_disc);
	}

void Sphere3::operator-=(const Vector3& antiTranslation)
	{
	m_center -= antiTranslation;
	}

void Sphere3::operator+=(const Vector3& translation)
	{
	m_center += translation;
	}

Sphere3 Sphere3::operator-(const Vector3& antiTranslation) const
	{
	Sphere3 result(*this);
	result-=antiTranslation;
	return result;
	}

Sphere3 Sphere3::operator+(const Vector3& translation) const
	{
	Sphere3 result(*this);
	result += translation;
	return result;
	}


bool doSpheresOverlap(const Sphere3& a, const Sphere3& b)
	{
	float radiusSum=a.m_radius+b.m_radius;
	Vector3 displacement=a.m_center-b.m_center;
	float distSquared=displacement.calcLengthSquared();
	bool result = radiusSum*radiusSum > distSquared;
	return result;
	}

bool doSpheresOverlap(const Vector3& aCenter, float aRadius,
						const Vector3& bCenter, float bRadius)
	{
	bool result = doSpheresOverlap(Sphere3(aCenter, aRadius), Sphere3(bCenter, bRadius));
	return result;
	}

bool DiscAndAABB2::doMembersOverlap()
	{
	return m_discInit && m_disc.doesOverlapWith(m_box);
	}

Sphere3 interpolate(float scaleA, const Sphere3& A, float scaleB, const Sphere3& B)
	{
	Sphere3 result;
	result.m_center=A.m_center*scaleA + B.m_center*scaleB;
	result.m_radius=scaleA*A.m_radius + scaleB*B.m_radius;
	return result;
	}

