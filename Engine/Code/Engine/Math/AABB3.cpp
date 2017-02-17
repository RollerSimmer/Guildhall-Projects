#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Sphere3.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/LineSegment3.hpp"


AABB3::AABB3(const Vector3& mins, const Vector3& maxs)
	: m_mins(mins)
	, m_maxs(maxs)
	{

	}

AABB3::AABB3()
	: m_mins(Vector3(0.f, 0.f, 0.f))
	, m_maxs(Vector3(0.f, 0.f, 0.f))
	{

	}

void AABB3::setXYs(float x1, float y1, float z1, float x2, float y2, float z2)
	{
	m_mins=Vector3(x1,y1,z1);
	m_maxs=Vector3(x2,y2,z2);
	}

bool AABB3::doesOverlapWith(const AABB3& otherbox) const
	{
	if(m_maxs.x<otherbox.m_mins.x)
		return false;
	if(otherbox.m_maxs.x < m_mins.x)
		return false;
	if (m_maxs.y < otherbox.m_mins.y)
		return false;
	if (otherbox.m_maxs.y < m_mins.y)
		return false;
	if (m_maxs.z < otherbox.m_mins.z)
		return false;
	if (otherbox.m_maxs.z < m_mins.z)
		return false;
	return true;
	}

bool AABB3::doesOverlapWith(const Sphere3& somesphere) const
	{
	return somesphere.doesOverlapWith(*this);
	}

void AABB3::stretchToIncludePoint(const Vector3& point)
	{
	m_mins.x = minOf(point.x, m_mins.x);
	m_mins.y = minOf(point.y, m_mins.y);
	m_mins.z = minOf(point.z, m_mins.z);
	m_maxs.x = maxOf(point.x, m_maxs.x);
	m_maxs.y = maxOf(point.y, m_maxs.y);
	m_maxs.z = maxOf(point.z, m_maxs.z);
	}

void AABB3::addPaddingToSides(const Vector3& padding)
	{
	m_mins-=padding;
	m_maxs+=padding;
	}

void AABB3::translate(const Vector3& translation)
	{
	m_mins+=translation;
	m_maxs+=translation;
	}

bool AABB3::isPointInside(const Vector3& point) const
	{
	bool isXInside = point.x >= m_mins.x && point.x <= m_maxs.x;
	bool isYInside = point.y >= m_mins.y && point.y <= m_maxs.y;
	bool isZInside = point.z >= m_mins.z && point.z <= m_maxs.z;
	bool result = isXInside && isYInside && isZInside;
	return result;
	}

Vector3 AABB3::calcDimensions() const
	{
	Vector3 result(calcWidth(),calcDepth(),calcHeight());
	return result;
	}

Vector3 AABB3::calcCenter() const
	{
	Vector3 result(0.5f*(m_maxs.x + m_mins.x), 0.5f*(m_maxs.y - m_mins.y), 0.5f*(m_maxs.z - m_mins.z));
	return result;
	}

float AABB3::calcWidth() const
	{
	return absval(m_maxs.x - m_mins.x);
	}

float AABB3::calcHeight() const
	{
	return absval(m_maxs.z - m_mins.z);
	}

float AABB3::calcDepth() const
	{
	return absval(m_maxs.y - m_mins.y);
	}

Vector3 AABB3::getRandomPointInBox()
	{
	return Vector3(  GetRandomFloatInRange(m_mins.x, m_maxs.x),
				     GetRandomFloatInRange(m_mins.y, m_maxs.y),
				     GetRandomFloatInRange(m_mins.z, m_maxs.z)  );
	}

Vector3 AABB3::getClosestPointInBoxTo(const Vector3& point) const
	{
	Vector3 result;
	result.x=clampFloat(point.x,m_mins.x,m_maxs.x);
	result.y=clampFloat(point.y,m_mins.y,m_maxs.y);
	result.z=clampFloat(point.z,m_mins.z,m_maxs.z);
	return result;
	}

bool AABB3::doesContainPoint(const Vector3& point) const
	{
	bool result =    isInRange(point.x, m_mins.x, m_maxs.x)
		          && isInRange(point.y, m_mins.y, m_maxs.y)
		          && isInRange(point.z, m_mins.z, m_maxs.z);
	return result;
	}


