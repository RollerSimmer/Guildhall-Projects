#include "Game/Projectile.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/PhysicsUtil.hpp"

Projectile::Projectile(Vector2 pos/*=Vector2::ZERO*/,float physicsRadius/*=DEFAULT_PROJECTILE_RADIUS*/,float cosmeticRadius/*=DEFAULT_PROJECTILE_RADIUS*/)
	: m_mass(physicsRadius*physicsRadius)
	,m_pos(pos)
	,m_lastPos(pos)
	, m_vel(Vector2::ZERO)
	, m_physicsRadius(physicsRadius)
	, m_cosmeticRadius(cosmeticRadius)
	, m_color(Rgba::pick())
	{
	m_color.setLevelRange(0.0f,0.8f);
	}

void Projectile::render() const
	{

	}

void Projectile::update(float deltaSeconds)
	{
 	accelerate(deltaSeconds);
 	move(deltaSeconds);
	//calcActualVelocity(deltaSeconds);
 	wrapAround();
	m_lastPos=m_pos;
	}

void Projectile::move(float deltaSeconds)
	{
	float left=g_theWorldBounds->m_mins.x+m_physicsRadius;
	float right=g_theWorldBounds->m_maxs.x-m_physicsRadius;
	Vector2 center= g_theWorldBounds->CalcCenter();
	m_pos += m_vel*deltaSeconds;
	m_pos.x = clampFloat(m_pos.x,left,right);
// 	if(m_pos.y > center.y)
// 		m_pos.y=center.y;
	}

void Projectile::accelerate(float deltaSeconds)
	{
	Vector2 accel = Vector2(0.f,g_worldGravitationalAcceleration);
	m_vel += accel*deltaSeconds;
	float speed=m_vel.CalcLength();
 	if(speed>g_projectileTerminalSpeed)
		m_vel.SetLength(g_projectileTerminalSpeed);
	}

void Projectile::wrapAround()
	{
	if (this->isCompletelyOutOfBounds())
		this->teleportToOtherSide();
	}

bool Projectile::isCompletelyOutOfBounds()
	{
	AABB2 expandedWorld = g_theWorldBounds->createdWorldExpandedBy(m_cosmeticRadius);
	if (m_pos.x < expandedWorld.m_mins.x)
		return true;
	if (m_pos.y < expandedWorld.m_mins.y)
		return true;
	if (m_pos.x > expandedWorld.m_maxs.x)
		return true;
	if (m_pos.y > expandedWorld.m_maxs.y)
		return true;
	if (isNan(m_pos.x))
		return true;
	if (isNan(m_pos.y))
		return true;
	return false;
	}

void Projectile::teleportToOtherSide()
	{
	if (m_pos.y < g_theWorldBounds->m_mins.y - m_cosmeticRadius)
		{
		//m_pos.y = g_theWorldBounds->m_maxs.y + m_cosmeticRadius;
		}
	else if (m_pos.y > g_theWorldBounds->m_maxs.y + m_cosmeticRadius)
		m_pos.y = g_theWorldBounds->m_mins.y - m_cosmeticRadius;
	m_vel=Vector2::ZERO;
	}

void Projectile::pullApartFrom(Projectile& other)
	{
	float radiusSum=other.m_physicsRadius+m_physicsRadius;
	float distance = CalcDistance(m_pos,other.m_pos);
	if(distance>radiusSum)
		return;
	Vector2 displacement = other.m_pos - m_pos;
	displacement.Normalize();
	displacement*=0.5f*(radiusSum-distance);
	m_pos-=displacement;
	other.m_pos+=displacement;
	}

bool Projectile::doesCollideWithOtherProjectile(const Projectile& other)
	{
	float radiusSum=m_physicsRadius+other.m_physicsRadius;
	float dist=CalcDistance(m_pos,other.m_pos);
	return dist<radiusSum;
	}

void Projectile::transferMomentumWithOtherProjectile(Projectile& other)
	{
	transferMomentumGeneric(m_mass,m_vel,m_pos,g_obstacleElasticity,other.m_mass,other.m_vel,other.m_vel,g_projectileElasticity);
	}

void Projectile::scaleBy(float scale)
	{
	m_physicsRadius*=scale;
	m_cosmeticRadius*=scale;
	updateMassBasedOnPhysicsRadius();
	}

Disc2 Projectile::getPhysicsDisc2() const
	{
	return Disc2(m_pos,m_physicsRadius);
	}

void Projectile::calcActualVelocity(float deltaSeconds)
	{
	float invTimeFactor=(1.f/deltaSeconds);
	m_vel=(m_pos-m_lastPos)*invTimeFactor;
	}

void Projectile::updateMassBasedOnPhysicsRadius()
	{
	m_mass=m_physicsRadius*m_physicsRadius;
	}
