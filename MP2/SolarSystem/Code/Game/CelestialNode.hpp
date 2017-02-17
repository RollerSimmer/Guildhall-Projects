#pragma once
#include "Game/SceneNode.hpp"
#include "Engine/Renderer/Rgba.hpp"

class CelestialNode: public SceneNode
	{
	public:
		CelestialNode(CelestialNode* parent,const std::string& name,const Vector3& relPos,
					  const Vector3& rotAxis,const Rgba& color,float rotAngularVel,float radius,float orbitAngularVel,
					  const Vector3& orbitalCenter,const Vector3& orbitalPlaneX,const Vector3& orbitalPlaneY,const Vector2& ijDistances) ;

		void rotate(float deltaSeconds);
		void orbit(float deltaSeconds);
		virtual void update(float deltaSeconds);
		virtual void render() const;
		void calcOrbitalPlane();

		Matrix4 getRotationMatrix() { return m_rotMatrix; }
		Matrix4 getOrbitalMatrix() { return m_orbitMatrix; }
		Vector3 getRotationAxis() { return m_rotAxis; }
		float getRotationAngleDegrees() { return m_rotDegrees; }
		Vector3 getRelativePosition() { return m_relPos; }
		float getRadius() { return m_radius; }
		Vector3 calcAbsolutePosition();
		Rgba getColor() { return m_color; }

	protected: 
		Rgba m_color;
		Vector3 m_relPos;
		Vector3 m_rotAxis;
		float m_rotAngularVel;
		float m_rotDegrees;
		float m_radius;
		Vector3 m_orbitalCenter;
		Vector3 m_orbitalPlaneX;
		Vector3 m_orbitalPlaneY;
		float m_orbitDegrees;
		float m_orbitAngularVel;
		Vector2 m_ijDistances;
		Matrix4 m_rotMatrix;
		Matrix4 m_orbitMatrix;
	};