Vector3 AABB3::getFaceIntersectionNormalForXAxis(const LineSegment3& line) const
	{
	bool isPerpendicularToX=line.m_start.x==line.m_end.x;
	if (isPerpendicularToX)
		return Vector3::ZERO;
	float invXSpan = isPerpendicularToX ? 1.f : 1.f/(line.m_end.x-line.m_start.x);
	float tAtMaxX=(m_maxs.x-line.m_start.x)*invXSpan;
	float tAtMinX=(m_mins.x-line.m_start.x)*invXSpan;
	bool couldMinFaceHaveBeenHit=!(tAtMinX<0.f || tAtMinX>1.f);
	bool couldMaxFaceHaveBeenHit=!(tAtMaxX<0.f || tAtMaxX>1.f);
	if (!couldMinFaceHaveBeenHit && !couldMaxFaceHaveBeenHit)
		return Vector3::ZERO;

	AABB2 yzBounds(Vector2(m_mins.y,m_mins.z),Vector2(m_maxs.y,m_maxs.z));
	Vector2 yzAtLineStart=Vector2(line.m_start.y,line.m_start.z);
	Vector2 yzAtLineEnd=Vector2(line.m_end.y,line.m_end.z);
	Vector2 yzDisp=yzAtLineEnd-yzAtLineStart;
	Vector2 yzAtMinX = yzAtLineStart + tAtMinX*yzDisp;
	Vector2 yzAtMaxX = yzAtLineStart + tAtMaxX*yzDisp;

	if (yzBounds.doesContainPoint(yzAtMinX) && couldMinFaceHaveBeenHit)
		return Vector3(-1.f,0.f,0.f);
	if (yzBounds.doesContainPoint(yzAtMaxX) && couldMaxFaceHaveBeenHit)
		return Vector3(1.f,0.f,0.f);
	return Vector3::ZERO;
	}

Vector3 AABB3::getFaceIntersectionNormalForYAxis(const LineSegment3& line) const
	{
	bool isPerpendicularToY=line.m_start.y==line.m_end.y;
	if (isPerpendicularToY)
		return Vector3::ZERO;
	float invYSpan = isPerpendicularToY ? 1.f : 1.f/(line.m_end.y-line.m_start.y);
	float tAtMaxY=(m_maxs.y-line.m_start.y)*invYSpan;
	float tAtMinY=(m_mins.y-line.m_start.y)*invYSpan;
	bool couldMinFaceHaveBeenHit=!(tAtMinY<0.f || tAtMinY>1.f);
	bool couldMaxFaceHaveBeenHit=!(tAtMaxY<0.f || tAtMaxY>1.f);
	if (!couldMinFaceHaveBeenHit && !couldMaxFaceHaveBeenHit)
		return Vector3::ZERO;

	AABB2 xzBounds(Vector2(m_mins.x,m_mins.z),Vector2(m_maxs.x,m_maxs.z));
	Vector2 xzAtLineStart=Vector2(line.m_start.x,line.m_start.z);
	Vector2 xzAtLineEnd=Vector2(line.m_end.x,line.m_end.z);
	Vector2 xzDisp=xzAtLineEnd-xzAtLineStart;
	Vector2 xzAtMinY = xzAtLineStart + tAtMinY*xzDisp;
	Vector2 xzAtMaxY = xzAtLineStart + tAtMaxY*xzDisp;

	if (xzBounds.doesContainPoint(xzAtMinY) && couldMinFaceHaveBeenHit)
		return Vector3(0.f,-1.f,0.f);
	if (xzBounds.doesContainPoint(xzAtMaxY) && couldMaxFaceHaveBeenHit)
		return Vector3(0.f,1.f,0.f);
	return Vector3::ZERO;
	}

Vector3 AABB3::getFaceIntersectionNormalForZAxis(const LineSegment3& line) const
	{
	bool isPerpendicularToZ=line.m_start.z==line.m_end.z;
	if (isPerpendicularToZ)
		return Vector3::ZERO;
	float invZSpan = isPerpendicularToZ ? 1.f : 1.f/(line.m_end.z-line.m_start.z);
	float tAtMaxZ=(m_maxs.z-line.m_start.z)*invZSpan;
	float tAtMinZ=(m_mins.z-line.m_start.z)*invZSpan;
	bool couldMinFaceHaveBeenHit=!(tAtMinZ<0.f || tAtMinZ>1.f);
	bool couldMaxFaceHaveBeenHit=!(tAtMaxZ<0.f || tAtMaxZ>1.f);
	if (!couldMinFaceHaveBeenHit && !couldMaxFaceHaveBeenHit)
		return Vector3::ZERO;

	AABB2 xyBounds(Vector2(m_mins.x,m_mins.y),Vector2(m_maxs.x,m_maxs.y));
	Vector2 xyAtLineStart=Vector2(line.m_start.x,line.m_start.y);
	Vector2 xyAtLineEnd=Vector2(line.m_end.x,line.m_end.y);
	Vector2 xyDisp=xyAtLineEnd-xyAtLineStart;
	Vector2 xyAtMinZ = xyAtLineStart + tAtMinZ*xyDisp;
	Vector2 xyAtMaxZ = xyAtLineStart + tAtMaxZ*xyDisp;

	if (xyBounds.doesContainPoint(xyAtMinZ) && couldMinFaceHaveBeenHit)
		return Vector3(0.f,0.f,-1.f);
	if (xyBounds.doesContainPoint(xyAtMaxZ) && couldMaxFaceHaveBeenHit)
		return Vector3(0.f,0.f,1.f);
	return Vector3::ZERO;
	}

