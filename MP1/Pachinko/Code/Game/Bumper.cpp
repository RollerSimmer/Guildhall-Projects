#include "Game/Bumper.hpp"
#include "Game/Projectile.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/PhysicsUtil.hpp"

Bumper::Bumper(const Disc2& shape) 
	: FixedObstacle()
	, m_shape(shape)
	{
	}

Bumper::~Bumper()
	{
	}

void Bumper::render() const
	{
	g_theRenderer->drawShinyDisc2(m_shape.m_center.x, m_shape.m_center.y, m_shape.m_radius, AMT_BUMPER_EDGES, Rgba::GRAY);
	g_theRenderer->drawCircle(m_shape.m_center.x, m_shape.m_center.y, m_shape.m_radius, AMT_BUMPER_EDGES, Rgba::BLACK);
	}

bool Bumper::doesCollideWith(const Projectile* projectile) const
	{
	return m_shape.doesOverlapWith(projectile->getPhysicsDisc2());
	}

void Bumper::processCollision(Projectile* projectile)
	{
	pullApartFrom(projectile);
	transferMomentum(projectile);
	}

void Bumper::pullApartFrom(Projectile* projectile)
	{
	if(projectile == nullptr)
		return;
	float radiusSum = projectile->getPhysicsRadius() + m_shape.m_radius;
	Vector2 projpos = projectile->getpos();
	float distance = CalcDistance(m_shape.m_center, projectile->getpos());
	if (distance > radiusSum)
		return;
	Vector2 displacement = projectile->getpos() - m_shape.m_center;
	displacement.Normalize();
	displacement *= 1.1f*(radiusSum - distance);
	projpos += displacement;
	projectile->setpos(projpos);
	//m_shape.m_center -= displacement;
	}

void Bumper::transferMomentum(Projectile* projectile)
	{
	Vector2 projvel=projectile->getVelocity();
	Vector2 thisvel=Vector2(0.f,0.f);
	transferMomentumGeneric(0.f,thisvel,m_shape.m_center,g_projectileElasticity,
		                    projectile->getmass(),projvel,projectile->getpos(),g_projectileElasticity);
	projectile->setVelocity(projvel);

	}

