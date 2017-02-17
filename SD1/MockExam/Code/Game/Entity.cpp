#include "Game/Entity.hpp"
#include <cmath>
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Game/PlayerBullet.hpp"
#include "Game/Ship.hpp"
#include "Game/EnemyShip.hpp"

Entity::Entity(int indexIntoGameArray, EntityType mytype, const Vector2& pos,float radius,float speed) 
	: m_pos(pos)
	, m_prevPos(pos)
	, m_indexIntoGameArray(indexIntoGameArray)
	, m_type(mytype)
	, m_vel(0.f, 0.f)
	, m_orientationDegrees(0.f)
	, m_goalOrientationDegrees(0.f)
	, m_cosmeticRadius(radius)
	, m_physicsRadius(radius*GRAPHICS_TO_PHYSICS_RADIUS_RATIO)
	, m_angularVelocity(0.f)
	, m_drawingScale(1.f)
	, m_isTurning(false)
	, m_turningRate(DEFAULT_ENTITY_TURNING_RATE)
	, m_accel(Vector2(0.f,0.f))
	, m_baseSpriteIndex(0)
	, m_treadTraction(0.f)
	, m_hasHealthBar(false)
	, m_secondsSinceDeath(0.f)
	, m_explosionSound (MISSING_SOUND_ID)
	, m_shouldExplodeOnDeath(false)
	, m_shouldDiscardOnDeath(false)
	, m_elevation(0.f)
	, m_isMoveable(true)
	, m_canOverlap(false)
	, m_shouldKeepInWorld(true)
	{
	m_angularVelocity = 0.f;
	m_vel.x = GetRandomFloatInRange(-1.f, 1.f);
	m_vel.y = GetRandomFloatInRange(-1.f, 1.f);
	this->setSpeed(speed);
	m_shouldDiscardOnDeath=true;
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

void Entity::startTurningTowards(float goalOrientationDegrees)
	{
	if (goalOrientationDegrees != m_orientationDegrees)
		{
		m_goalOrientationDegrees = goalOrientationDegrees;
		m_isTurning = true;
		}
	}

void Entity::stopTurning()
	{
	m_goalOrientationDegrees=m_orientationDegrees;
	m_isTurning=false;
	}

void Entity::turnTowardsGoal(float deltaSeconds)
	{
	if(!m_isTurning)
		return;
	float less180=m_goalOrientationDegrees-180.f;
	float more180=m_goalOrientationDegrees+180.f;
	float degreesLeft=m_goalOrientationDegrees-m_orientationDegrees;
	if (degreesLeft != 0.f)
		{
		float angStep = m_turningRate*deltaSeconds;
		while(m_orientationDegrees<less180)
			m_orientationDegrees+=360.f;
		while(m_orientationDegrees>more180)
			m_orientationDegrees-=360.f;
		float sign=getSign(degreesLeft);
		angStep=minOf(absval(degreesLeft),angStep);
		angStep*=sign;
		m_orientationDegrees+=angStep;
		}
	else
		m_isTurning=false;
	}

void Entity::accelerate(float deltaSeconds)
	{
	m_vel+=m_accel*deltaSeconds;
	}

void Entity::pushAwayFrom(const AABB2& forbiddenZone)
	{
	Vector2 edgePoint = forbiddenZone.getClosestEdgePointTo(m_pos.x, m_pos.y);
	Vector2 displacement = m_pos - edgePoint;
	if (displacement.CalcLengthSquared() == 0.f)
		{
		if(edgePoint.x==forbiddenZone.m_mins.x)
			displacement.x=-m_physicsRadius;
		else if(edgePoint.x==forbiddenZone.m_maxs.x)
			displacement.x=m_physicsRadius;
		if(edgePoint.y==forbiddenZone.m_mins.y)
			displacement.y=-m_physicsRadius;
		else if(edgePoint.y==forbiddenZone.m_maxs.y)
			displacement.y=m_physicsRadius;
		if(displacement.CalcLengthSquared()==0.f) // still?
			displacement.x=m_physicsRadius;
		}
	else if (forbiddenZone.doesContainPoint(m_pos))
		{
		displacement=-displacement;
		}
	displacement.setLength(m_physicsRadius*1.01f);
	m_pos = edgePoint + displacement;
	}


void Entity::checkAndPushAwayFrom(const AABB2& forbiddenZone)
	{
	Disc2 physicsBounds=calcPhysicsBounds();
	if (physicsBounds.doesOverlapWith(forbiddenZone))
		{
		pushAwayFrom(forbiddenZone);
		}
	}


void Entity::halt()
	{
	m_vel=Vector2::ZERO;
	}

void Entity::changeHitPointsBy(int hpdelta)
	{
	while(hpdelta<-1 && m_hitPoints+hpdelta<=0)
		break;
	setHitPoints(m_hitPoints+hpdelta);
	}

void Entity::setHitPoints(int newHitPoints)
	{
	m_hitPoints=newHitPoints;
	}

void Entity::initHitPoints(int hitPointCapacity)
	{
	m_hitPoints = m_hitPointsLastFrame = m_hitPointCapacity = hitPointCapacity;
	}

bool Entity::isDead() const
	{
	bool result = m_hitPoints <= 0;
	result = result || !calcCosmeticBounds().doesOverlapWith(g_theWorldBounds->createdWorldExpandedBy(m_cosmeticRadius*2.f));
	return result;
	}

bool Entity::didJustDie() const
	{
	bool isDeadThisFrame=isDead();
	bool wasAliveLastFrame=m_hitPointsLastFrame>0;
	bool result=isDeadThisFrame&&wasAliveLastFrame;
	while(result)
		break;
	return result;
	}

void Entity::changeDirectionX()
	{
	m_vel.x=-m_vel.x;
	m_orientationDegrees=m_goalOrientationDegrees= 180.f -m_orientationDegrees;
	}

void Entity::changeDirectionY()
	{
	m_vel.y=-m_vel.y;
	m_orientationDegrees=m_goalOrientationDegrees=-m_orientationDegrees;
	}

void Entity::drawHeading() const
	{
	Vector2 heading=calcHeadingVector();
	heading*=(2.f*m_cosmeticRadius);
	LineSegment2 line(m_pos,m_pos+heading);
	float lineThickness=2.f;
	g_theRenderer->drawLine(line,Rgba::RED,lineThickness);
	}

void Entity::drawGoalHeading() const
	{
	Vector2 heading = calcGoalVector();
	heading *= (2.f*m_cosmeticRadius);
	LineSegment2 line(m_pos, m_pos + heading);
	float lineThickness = 2.f;
	g_theRenderer->drawLine(line, Rgba::PURPLE, lineThickness);
	}

void Entity::render() const
	{
	if(isDead())
		return;
	if (g_doShowDebugGraphics)
		{
		drawGraphicsShell();
		drawPhysicsShell();
		drawVelocityVector();
		drawHeading();
		drawGoalHeading();
		}
	drawBaseSprite();
	if(m_hasHealthBar)
		drawHealthBar();
	}

void Entity::drawHealthBar() const
	{
	float barTop=m_pos.y-m_cosmeticRadius;
	float barBottom=barTop-ENTITY_HEALTH_BAR_HEIGHT;
	float halfwidth = 0.5f*ENTITY_HEALTH_BAR_WIDTH;
	float barLeft = m_pos.x - halfwidth;
	float barRight = m_pos.x + halfwidth;
	float healthScale = (m_hitPointCapacity==0)? 0.f: (float)m_hitPoints/(float)m_hitPointCapacity;
	float barMiddleX = barLeft*(1.f-healthScale) + barRight*healthScale;
	//draw the bar
	g_theRenderer->drawFilledBox(AABB2(Vector2(barLeft, barTop), Vector2(barMiddleX, barBottom)), Rgba::GREEN);
	g_theRenderer->drawFilledBox(AABB2(Vector2(barMiddleX, barTop),Vector2(barRight,barBottom)), Rgba::RED);
	//draw the border
	float barBorderThickness=2.f;
	g_theRenderer->drawBox(barLeft, barTop,barRight, barBottom, Rgba::WHITE,barBorderThickness);
	}

void Entity::drawBaseSprite() const
	{
	if (g_allSprites)
		{
		Rgba tint=Rgba::WHITE;
		g_allSprites->drawSpriteByPositionRadius(m_pos,m_cosmeticRadius,m_baseSpriteIndex,tint,m_orientationDegrees);
		}
	}

float Entity::getVelocityAngleDegrees()
	{
	float result=atan2Degrees(m_vel.y, m_vel.x);
	return result;
	}

void Entity::fixOrientation()
	{
	m_orientationDegrees=fixDegrees180(m_orientationDegrees);
	m_goalOrientationDegrees=fixDegrees180(m_goalOrientationDegrees);
	}

void Entity::doDamage(int damage)
	{
	if(m_hitPoints>damage)
		changeHitPointsBy(-damage);
	else
		setHitPoints(0);
	}

float Entity::howLongSinceDeathInSeconds()
	{
	return m_secondsSinceDeath;
	}

float Entity::shouldBeDiscardedAtTimeOfDeath()
	{
	return m_shouldDiscardOnDeath;
	}

void Entity::updateDeathCounter(float deltaSeconds)
	{
	if(isDead())
		m_secondsSinceDeath += deltaSeconds;
	else
		m_secondsSinceDeath = 0.f;
	}

void Entity::updateEnd(float /*deltaSeconds*/)
	{
	if (didJustDie() && m_shouldExplodeOnDeath)
		{
		if (m_shouldExplodeOnDeath)
			explode();
		}
	}

void Entity::setApartTheDead()
	{
	if(isDead())
		m_pos=DEAD_ENTITY_POSITION;
	}

void Entity::respawn(const Vector2& newpos)	
	{
	m_pos=newpos;
	m_secondsSinceDeath = 0.f;
	}

bool Entity::shouldBeRespawned()
	{
	if (isDead())
		{
		return m_secondsSinceDeath > ENTITY_DEATH_TIME_OUT_IN_SECONDS;
		}
	return false;
	}

void Entity::explode()
	{
	if(m_explosionSound != MISSING_SOUND_ID)
		g_theAudio->playSound(m_explosionSound);
	float durationScale=m_cosmeticRadius*(1.f/LARGE_ENTITY_RADIUS);
	float explosionDurationSeconds=LARGE_ENTITY_EXPLOSION_DURATION_IN_SECONDS*durationScale;
	g_theMap->spawnExplosion(m_pos,m_cosmeticRadius*3.f, explosionDurationSeconds);
	}

bool Entity::compareEntitiesForSort(Entity* one, Entity* another)
	{
	if(one==nullptr)
		return false;
	if(another==nullptr)
		return false;
	bool result=one->getElevation()<another->getElevation();
	return result;
	}

void Entity::startHitPointFrame()
	{
	m_hitPointsLastFrame=m_hitPoints;
	}

void Entity::keepInWorld()
	{
	if (m_shouldKeepInWorld)
		{
		AABB2 shrunkWorldBounds=g_theWorldBounds->createdWorldExpandedBy(-m_cosmeticRadius);
		m_pos=shrunkWorldBounds.getClosestInteriorPointTo(m_pos);
		}
	}

void Entity::drawGraphicsShell() const
	{
	const Rgba GRAPHICS_SHELL_COLOR = Rgba::BLUE;
	g_theRenderer->drawCircle(m_pos.x, m_pos.y, m_cosmeticRadius*m_drawingScale, AMT_BOUNDS_CIRCLE_EDGES, GRAPHICS_SHELL_COLOR);
	}

void Entity::drawPhysicsShell() const
	{
	const Rgba PHYSICS_SHELL_COLOR = Rgba::CYAN;
	g_theRenderer->drawCircle(m_pos.x, m_pos.y, m_physicsRadius*m_drawingScale, AMT_BOUNDS_CIRCLE_EDGES, PHYSICS_SHELL_COLOR);
	}

void Entity::drawVelocityVector() const
	{
	const Rgba VELOCITY_VECTOR_COLOR = Rgba(0xFFFFFF7F);
	Vector2 endpoint=m_pos+m_vel*m_drawingScale;
	float lineThickness=5.f;
	g_theRenderer->drawLine(m_pos.x,m_pos.y,endpoint.x,endpoint.y,VELOCITY_VECTOR_COLOR,lineThickness);
	}

Vector2 Entity::calcMomentum()
	{
	return m_mass*m_vel;
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
	m_drawingScale=scale;
	}

Disc2 Entity::calcCosmeticBounds() const
	{
	Disc2 result(m_pos,m_cosmeticRadius);
	return result;
	}

Disc2 Entity::calcPhysicsBounds() const
	{
	Disc2 result(m_pos, m_physicsRadius);
	return result;
	}

AABB2 Entity::calcCosmeticBoundingBox() const
	{
	AABB2 result;
	Vector2 displacement(m_cosmeticRadius,m_cosmeticRadius);
	result.m_mins = m_pos-displacement;
	result.m_maxs = m_pos+displacement;
	return result;
	}

void separateTwoEntities(Entity& a, Entity& b)
	{
	Disc2 adisc(a.m_pos,a.m_physicsRadius);
	Disc2 bdisc(b.m_pos,b.m_physicsRadius);
	if(!DoDiscsOverlap(adisc,bdisc))
		return;
	Vector2 displacement=b.m_pos-a.m_pos;
	displacement.Normalize();
	float radiusSum = a.m_physicsRadius + b.m_physicsRadius;
	float dist = CalcDistance(adisc.m_center, bdisc.m_center);
	float displacementLen = radiusSum - dist;
	displacement *= displacementLen;
	float aDispScale = !a.isMoveable()? 0.f: 0.5f;
	float bDispScale = !b.isMoveable()? 0.f: 1.f-aDispScale;
	float dispScaleSum=aDispScale+bDispScale;
	if (dispScaleSum>0.f)
		{
		float correctionScale=1.f/dispScaleSum;
		aDispScale*=correctionScale;
		bDispScale*=correctionScale;
		}
	a.m_pos -= displacement*aDispScale;
	b.m_pos += displacement*bDispScale;
	}

void Entity::update(float /*deltaSeconds*/)
	{

	}

void Entity::updateStart(float deltaSeconds)
	{
	startHitPointFrame();
	if (isDead())
		{
		updateDeathCounter(deltaSeconds);
		setApartTheDead();
		}
	}

void Entity::applyTraction()
	{
//	float curSpeedSquared=m_vel.CalcLengthSquared();
	float speedScale=1.f;
// 	if(curSpeedSquared>1000.f)
// 		speedScale=1000.f/curSpeedSquared;
	float adjustedTraction=m_treadTraction*speedScale;
	Vector2 heading = calcHeadingVector();
	float speedAlongHeading = dotProduct(m_vel, heading);
	Vector2 headingVel = heading*speedAlongHeading;
	m_vel = m_vel*(1.f - adjustedTraction) + headingVel*adjustedTraction;
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
	m_prevPos=m_pos;
	m_pos += m_vel*deltaSeconds;
	keepInWorld();
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

Vector2 Entity::calcHeadingVector() const
	{
	float radians=ConvertDegreesToRadians(m_orientationDegrees);
	Vector2 result(cos(radians),sin(radians));
	result.Normalize();
	return result;
	}

Vector2 Entity::calcGoalVector() const
{
	float radians = ConvertDegreesToRadians(m_goalOrientationDegrees);
	Vector2 result(cos(radians), sin(radians));
	result.Normalize();
	return result;
	}

bool Entity::doesCollideWith(const Entity& other)
	{
	Disc2 thisdisc(m_pos,m_physicsRadius);
	Disc2 otherdisc(other.m_pos,other.m_physicsRadius);
	return thisdisc.doesOverlapWith(otherdisc);
	}

void processVerfiedEnemyCollision(Entity& a,Entity& b)
	{
	EnemyShip* eship=nullptr;
	Ship* pship=nullptr;
	PlayerBullet* pbullet=nullptr;
	EntityType atype=a.gettype();
	switch (atype)
		{
		case ENTITY_TYPE_PLAYER_SHIP:   pship=(Ship*)&a; break;
		case ENTITY_TYPE_PLAYER_BULLET: pbullet=(PlayerBullet*)&a; break;
		case ENTITY_TYPE_ENEMY_SHIP:    eship=(EnemyShip*)&a; break;
		}
	EntityType btype=b.gettype();
	switch (btype)
		{
		case ENTITY_TYPE_PLAYER_SHIP:   pship=(Ship*)&b; break;
		case ENTITY_TYPE_PLAYER_BULLET: pbullet=(PlayerBullet*)&b; break;
		case ENTITY_TYPE_ENEMY_SHIP:    eship=(EnemyShip*)&b; break;
		}
	if (eship&&pbullet)
		{
		pbullet->doDamage(1);
		eship->doDamage(STARTING_ENEMY_SHIP_HIT_POINTS);
		}
	else if (eship&&pship)
		{
		eship->doDamage(STARTING_ENEMY_SHIP_HIT_POINTS);
		pship->doDamage(1);
		}
	}

bool checkCollsionBetweenTwoEntities(Entity& a,Entity& b)
	{
	bool result=&a!=&b;
	result=result&&a.doesCollideWith(b);
	return result;
	}

void checkAndCollideTwoEntities(Entity& a,Entity& b)
	{
	if (checkCollsionBetweenTwoEntities(a,b))
		{
		processVerfiedEnemyCollision(a,b);
		}
	}
