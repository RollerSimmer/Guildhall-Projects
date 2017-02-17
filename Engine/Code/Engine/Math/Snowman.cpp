#include "Engine/Math/Snowman.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/LineSegment3.hpp"
#include "Engine/Core/EngineBase.hpp"

Snowman::Snowman()
	: m_center(0.f,0.f,0.f)
	, m_radius(0.f)
	, m_halfHeight(0.f)
	{
	reCenter(m_center);
	}

Snowman::Snowman(const Snowman& copy)
	{
	*this=copy;
	}

Snowman::Snowman(const Vector3& center,float radius,float height)
	: m_center(center)
	, m_radius(radius)
	, m_halfHeight(height)
	{
	reCenter(center);
	}

void Snowman::translate(const Vector3& translation)
	{
	reCenter(m_center+translation);
	}

bool Snowman::isPointInside(const Vector3& point) const
	{
	bool result=false;
	result=m_spheres[0].isPointInside(point)
		|| m_spheres[1].isPointInside(point)
		|| m_spheres[2].isPointInside(point);
	return result;
	}

bool Snowman::doesOverlapWith(const Sphere3& otherShape) const
	{
	bool result=false;
	result=m_spheres[0].doesOverlapWith(otherShape)
		|| m_spheres[1].doesOverlapWith(otherShape)
		|| m_spheres[2].doesOverlapWith(otherShape);
	return result;
	}

bool Snowman::doesOverlapWith(const AABB3& otherShape) const
	{
	bool result=false;
	result=m_spheres[0].doesOverlapWith(otherShape)
		|| m_spheres[1].doesOverlapWith(otherShape)
		|| m_spheres[2].doesOverlapWith(otherShape);
	return result;
	}

void Snowman::scaleBy(float scale)
	{
	m_radius*=scale;
	m_halfHeight*=scale;
	reCenter(m_center);
	}

void Snowman::reCenter(const Vector3& newCenter)
	{
	float distTweenCenters=maxOf(m_halfHeight-m_radius,0.f);
	Vector3 zinc=Vector3::Z_AXIS_VECTOR*distTweenCenters;
	m_center=newCenter;
	m_spheres[1]=Sphere3(m_center,m_radius);
	m_spheres[2]=Sphere3(m_center+zinc,m_radius);
	m_spheres[0]=Sphere3(m_center-zinc,m_radius);
	}

Vector3 Snowman::getCenter()
	{
	return m_spheres[1].m_center;
	}

void Snowman::separateFrom(const AABB3& box,const Vector3& vel)
	{
	UNUSED(vel);
	calcOverlapRegionCode(box);
	
	bool shouldDoBreak=true;
	bool shouldGoStraightUp=false;

	switch (m_overlapRegionCode)
		{
		case 0x111:
			shouldGoStraightUp=true;
			separateFromInsideBox(box,vel, shouldGoStraightUp);
/*			break;*/
		case 0x211: case 0x121: case 0x112: case 0x011: case 0x101: case 0x110:
// 			separateFromBoxFaces(box,vel);
// 			shouldDoBreak=false;
// 			break;
			 //1           2           3            4        5               6        7          8         9             10            11          12
		case 0x001: case 0x010: case 0x012: case 0x021: case 0x100: case 0x102: case 0x120: case 0x122: case 0x201: case 0x210: case 0x212: case 0x221: 
// 			separateFromBoxEdges(box,vel);
// 			shouldDoBreak=false;
// 			break;
 		case 0x222: case 0x000: case 0x020: case 0x002: case 0x200: case 0x202: case 0x022: case 0x220: 
			separateFromOutsideBoxGeneral(box,vel);
			shouldDoBreak=false;
			//separateFromBoxCorners(box,vel);

// 			break;
		}
	while(m_overlapRegionCode==0x001)
		break;
	}

