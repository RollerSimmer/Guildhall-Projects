#pragma once

#include "Engine/Math/Vector3.hpp"
class LineSegment3;


class Sphere3;

class AABB3
	{
	public:
		// constructor
		explicit AABB3(const Vector3& mins, const Vector3& maxs);
		AABB3();
		// methods
		void setXYs(float x1, float y1, float z1,float x2, float y2,float z2);
		bool doesOverlapWith(const AABB3& otherbox) const;
		bool doesOverlapWith(const Sphere3& somesphere) const;
		void stretchToIncludePoint(const Vector3& point);
		void addPaddingToSides(const Vector3& padding);
		void translate(const Vector3& translation);
		bool isPointInside(const Vector3& point) const;
		Vector3 calcDimensions() const; // or perhaps CalcDimensions
		Vector3 calcCenter() const;
		float calcWidth() const;
		float calcHeight() const;
		float calcDepth() const;
		Vector3 getRandomPointInBox();
		Vector3 getClosestPointInBoxTo(const Vector3& point) const;
		bool doesContainPoint(const Vector3& point) const;
		Vector3 getFaceIntersectionNormalForXAxis(const LineSegment3& line) const;
		Vector3 getFaceIntersectionNormalForYAxis(const LineSegment3& line) const;
		Vector3 getFaceIntersectionNormalForZAxis(const LineSegment3& line) const;
		Vector3 getFaceIntersectionNormal(const LineSegment3& line) const;
		Vector3 calcConfirmedFaceIntersectionPointUsingFaceNormal(const LineSegment3& line,const Vector3& normal) const;
		// operators
		AABB3 operator * (float scale) const;
		AABB3 operator + (const AABB3& other) const;
		AABB3 operator + (const Vector3& translation) const;
		AABB3 operator - (const Vector3& antiTranslation) const;
		void operator += (const Vector3& translation);
		void operator -= (const Vector3& antiTranslation);
		bool operator == (const AABB3& other) const;
	public:
		friend bool doAABBsOverlap(const AABB3& a, const AABB3& b);
		friend AABB3 interpolate(float scaleA, const AABB3& A, float scaleB, const AABB3& B);
	public:
		Vector3 m_mins;
		Vector3 m_maxs;
	};

