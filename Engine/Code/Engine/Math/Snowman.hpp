#pragma once
#include "Engine/Math/Sphere3.hpp"

class Snowman
	{
	public: // constructors
		Snowman();
		Snowman(const Snowman& copy);
		explicit Snowman(const Vector3& center, float radius, float halfHeight);
	public: //methods 
		void addPadding(float paddingRadius);
		void translate(const Vector3& translation);
		bool isPointInside(const Vector3& point) const;
		bool doesOverlapWith(const Sphere3& otherShape) const;
		bool doesOverlapWith(const AABB3& otherShape) const;
		void scaleBy(float scale);
		void reCenter(const Vector3& newCenter);
		Vector3 getCenter();
		void separateFrom(const AABB3& box,const Vector3& vel);
			bool separateFromOutsideBoxGeneral(const AABB3& box,const Vector3& vel);
			void separateFromInsideBox(const AABB3& box,const Vector3& vel, bool shouldGoStraightUp);
			bool separateFromBoxFaces(const AABB3& box,const Vector3& vel);
			bool separateFromBoxEdges(const AABB3& box,const Vector3& vel);
			bool separateFromBoxCorners(const AABB3& box,const Vector3& vel);
		void calcOverlapRegionCode(const AABB3& box);

	public: // operators
		Snowman operator + (const Vector3& translation) const;
		Snowman operator - (const Vector3& antiTranslation) const;
		void operator += (const Vector3& translation);
		void operator -= (const Vector3& antiTranslation);
			
	public:
		Sphere3 m_spheres[3];
		Vector3 m_center;
		float m_radius;
		float m_halfHeight;	


	public:
		int m_overlapRegionCode;
	};
