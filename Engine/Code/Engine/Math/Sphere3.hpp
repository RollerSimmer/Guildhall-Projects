#pragma once

#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"

// TODO: implement AABB3 class later


struct DiscAndAABB2
	{
	Disc2 m_disc;
	AABB2 m_box;
	bool m_discInit;
	bool doMembersOverlap();
	};

class Sphere3
	{
	public:
		static const Sphere3 UNIT_SPHERE;
	public: // constructors and destructors
		Sphere3();
		Sphere3(const Sphere3& copy);
		explicit Sphere3(float initialX, float initialY, float initialZ, float initialRadius);
		explicit Sphere3(const Vector3& initialCenter, float initialRadius);
	public: // accessors and mutators
		void setXYsAndRadius(float xcenter, float ycenter, float zcenter, float radius);
	public: // methods
		AABB3 makeCubeAroundDisc() const;
		void stretchToIncludePoint(const Vector3& point);
		void addPadding(float paddingRadius);
		void translate(const Vector3& translation);
		bool isPointInside(const Vector3& point) const;
		bool doesOverlapWith(const Sphere3& other) const;
		bool doesOverlapWith(const AABB3& other) const;
		bool getIntersectionWithXPlane(float x, Disc2& retdisc) const;
		bool getIntersectionWithYPlane(float y, Disc2& retdisc) const;
		bool getIntersectionWithZPlane(float z, Disc2& retdisc) const;
		void scaleBy(float scale);

	public: // operators
		Sphere3 operator + (const Vector3& translation) const;
		Sphere3 operator - (const Vector3& antiTranslation) const;
		void operator += (const Vector3& translation);
		void operator -= (const Vector3& antiTranslation);

	public: //friend functions
		friend bool doSpheresOverlap(const Sphere3& a, const Sphere3& b);
		friend bool doSpheresOverlap(const Vector3& aCenter, float aRadius,
			const Vector3& bCenter, float bRadius);
		friend Sphere3 interpolate(float scaleA,const Sphere3& A,float scaleB,const Sphere3& B);

	public:
		Vector3 m_center;
		float m_radius;
	private:
		bool doesOverlapWithFaceChecks(const AABB3& other) const;
		void calcFaceIntersectionRegions(const AABB3& box, DiscAndAABB2& top, DiscAndAABB2& bottom,
										DiscAndAABB2& left, DiscAndAABB2& right,
										DiscAndAABB2& front, DiscAndAABB2& back) const;
	};

