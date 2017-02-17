
#include "Game/Bullet.hpp"
#include "Game/GameCommon.hpp"
#include <cmath>

void Bullet::getOlder(float deltaSeconds)
	{
	m_secondsLeftToLive-=deltaSeconds;
	deathCheck();
	}

void Bullet::deathCheck()
	{
	if (m_secondsLeftToLive <= 0.f)
		{
		m_isDead = true;
		m_secondsLeftToLive=0.f;
		}
	}

float Bullet::getTimeLeftToLive()
	{
	return m_secondsLeftToLive;
	}

bool Bullet::isDead()
	{
	return m_isDead;
	}

Bullet::Bullet(const Vector2& pos, Vector2& heading, float speed)
	: Entity(pos, BULLET_GRAPHICAL_RADIUS, speed)
	, m_isDead(false)
	, m_secondsLeftToLive(BULLET_LIFESPAN)
	{
	this->setVelocity(heading);
	setSpeed(speed);
	m_mass=BULLET_MASS;
	}

Bullet::Bullet(const Vector2& pos, Vector2& velocity)
	: Entity(pos, BULLET_GRAPHICAL_RADIUS, velocity.CalcLength())
	, m_isDead(false)
	, m_secondsLeftToLive(BULLET_LIFESPAN)
	{
	this->setVelocity(velocity);
	}

void Bullet::render() const
{
	Rgba bulletcolor=Rgba::GREEN;
	bulletcolor.alpha()=255;
	bulletcolor.scaleAlpha(m_secondsLeftToLive/BULLET_LIFESPAN);
	g_theRenderer->drawDisc2(m_pos.x,m_pos.y,m_graphicsRadius,AMT_BULLET_SIDES,bulletcolor);
	}

void Bullet::update(float deltaSeconds)
	{
	Entity::update(deltaSeconds);
	getOlder(deltaSeconds);
	}