bool Snowman::separateFromOutsideBoxGeneral(const AABB3& box,const Vector3& vel)
	{
	UNUSED(vel);
	Vector3 dispsToOutside[3];
	Vector3 separationPoints[3];
	float minDistSquared=m_radius*m_radius;
	int indexOfClosest=-1;

	for (int i=0;i<3;i++)
		{
		separationPoints[i]=box.getClosestPointInBoxTo(m_spheres[i].m_center);
		dispsToOutside[i]=m_spheres[i].m_center-separationPoints[i];
		float distSquared=dispsToOutside[i].calcLengthSquared();
		if (distSquared<minDistSquared)
			{
			indexOfClosest=i;
			minDistSquared=distSquared;
			}
		}
	if (indexOfClosest==-1)
		return false;
	Vector3 disp=dispsToOutside[indexOfClosest];
	Vector3 separationPoint=separationPoints[indexOfClosest];
	Vector3 newCenter;
	Vector3 radiusDisp=disp.getVectorOfLength(m_radius);
	newCenter=separationPoint+radiusDisp; 
	switch(indexOfClosest)
		{
		case 0: case 2:
			newCenter.z+=(m_center.z-m_spheres[indexOfClosest].m_center.z); break;
		}
	Vector3 centerDisp=newCenter-m_center;
	reCenter(newCenter);
	return true;
	}

void Snowman::separateFromInsideBox(const AABB3& box,const Vector3& vel, bool shouldGoStraightUp)
	{
	if(m_overlapRegionCode!=0x111)
		return;
	for (int i=0;i<3;i++)
		{
		if (box.isPointInside(m_spheres[i].m_center))
			{
			if (shouldGoStraightUp)
				{
				Vector3 snowmanBottomPoint=m_center-Vector3(0.f,0.f,m_halfHeight);
				Vector3 boxTopPoint=Vector3(snowmanBottomPoint.x,snowmanBottomPoint.y,box.m_maxs.z);
				Vector3 disp=boxTopPoint-snowmanBottomPoint;
				Vector3 newCenter=m_center+disp;
				reCenter(newCenter);
				}
			else
				{
				LineSegment3 line(m_spheres[i].m_center,m_spheres[i].m_center-vel);
				Vector3 normal=box.getFaceIntersectionNormal(line);
				if (normal!=Vector3::ZERO)
					{
					Vector3 intersectionPoint=box.calcConfirmedFaceIntersectionPointUsingFaceNormal(line,normal);
					Vector3 newCenter=intersectionPoint+m_spheres[1].m_center-m_center;
					Vector3 centerDisp=newCenter-m_center;
					float dispLength=centerDisp.calcLength();
					dispLength+=m_radius;
					centerDisp.setLength(dispLength);
					newCenter=m_center+centerDisp;
					reCenter(newCenter);
					}
				}
			}
		}
	}

bool Snowman::separateFromBoxFaces(const AABB3& box,const Vector3& /*vel*/)
	{
	bool result=false;

	Vector3 newCenter=m_spheres[1].m_center;

	switch (m_overlapRegionCode)
		{
		case 0x011:
			result=true;
			newCenter.x=box.m_mins.x-m_radius;
			break;
		case 0x211:
			result=true;
			newCenter.x=box.m_maxs.x+m_radius;
			break;
		case 0x101:
			result=true;
			newCenter.y=box.m_mins.y-m_radius;
			break;
		case 0x121:
			result=true;
			newCenter.y=box.m_maxs.y+m_radius;
			break;
		case 0x110:
			result=true;
			newCenter.z=box.m_mins.z-m_halfHeight;
			break;
		case 0x112:
			result=true;
			newCenter.z=box.m_maxs.z+m_halfHeight;
			break;
		}


	if(result)
		reCenter(newCenter);
	
	return result;
	}

