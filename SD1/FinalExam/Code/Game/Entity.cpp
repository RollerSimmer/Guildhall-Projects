#include "Game/Entity.hpp"
#include <cmath>
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/Disc2.hpp"

Entity::Entity(const Vector2& pos,EntityType etype, bool isMounted,  float radius,float speed,int hpCapacity, bool canOverlap) : m_type(etype)
	, m_pos(pos)
	, m_vel(0.f, 0.f)

	, m_orientationDegrees(0.f)
	, m_isMounted(isMounted)
	, m_canOverlap(canOverlap)

	, m_hitPoints(hpCapacity)
	, m_hitPointCapacity(hpCapacity)

	, m_cosmeticRadius(radius)
	, m_physicsRadius(radius*GRAPHICS_TO_PHYSICS_RADIUS_RATIO)
	, m_outOfBoundsCounterSeconds(0.f)
	, m_drawScale(1.f)
	, m_movingSpriteTexture(nullptr)

	,m_spinRateDegrees(0.f)
	,m_isSpinning(false)
	{
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
	m_cosmeticRadius=radius;
	m_physicsRadius=radius*GRAPHICS_TO_PHYSICS_RADIUS_RATIO;
	}

float Entity::getGraphicsRadius()
	{
	return m_cosmeticRadius;
	}

Vector2 Entity::getVelocity()
	{
	return m_vel;
	}

float Entity::getSpeed()
	{
	Vector2 vel=getVelocity();
	return vel.CalcLength();
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
	drawMovingSprite();
	}


void Entity::drawMovingSprite() const
	{
	if (m_movingSpriteTexture)
		{
		Rgba color = m_color;
		AABB2 bounds(m_pos,m_cosmeticRadius,m_cosmeticRadius);
		g_theRenderer->drawTexturedAABB2(bounds,*m_movingSpriteTexture,Vector2(0.f,1.f),Vector2(1.f,0.f),color,
										 m_orientationDegrees);
		}
	}


void Entity::pushAwayFrom(const AABB2& forbiddenZone)
	{
	Vector2 edgePoint = forbiddenZone.getClosestEdgePointTo(m_pos.x,m_pos.y);
	Vector2 displacement = m_pos - edgePoint;
	if (displacement.CalcLengthSquared() == 0.f)
		{
		if (edgePoint.x==forbiddenZone.m_mins.x)
			displacement.x=-m_physicsRadius;
		else if (edgePoint.x==forbiddenZone.m_maxs.x)
			displacement.x=m_physicsRadius;
		if (edgePoint.y==forbiddenZone.m_mins.y)
			displacement.y=-m_physicsRadius;
		else if (edgePoint.y==forbiddenZone.m_maxs.y)
			displacement.y=m_physicsRadius;
		if (displacement.CalcLengthSquared()==0.f) // still?
			displacement.x=m_physicsRadius;
		}
	else if (forbiddenZone.doesContainPoint(m_pos))
		{
		displacement=-displacement;
		}
	displacement.setLength(m_physicsRadius*1.01f);
	m_pos = edgePoint + displacement;
	}


void Entity::processCollisionWithOther(Entity& /*other*/)
	{
	
	}

void Entity::drawGraphicsShell() const
	{
	const Rgba GRAPHICS_SHELL_COLOR = Rgba::BLUE;
	g_theRenderer->drawCircle(m_pos.x, m_pos.y, m_cosmeticRadius*m_drawScale, AMT_BOUNDS_CIRCLE_EDGES, GRAPHICS_SHELL_COLOR);
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
	updateSpin(deltaSeconds);
	//wrapAround();
	}

bool Entity::isCompletelyOutOfBounds()
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

bool Entity::isPartiallyOutOfBounds()
	{
	AABB2 contractedWorld = g_theWorldBounds->createdWorldExpandedBy(-m_cosmeticRadius);
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
	if (m_pos.x < g_theWorldBounds->m_mins.x - m_cosmeticRadius)
		m_pos.x = g_theWorldBounds->m_maxs.x + m_cosmeticRadius;
	else if (m_pos.x > g_theWorldBounds->m_maxs.x + m_cosmeticRadius)
		m_pos.x = g_theWorldBounds->m_mins.x - m_cosmeticRadius;
	if (m_pos.y < g_theWorldBounds->m_mins.y - m_cosmeticRadius)
		m_pos.y = g_theWorldBounds->m_maxs.y + m_cosmeticRadius;
	else if (m_pos.y > g_theWorldBounds->m_maxs.y + m_cosmeticRadius)
		m_pos.y = g_theWorldBounds->m_mins.y - m_cosmeticRadius;
	}

void Entity::setSpeedFromCurrentHeading(float speed)
	{
	m_vel=calcHeadingVector()*speed;
	}

void Entity::setSpeed(float speed)
	{
	m_vel.setLength(speed);
	}

Vector2 Entity::getpos() const
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

void Entity::startSpinning()
	{
	m_isSpinning=true;
	}

void Entity::stopSpinning()
	{
	m_isSpinning=false;
	}

void Entity::updateSpin(float deltaSeconds)
	{
	while(m_type==ENTITY_TYPE_GOAL)
		break;
	if (m_isSpinning)
		{
		m_orientationDegrees+=m_spinRateDegrees*deltaSeconds;
		m_orientationDegrees=fixDegrees180(m_orientationDegrees);
		}
	}
