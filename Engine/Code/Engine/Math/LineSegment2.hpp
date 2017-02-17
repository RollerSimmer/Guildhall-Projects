#pragma once

#include "Engine/Math/Vector2.hpp"
//#include "Engine/Math/AABB2.hpp"
class AABB2;

class LineSegment2
	{
	public:
		Vector2 m_start;
		Vector2 m_end;
	public: // constructors and destructors
		LineSegment2();
		LineSegment2(const Vector2& startpoint, const Vector2& endpoint);
		LineSegment2(float x1,float y1,float x2,float y2);
	public: // methods
		AABB2 findBoundingBox() const;
		bool doesIntersectWith(Vector2& poi, const LineSegment2& otherline) const;
		bool doesAABBIntersectWithAABBOf(const LineSegment2& otherline) const;
		void swapStartAndEnd();
		void rotateAroundStart(float angleDegrees);
		Vector2 makeIntoVector() const;
		float dotProduct(const LineSegment2& other);
		float calcHeadingDegrees();
		float calcLength() const;
		float calcLengthSquared();
		void translate(Vector2 offset);
		void scaleFromCenter(float scale);
		void scaleFromStart(float scale);
		Vector2 calcMidpoint();
		Vector2 calcVector() const;
	};