bool Snowman::separateFromBoxEdges(const AABB3& box,const Vector3& /*vel*/)
	{
	bool result=false;

	float distToEastFace=box.m_mins.x-m_spheres[1].m_center.x;
	float distToWestFace=m_spheres[1].m_center.x-box.m_maxs.x;
	float distToSouthFace=box.m_mins.y-m_spheres[1].m_center.y;
	float distToNorthFace=m_spheres[1].m_center.y-box.m_maxs.y;
	float distToUpFace=box.m_mins.z-m_spheres[0].m_center.z;
	float distToDownFace=m_spheres[2].m_center.z-box.m_maxs.z;

	Vector3 dispToSouthEastEdge=Vector3(distToEastFace,distToSouthFace,0.f);
	Vector3 dispToSouthWestEdge=Vector3(distToWestFace,distToSouthFace,0.f);
	Vector3 dispToNorthEastEdge=Vector3(distToEastFace,distToNorthFace,0.f);
	Vector3 dispToNorthWestEdge=Vector3(distToWestFace,distToNorthFace,0.f);
	Vector3 dispToUpEastEdge=Vector3(distToEastFace,0.f,distToUpFace);
	Vector3 dispToUpWestEdge=Vector3(distToWestFace,0.f,distToUpFace);
	Vector3 dispToUpNorthEdge=Vector3(0.f,distToNorthFace,distToUpFace);
	Vector3 dispToUpSouthEdge=Vector3(0.f,distToSouthFace,distToUpFace);
	Vector3 dispToDownEastEdge=Vector3(distToEastFace,0.f,distToDownFace);
	Vector3 dispToDownWestEdge=Vector3(distToWestFace,0.f,distToDownFace);
	Vector3 dispToDownNorthEdge=Vector3(0.f,distToNorthFace,distToDownFace);
	Vector3 dispToDownSouthEdge=Vector3(0.f,distToSouthFace,distToDownFace);

	Vector3 newCenter=m_spheres[1].m_center;
// 
// 	float radiusSquared=m_radius*m_radius;
	
	switch(m_overlapRegionCode)
		{
		case 0x201:		
			result=true; newCenter+=dispToSouthEastEdge.getVectorOfLength(m_radius);	break;
		case 0x001: 
			result=true; newCenter+=dispToSouthWestEdge.getVectorOfLength(m_radius);		break;
		case 0x221:
			result=true; newCenter+=dispToNorthEastEdge.getVectorOfLength(m_radius);		break;
		case 0x021:
			result=true; newCenter+=dispToNorthWestEdge.getVectorOfLength(m_radius);		break;
		case 0x212:
			result=true; newCenter+=dispToUpEastEdge.getVectorOfLength(m_radius);		break;
		case 0x012:
			result=true; newCenter+=dispToUpWestEdge.getVectorOfLength(m_radius);		break;
		case 0x122: 
			result=true; newCenter+=dispToUpNorthEdge.getVectorOfLength(m_radius);		break;
		case 0x102:
			result=true; newCenter+=dispToUpSouthEdge.getVectorOfLength(m_radius);		break;
		case 0x210:
			result=true; newCenter+=dispToDownEastEdge.getVectorOfLength(m_radius);		break;
		case 0x010:
			result=true; newCenter+=dispToDownWestEdge.getVectorOfLength(m_radius);		break;
		case 0x120:
			result=true; newCenter+=dispToDownNorthEdge.getVectorOfLength(m_radius);		break;
		case 0x100:
			result=true; newCenter+=dispToDownSouthEdge.getVectorOfLength(m_radius);		break;
		}


	if (result)
		reCenter(newCenter);

	return result;
	}

