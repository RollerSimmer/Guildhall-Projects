#include "Game/Particle.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtil.hpp"

Particle::Particle(int indexIntoGameArray, const Vector2& pos, const Vector2& vel, float radius, float lifespanSeconds, const Rgba& color) 
	: Entity(indexIntoGameArray,ENTITY_TYPE_PARTICLE,pos,radius,0.f)
	, m_lifespanSeconds(lifespanSeconds)
	, m_ageSeconds(0.f)
	{
	m_pos=pos;
	m_vel=vel;
	m_baseTexture=nullptr;
	m_primaryColor=m_secondaryColor=color;
	m_cosmeticRadius=radius;
	m_physicsRadius=0.f;
	m_orientationDegrees=m_goalOrientationDegrees=0.f;
	m_mass=1.f;
	m_accel=Vector2::ZERO;
	m_explosionSize=0.f;
	m_explosionSound = MISSING_SOUND_ID;
	m_shouldDiscardOnDeath=true;
	m_shouldExplodeOnDeath=false;
	m_hitPoints=1;
	m_hitPointCapacity=1;
	m_canOverlap=true;
	}

void Particle::render() const
	{
	int amtEdges=clampInt(16*(int)(m_cosmeticRadius*0.25f),16,90);
	Rgba color=m_primaryColor;
	float alphaScale = calcAgeScale();
	color.scaleAlpha(alphaScale);
// 	g_theRenderer->drawFilledDisc2(m_pos.x, m_pos.y, m_cosmeticRadius, amtEdges, color);
	g_theRenderer->drawLiquidDisc2(m_pos.x, m_pos.y, m_cosmeticRadius, amtEdges, color);
	}

void Particle::update(float deltaSeconds)
	{
	updateStart(deltaSeconds);
	move(deltaSeconds);
	changeAge(deltaSeconds);
	updateEnd(deltaSeconds);
	}

void Particle::accelerate(float /*deltaSeconds*/)
	{
	// placeholder
	}

void Particle::move(float deltaSeconds)
	{
	Entity::move(deltaSeconds);
	}

bool Particle::isDead() const
	{
	bool result = m_ageSeconds>=m_lifespanSeconds;
	while(result)
		break;
	return result;
	}

bool Particle::justDied() const
	{
	return isDead();
	}

void Particle::changeAge(float deltaSeconds)
	{
	m_ageSeconds+=deltaSeconds;
	}

float Particle::calcAgeScale() const
	{
	float x = m_ageSeconds/m_lifespanSeconds;
	float tinv=1-x;
	int exp=2;
	float tbell= bellCurveN(x,exp);
	float result = 0.5f*tinv + 0.5f*tbell;
	result=clampFloat(result,0.f,1.f);
	return result;
	}

