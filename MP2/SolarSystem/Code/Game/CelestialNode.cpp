#include "Game/CelestialNode.hpp"
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Game/GameCommon.hpp"


CelestialNode::CelestialNode(CelestialNode* parent,const std::string& name,const Vector3& relPos, const Vector3& rotAxis,
							 const Rgba& color,float rotAngularVel,float radius, float orbitAngularVel, 
							 const Vector3& orbitalCenter, const Vector3& orbitalPlaneX, const Vector3& orbitalPlaneY,const Vector2& ijDistances) 
	:	SceneNode(name,parent),
		m_color(color),
		m_relPos(relPos),
		m_rotAxis(rotAxis),
		m_rotAngularVel(rotAngularVel),
		m_radius(radius),
		m_orbitalCenter(orbitalCenter),
		m_orbitalPlaneX(orbitalPlaneX),
		m_orbitalPlaneY(orbitalPlaneY),
		m_ijDistances(ijDistances),
		m_rotDegrees(0.f),
		m_orbitDegrees(0.f),
		m_orbitAngularVel(orbitAngularVel)
	{
	calcOrbitalPlane();
	m_orbitMatrix.makeIdentity();
	m_rotMatrix.makeIdentity();
	}

void CelestialNode::rotate(float deltaSeconds)
	{
	float rotInc=m_rotAngularVel*deltaSeconds;
	Vector3 translation=Vector3::ZERO;
	Matrix4 quatRot=Matrix4::calcQuaternionRotationMatrix(m_rotAxis,translation,rotInc);
	m_rotMatrix=m_rotMatrix.getTransformed(quatRot);
	m_rotDegrees += rotInc;
	m_rotDegrees = fixDegrees180(m_rotDegrees);
	}

void CelestialNode::orbit(float deltaSeconds)
	{
	float orbitInc=m_orbitAngularVel*deltaSeconds;
	float i=m_ijDistances.x;
	float j=m_ijDistances.y;
	m_orbitDegrees += orbitInc;
	m_orbitDegrees=fixDegrees180(m_orbitDegrees);
	Vector3 translation = m_orbitalPlaneX*i*cosDegrees(m_orbitDegrees) + m_orbitalPlaneY*j*sinDegrees(m_orbitDegrees);
	m_relPos=translation;
	float rotInc=0.f;
	Vector3 rotAxis=Vector3::Z_AXIS_VECTOR;
	Matrix4	orbitMatrix=Matrix4::calcQuaternionRotationMatrix(rotAxis,translation,rotInc);
	m_orbitMatrix=m_orbitMatrix.getTransformed(orbitMatrix);
	}

void CelestialNode::update(float deltaSeconds)
	{
	UNUSED(deltaSeconds);
	}

void CelestialNode::render() const
	{
	Sphere3 sphere(Vector3::ZERO,m_radius);
	float lineThickness=3.f;
	g_theRenderer->drawWireframeSphere3(sphere,m_color,lineThickness);
	}

void CelestialNode::calcOrbitalPlane()
	{
	Vector3 orbitalPlaneZ=m_orbitalPlaneX.crossProduct(m_orbitalPlaneY);
	m_orbitalPlaneY=orbitalPlaneZ.crossProduct(m_orbitalPlaneX);
	m_orbitalPlaneX.normalize();
	m_orbitalPlaneY.normalize();
	}

Vector3 CelestialNode::calcAbsolutePosition()
	{
	Vector3 result=Vector3::ZERO;
	CelestialNode* node=this;
	while (node)
		{
		result+=node->m_relPos;
		node=(CelestialNode*)node->m_parent;
		}
	return result;
	}
