#include "Game/Entity.hpp"
#include <cmath>
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/Disc2.hpp"

Entity::Entity(const Vector2& pos,float radius,float speed)
	: m_pos(pos)
	, m_vel(0.f, 0.f)
	, m_orientationDegrees(0.f)
	, m_graphicsRadius(radius)
	, m_physicsRadius(radius*GRAPHICS_TO_PHYSICS_RADIUS_RATIO)
	, m_angularVelocity(0.f)
	, m_outOfBoundsCounterSeconds(0.f)
	, m_drawScale(1.f)
	{
	m_angularVelocity = GetRandomFloatInRange(-135.f, 135.f);
	m_vel.x = GetRandomFloatInRange(-1.f, 1.f);
	m_vel.y = GetRandomFloatInRange(-1.f, 1.f);
	this->setSpeed(speed);
	}

float Entity::getMass()
	{
	return m_mass;
	}

void Entity::setRadii(float radius)
	{
	m_graphicsRadius=radius;
	m_physicsRadius=radius*GRAPHICS_TO_PHYSICS_RADIUS_RATIO;
	}

float Entity::getGraphicsRadius()
	{
	return m_graphicsRadius;
	}

Vector2 Entity::getVelocity()
	{
	return m_vel;
	}

float Entity::getSpeed()
	{
	Vector2 vel=getVelocity();
	return vel.calcLength();
	}

float Entity::getOrientationDegrees()
	{
	return m_orientationDegrees;
	}

void Entity::render() const
	{
	if (g_doShowDebugGraphics)
		{
		drawGraphicsShell();
		drawPhysicsShell();
		drawVelocityVector();
		}
	}

void Entity::drawGraphicsShell() const
	{
	const Rgba GRAPHICS_SHELL_COLOR = Rgba::BLUE;
	g_theRenderer->drawCircle(m_pos.x, m_pos.y, m_graphicsRadius*m_drawScale, AMT_BOUNDS_CIRCLE_EDGES, GRAPHICS_SHELL_COLOR);
	}

void Entity::drawPhysicsShell() const
	{
	const Rgba PHYSICS_SHELL_COLOR = Rgba::CYAN;
	g_theRenderer->drawCircle(m_pos.x, m_pos.y, m_physicsRadius*m_drawScale, AMT_BOUNDS_CIRCLE_EDGES, PHYSICS_SHELL_COLOR);
	}

void Entity::drawVelocityVector() const
	{
	const Rgba VELOCITY_VECTOR_COLOR = Rgba::PURPLE;
	Vector2 endpoint=m_pos+m_vel*m_drawScale;
	g_theRenderer->drawLine(m_pos.x,m_pos.y,endpoint.x,endpoint.y,VELOCITY_VECTOR_COLOR);
	}

Vector2 Entity::calcMomentum()
	{
	return m_mass*m_vel;
	}

void Entity::incOutOfBoundsCounter(float deltaSeconds)
	{
	m_outOfBoundsCounterSeconds+=deltaSeconds;
	}

void Entity::resetOutOfBoundsCounter()
	{
	m_outOfBoundsCounterSeconds=0.f;
	}

bool Entity::hasBeenOutOfBoundsTooLong()
	{
	return m_outOfBoundsCounterSeconds > MAX_ENTITY_OUT_OF_BOUNDS_TIME;
	}

void Entity::adjustRotation(float degrees)
	{
	m_orientationDegrees+=degrees;
	}

void Entity::adjustPosition(const Vector2& displacement)
	{
	m_pos+=displacement;
	}

void Entity::setDrawScale(float scale)
	{
	m_drawScale=scale;
	}

void separateEntities(Entity& a, Entity& b)
	{
	Disc2 adisc(a.m_pos,a.m_physicsRadius);
	Disc2 bdisc(b.m_pos,b.m_physicsRadius);
	if(!DoDiscsOverlap(adisc,bdisc))
		return;
	Vector2 displacement=b.m_pos-a.m_pos;
	displacement.Normalize();
	float radiusSum = a.m_physicsRadius + b.m_physicsRadius;
	float dist = CalcDistance(adisc.m_center, bdisc.m_center);
	float displacementLen = (radiusSum - dist) * 0.5f;
	displacement *= displacementLen;
	b.m_pos += displacement;
	a.m_pos -= displacement;
	}

void Entity::update(float deltaSeconds)
	{
	move(deltaSeconds);
	rotate(deltaSeconds);
	wrapAround();
	}

bool Entity::isCompletelyOutOfBounds()
	{
	AABB2 expandedWorld = g_theWorldBounds->createdWorldExpandedBy(m_graphicsRadius);
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

bool Entity::isPartiallyOutOfBounds()
	{
	AABB2 contractedWorld = g_theWorldBounds->createdWorldExpandedBy(-m_graphicsRadius);
	if (m_pos.x < contractedWorld.m_mins.x)
		return true;
	if (m_pos.y < contractedWorld.m_mins.y)
		return true;
	if (m_pos.x > contractedWorld.m_maxs.x)
		return true;
	if (m_pos.y > contractedWorld.m_maxs.y)
		return true;
	if (isNan(m_pos.x))
		return true;
	if (isNan(m_pos.y))
		return true;
	return false;
	}

void Entity::teleportToOtherSide()
	{
	if (m_pos.x < g_theWorldBounds->m_mins.x - m_graphicsRadius)
		m_pos.x = g_theWorldBounds->m_maxs.x + m_graphicsRadius;
	else if (m_pos.x > g_theWorldBounds->m_maxs.x + m_graphicsRadius)
		m_pos.x = g_theWorldBounds->m_mins.x - m_graphicsRadius;
	if (m_pos.y < g_theWorldBounds->m_mins.y - m_graphicsRadius)
		m_pos.y = g_theWorldBounds->m_maxs.y + m_graphicsRadius;
	else if (m_pos.y > g_theWorldBounds->m_maxs.y + m_graphicsRadius)
		m_pos.y = g_theWorldBounds->m_mins.y - m_graphicsRadius;
	}

void Entity::setSpeedFromCurrentHeading(float speed)
	{
	m_vel=calcHeadingVector()*speed;
	}

void Entity::setSpeed(float speed)
	{
	m_vel.setLength(speed);
	}

Vector2& Entity::getpos()
	{
	return m_pos;
	}

void Entity::setVelocity(const Vector2& newVelocity)
	{
	m_vel = newVelocity;
	}

void Entity::move(float deltaSeconds)
	{
	m_pos += m_vel*deltaSeconds;
	}

void Entity::rotate(float deltaSeconds)
	{
	m_orientationDegrees += m_angularVelocity*deltaSeconds;
	m_orientationDegrees = (float)fmod(m_orientationDegrees, 360.0);
	}

void Entity::wrapAround()
	{
	if (this->isCompletelyOutOfBounds())
		this->teleportToOtherSide();
	}

Vector2 Entity::calcHeadingVector()
	{
	float radians=ConvertDegreesToRadians(m_orientationDegrees);
	Vector2 result(cos(radians),sin(radians));
	result.Normalize();
	return result;
	}

bool Entity::doesCollideWith(const Entity& other)
	{
	Disc2 thisdisc(m_pos,m_physicsRadius);
	Disc2 otherdisc(other.m_pos,other.m_physicsRadius);
	return thisdisc.doesOverlapWith(otherdisc);
	}

