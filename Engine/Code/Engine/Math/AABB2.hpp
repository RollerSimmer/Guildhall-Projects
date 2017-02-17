#pragma once

#include "Engine/Math/Vector2.hpp"
//#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/AABB2.hpp"

class Disc2;
class LineSegment2;

enum BoxEdge
	{
	BOX_EDGE_TOP,
	BOX_EDGE_LEFT,
	BOX_EDGE_BOTTOM,
	BOX_EDGE_RIGHT,
	BOX_EDGE_NONE,
	AMT_BOX_EDGE_VALUES
	};

class AABB2
	{
	public:
		Vector2 m_mins;
		Vector2 m_maxs;
	public: // constructors
		AABB2();
		explicit AABB2(const Vector2& newmins, const Vector2& newmaxs);
		explicit AABB2(float xmin, float ymin, float xmax, float ymax);
		explicit AABB2(const Vector2& center, float xradius, float yradius);
	public: // accessors and mutators
		void setMinMax(const Vector2& newMins, const Vector2& newMaxs);
		void setXYs(float x1, float y1, float x2, float y2);
		void getBounds(float& l, float& t, float& r, float& b);
	public: // methods
		BoxEdge getIntersectionEdge(const LineSegment2& line);
		bool doesIntersectWithEdge(const LineSegment2& line, BoxEdge edge);
		Vector2 GenerateEdgePoint(float objradius);
		bool doesOverlapWith(const AABB2& otherbox) const;
		bool doesOverlapWith(const Disc2& somedisc) const;
		bool doesOverlapWith(const Disc2& somedisc,Axis2& axisOfHit) const;
		AABB2 createdWorldExpandedBy(float radius);
		void stretchToIncludePoint(const Vector2& point);
		void addPaddingToSides(float xPaddingRadius, float yPaddingRadius);
		void translate(const Vector2& translation);
		bool IsPointInside(const Vector2& point) const;
		const Vector2 CalcSize() const; // or perhaps CalcDimensions
		const Vector2 CalcCenter() const;
		float calcWidth();
		float calcHeight();
		Vector2 getRandomPointInBox();
		Vector2 getClosestEdgePointTo(float x,float y) const;
		Vector2 getClosestEdgePointTo(const Vector2& pos) const;
		Vector2 getClosestInteriorPointTo(const Vector2& pos) const;
		bool doesContainPoint(const Vector2& point) const;
	public: // operators
		AABB2 operator * (float scale);
		AABB2 operator + (const AABB2& other) const;
		const AABB2 operator + (const Vector2& translation) const;
		const AABB2 operator - (const Vector2& antiTranslation) const;
		const AABB2& operator += (const Vector2& translation);
		const AABB2& operator -= (const Vector2& antiTranslation);
		bool operator == (const AABB2& other) const;
	public: // friend functions 
		friend bool DoAABBsOverlap(const AABB2& a, const AABB2& b);
		friend AABB2 makeAABB2(float x1, float y1, float x2, float y2);
		friend AABB2 makeAABB2(const Vector2& newMins, const Vector2& newMaxs);
		friend AABB2 interpolate(float scaleA, const AABB2& A, float scaleB, const AABB2& B);
	private:
		bool doesIntersectWithTopEdge(const LineSegment2& line);
		bool doesIntersectWithBottomEdge(const LineSegment2& line);
		bool doesIntersectWithLeftEdge(const LineSegment2& line);
		bool doesIntersectWithRightEdge(const LineSegment2& line);
		static bool doesIntersectWithAxialLine(float a, float bstart, float bend, float lineAStart, float lineAEnd, float lineBStart, float lineBEnd);
	};

