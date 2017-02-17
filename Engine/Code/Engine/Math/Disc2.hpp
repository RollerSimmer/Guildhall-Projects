#pragma once

#include "Engine/Math/Vector2.hpp"

class AABB2;

class Disc2
	{
	public:
		static const Disc2 UNIT_CIRCLE; // Disc2D::UNIT_CIRCLE; center (0,0) and radius 1
	public:
		Vector2 m_center;
		float m_radius;
	public: // constructors and destructors
		Disc2();
		Disc2(const Disc2& copy);
		explicit Disc2(float initialX, float initialY, float initialRadius);
		explicit Disc2(const Vector2& initialCenter, float initialRadius);		
	public: // accessors and mutatorsm+
		void setXYsAndRadius(float xcenter,float ycenter,float radius);
	public: // methods
		AABB2 makeBoxAroundDisc() const;
		void StretchToIncludePoint(const Vector2& point);
		void AddPadding(float paddingRadius);
		void Translate(const Vector2& translation);
		bool IsPointInside(const Vector2& point) const;
		bool doesOverlapWith(const Disc2& other) const;
		bool doesOverlapWith(const AABB2& other) const;
		bool doesOverlapWith(const AABB2& other, Axis2& axisOfHit) const;
		void scaleBy(float scale);
	public: // operators
		const Disc2 operator + (const Vector2& translation) const;
		const Disc2 operator - (const Vector2& antiTranslation) const;
		void operator += (const Vector2& translation);
		void operator -= (const Vector2& antiTranslation);

	public: //friend functions
		friend bool DoDiscsOverlap(const Disc2& a, const Disc2& b);
		friend bool DoDiscsOverlap(const Vector2& aCenter, float aRadius,
			const Vector2& bCenter, float bRadius);
		friend Disc2 interpolate(float scaleA, const Disc2& a, float scaleB, const Disc2& b);
	};

