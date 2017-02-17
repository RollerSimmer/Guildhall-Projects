#pragma once

#include "Engine/Math/Vector3.hpp"
class AABB3;

class LineSegment3
	{
	public: // constructors and destructors
		LineSegment3();
		LineSegment3(const Vector3& startpoint, const Vector3& endpoint);
	public: // methods
		AABB3 findBoundingBox() const;
		void swapStartAndEnd();
		void rotateAroundStart(float angleDegrees, const Vector3& axis);
		Vector3 makeIntoVector() const;
		float dotProduct(const LineSegment3& other);
		float calcHeadingDegreesCustomAxis(const Vector3& axis);
		float calcLength() const;
		float calcLengthSquared() const;
		Vector3 calcCenter() const;
		void translate(Vector3 offset);
		void scaleFromCenter(float scale);
		void scaleFromStart(float scale);
		Vector3 calcMidpoint();
		Vector3 calcVector() const;
		void operator *=(float scale);
		void scaleBy(float scale);	
		void operator +=(const LineSegment3& other);
	public:
		friend LineSegment3 interpolate(float scaleA, const LineSegment3& lineA,float scaleB, const LineSegment3& lineB);
	public:
		Vector3 m_start;
		Vector3 m_end;
	};