Vector3 AABB3::getFaceIntersectionNormal(const LineSegment3& line) const
	{
	Vector3 result;
	result=getFaceIntersectionNormalForXAxis(line);
	if (result!=Vector3::ZERO)
		return result;
	result=getFaceIntersectionNormalForYAxis(line);
	if (result!=Vector3::ZERO)
		return result;
	result=getFaceIntersectionNormalForZAxis(line);
	if (result!=Vector3::ZERO)
		return result;
	return Vector3::ZERO;
	}

Vector3 AABB3::calcConfirmedFaceIntersectionPointUsingFaceNormal(const LineSegment3& line,const Vector3& normal) const
	{
	float t=0.f;

	Vector3 disp=line.calcVector();

	float xSpan=disp.x;
	float ySpan=disp.y;
	float zSpan=disp.z;
	const float SMALL_FLOAT=1E-7f;
	float invXSpan=1.f/(xSpan!=0.f ? xSpan : SMALL_FLOAT);
	float invYSpan=1.f/(ySpan!=0.f ? ySpan : SMALL_FLOAT);
	float invZSpan=1.f/(zSpan!=0.f ? zSpan : SMALL_FLOAT);

	if		(normal==-Vector3::X_AXIS_VECTOR)		t=invXSpan*(m_mins.x-line.m_start.x);
	else if	(normal== Vector3::X_AXIS_VECTOR)		t=invXSpan*(m_maxs.x-line.m_start.x);
	else if	(normal==-Vector3::Y_AXIS_VECTOR)		t=invYSpan*(m_mins.y-line.m_start.y);
	else if	(normal== Vector3::Y_AXIS_VECTOR)		t=invYSpan*(m_maxs.y-line.m_start.y);
	else if	(normal==-Vector3::Z_AXIS_VECTOR)		t=invZSpan*(m_mins.z-line.m_start.z);
	else if	(normal== Vector3::Z_AXIS_VECTOR)		t=invZSpan*(m_maxs.z-line.m_start.z);

	Vector3 result = line.m_start + disp*t;
	return result;
	}

AABB3 AABB3::operator+(const AABB3& other) const
	{
	AABB3 result(*this);
	result.m_mins+=other.m_mins;
	result.m_maxs+=other.m_maxs;
	return result;
	}

AABB3 interpolate(float scaleA, const AABB3& A, float scaleB, const AABB3& B)
	{
	AABB3 result = A*scaleA + B*scaleB;
	return result;
	}

AABB3 AABB3::operator*(float scale) const
	{
	AABB3 result(*this);
	result.m_mins*=scale;
	result.m_maxs*=scale;
	return result;
	}

bool AABB3::operator==(const AABB3& other) const
	{
	bool result =  m_mins == other.m_mins && m_maxs == other.m_maxs;
	return result;
	}

void AABB3::operator-=(const Vector3& antiTranslation)
	{
	m_mins-=antiTranslation;
	m_maxs-=antiTranslation;
	}

void AABB3::operator+=(const Vector3& translation)
	{
	m_mins+=translation;
	m_maxs+=translation;
	}

AABB3 AABB3::operator-(const Vector3& antiTranslation) const
	{
	AABB3 result(*this);
	result-=antiTranslation;
	return result;
	}

AABB3 AABB3::operator+(const Vector3& translation) const
	{
	AABB3 result(*this);
	result+=translation;
	return result;
	}

bool doAABBsOverlap(const AABB3& a, const AABB3& b)
	{
	bool result=a.doesOverlapWith(b);
	return result;
	}