bool Snowman::separateFromBoxCorners(const AABB3& box,const Vector3& /*vel*/)
	{
	bool result=false;

	float distToEastFace=box.m_mins.x-m_spheres[1].m_center.x;
	float distToWestFace=m_spheres[1].m_center.x-box.m_maxs.x;
	float distToSouthFace=box.m_mins.y-m_spheres[1].m_center.y;
	float distToNorthFace=m_spheres[1].m_center.y-box.m_maxs.y;
	float distToUpFace=box.m_mins.z-m_spheres[0].m_center.z;
	float distToDownFace=m_spheres[2].m_center.z-box.m_maxs.z;

	Vector3 dispToSouthEastUpCorner  =Vector3(distToEastFace,distToSouthFace,distToUpFace);
	Vector3 dispToSouthEastDownCorner=Vector3(distToEastFace,distToSouthFace,distToDownFace);
	Vector3 dispToSouthWestUpCorner  =Vector3(distToWestFace,distToSouthFace,distToUpFace);
	Vector3 dispToSouthWestDownCorner=Vector3(distToWestFace,distToSouthFace,distToDownFace);
	Vector3 dispToNorthEastUpCorner  =Vector3(distToEastFace,distToNorthFace,distToUpFace);
	Vector3 dispToNorthEastDownCorner=Vector3(distToEastFace,distToNorthFace,distToDownFace);
	Vector3 dispToNorthWestUpCorner  =Vector3(distToWestFace,distToNorthFace,distToUpFace);
	Vector3 dispToNorthWestDownCorner=Vector3(distToWestFace,distToNorthFace,distToDownFace);

	Vector3 newCenter=m_spheres[1].m_center;

	float radiusSquared=m_radius*m_radius;

	if (distToEastFace>0 && distToSouthFace>0 && distToUpFace>0 && dispToSouthEastUpCorner.calcLengthSquared()>radiusSquared)
		{
		result=true; newCenter+=dispToSouthEastUpCorner.getVectorOfLength(m_radius);
		}
	if (distToWestFace>0 && distToSouthFace>0 && distToUpFace>0 && dispToSouthWestUpCorner.calcLengthSquared()>radiusSquared)
		{
		result=true; newCenter+=dispToSouthWestUpCorner.getVectorOfLength(m_radius);
		}
	if (distToEastFace>0 && distToNorthFace>0 && distToUpFace>0 && dispToSouthWestUpCorner.calcLengthSquared()>radiusSquared)
		{
		result=true; newCenter+=dispToNorthEastUpCorner.getVectorOfLength(m_radius);
		}
	if (distToWestFace>0 && distToNorthFace>0 && distToUpFace>0 && dispToSouthWestUpCorner.calcLengthSquared()>radiusSquared)
		{
		result=true; newCenter+=dispToNorthWestUpCorner.getVectorOfLength(m_radius);
		}
	if (distToEastFace>0 && distToSouthFace>0 && distToDownFace>0 && dispToSouthEastDownCorner.calcLengthSquared()>radiusSquared)
		{
		result=true; newCenter+=dispToSouthEastDownCorner.getVectorOfLength(m_radius);
		}
	if (distToWestFace>0 && distToSouthFace>0 && distToDownFace>0 && dispToSouthWestDownCorner.calcLengthSquared()>radiusSquared)
		{
		result=true; newCenter+=dispToSouthWestDownCorner.getVectorOfLength(m_radius);
		}
	if (distToEastFace>0 && distToNorthFace>0 && distToDownFace>0 && dispToSouthWestDownCorner.calcLengthSquared()>radiusSquared)
		{
		result=true; newCenter+=dispToNorthEastDownCorner.getVectorOfLength(m_radius);
		}
	if (distToWestFace>0 && distToNorthFace>0 && distToDownFace>0 && dispToSouthWestDownCorner.calcLengthSquared()>radiusSquared)
		{
		result=true; newCenter+=dispToNorthWestDownCorner.getVectorOfLength(m_radius);
		}

	if (result)
		reCenter(newCenter);

	return result;
	}

void Snowman::calcOverlapRegionCode(const AABB3& box)
	{
	unsigned char xs=0,ys=0,zs=0;

	//side-to-side
	if (m_spheres[1].m_center.x>box.m_maxs.x)			xs=0x2;
	else if (m_spheres[1].m_center.x<box.m_mins.x)		xs=0x0;
	else												xs=0x1;
														   
	//forward-and-backward								   
	if (m_spheres[1].m_center.y>box.m_maxs.y)			ys=0x2;
	else if (m_spheres[1].m_center.y<box.m_mins.y)		ys=0x0;
	else												ys=0x1;
														   
	//vertical											   
	if (m_spheres[0].m_center.z>box.m_maxs.z)			zs=0x2;
	else if (m_spheres[2].m_center.z<box.m_mins.z)		zs=0x0;
	else												zs=0x1;

	//combine
	m_overlapRegionCode=(xs<<8)|(ys<<4)|(zs);
	}

void Snowman::operator-=(const Vector3& antiTranslation)
	{
	translate(Vector3(-antiTranslation.x,-antiTranslation.y,-antiTranslation.z));
	}

void Snowman::operator+=(const Vector3& translation)
	{
	translate(translation);
	}

Snowman Snowman::operator-(const Vector3& antiTranslation) const
	{
	Snowman result(*this);
	result-=antiTranslation;
	return result;
	}

Snowman Snowman::operator+(const Vector3& translation) const
	{
	Snowman result(*this);
	result+=translation;
	return result;
	}

void Snowman::addPadding(float paddingRadius)
	{
	m_radius+=paddingRadius;
	m_halfHeight+=paddingRadius;
	reCenter(m_center);
	}
