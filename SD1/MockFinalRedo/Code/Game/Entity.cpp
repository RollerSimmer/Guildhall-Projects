#include "Game/Entity.hpp"
#include <cmath>
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Core/Time.hpp"

//TODO: make a method to keep entities in bounds if they are not bullets

Entity::Entity(ENTITY::TYPE etype,const Vector2& pos,float radius,const Vector2& vel,float orideg)
	: m_type(etype)
	,m_pos(pos)
	,m_orientationDegrees(orideg)
	,m_vel(vel)
	,m_graphicsRadius(radius)
	,m_physicsRadius(radius*GRAPHICS_TO_PHYSICS_RADIUS_RATIO)
// 	,m_angularVelocity(0.f)
	,m_outOfBoundsCounterSeconds(0.f)
	,m_drawScale(1.f)
	,m_spriteIndex(0)
	,m_shotCooldownCountdownSeconds(0.f)
	,m_shotCooldownDurationSeconds(0.1f)
	,m_target(nullptr)
	,m_isEntityMarkedForDeath(false)
	,m_animationFrameTimerSeconds(0.f)
	,m_animationFrameDurationSeconds(0.1f)
	,m_isAnimated(false)
	,m_shouldKeepInWorld(true)
	{

	}

Entity* Entity::createShip(const Vector2& pos)
	{
	Entity* result=new Entity(ENTITY::SHIP,pos,25.f,Vector2::ZERO,90.f);
	result->m_spriteIndex=0;
	result->m_shotCooldownCountdownSeconds=0.f;
	result->m_shotCooldownDurationSeconds=0.1f;
	return result;
	}


Entity* Entity::createBullet(const Vector2& bulletpos,const Vector2& bulletvel, float bulletorientationDegrees)
	{
	Entity* result=new Entity(ENTITY::BULLET,bulletpos,15.f,bulletvel,bulletorientationDegrees);
	result->m_spriteIndex=1;
	result->m_shouldKeepInWorld=false;
	g_theAudio->playSound(g_theAudio->createOrGetSound(std::string("Data/Audio/PlayerShoot.wav")));
	return result;
	}


Entity* Entity::createEnemy(const Vector2& pos)
	{
	Entity* result=new Entity(ENTITY::ENEMY,pos,ENEMY_COSMETIC_RADIUS,Vector2::ZERO,0.f);
	result->m_spriteIndex=GetRandomIntInRange(8,10);
	result->m_shouldKeepInWorld=false;
	result->m_target = g_theGame ? g_theGame->findThePlayerShip() : nullptr;
	return result;
	}

Entity* Entity::createExplosion(const Vector2& pos,float radius)
	{
	Entity* result=new Entity(ENTITY::EXPLOSION,pos,radius,Vector2::ZERO,90.f);
	result->m_spriteIndex=16;
	result->m_isAnimated=true;
	result->m_animationFrameDurationSeconds=sqrt(radius)*0.02f;
	return result;
	}

Entity* Entity::createWinAnimation()
	{
	Entity* result=new Entity(ENTITY::WIN_ANIMATION,Vector2::ZERO,0.01f,Vector2::ZERO,0.f);
	result->m_spriteIndex=0;
	result->m_animationFrameDurationSeconds=0.08f;
	result->m_isAnimated=true;
	return result;
	}

Entity* Entity::createDeathAnimation()
	{
	Entity* result=new Entity(ENTITY::DEATH_ANIMATION,Vector2::ZERO,0.01f,Vector2::ZERO,0.f);
	result->m_spriteIndex=0;
	result->m_animationFrameDurationSeconds=0.08f;
	result->m_isAnimated=true;
	return result;
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
	drawUnderglow();
	drawSprite();
	drawScreenAnimationFrame();
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

void Entity::markForDeath()
	{
	m_isEntityMarkedForDeath=true;
	}

bool Entity::isDead()
	{
	bool result=m_isEntityMarkedForDeath;
	if (m_type==ENTITY::EXPLOSION)
		result=m_spriteIndex>=21;
	else if (m_type==ENTITY::WIN_ANIMATION)
		result=m_spriteIndex>=WIN_ANIMATION_LAST_FRAME;	
	else if (m_type==ENTITY::DEATH_ANIMATION)
		result=m_spriteIndex>=DEATH_ANIMATION_LAST_FRAME;
	return result;
	}

void Entity::keepInWorld()
	{
	AABB2 shrunkworld=g_theWorldBounds->createdWorldExpandedBy(-m_graphicsRadius-0.1f);
	AABB2 expworld=g_theWorldBounds->createdWorldExpandedBy(m_graphicsRadius+0.1f);
	AABB2 bounds=m_shouldKeepInWorld? shrunkworld : expworld;
	Disc2 disc=calcCosmeticDisc();
	if (!bounds.doesOverlapWith(disc))
		{
		if (m_shouldKeepInWorld)
			{
			m_pos=shrunkworld.getClosestEdgePointTo(m_pos);
			}
		else
			{
			markForDeath();
			}
		}
	}

void Entity::drawUnderglow() const
	{
	float pulseAlpha=0.5f+0.25f*(1.f+cos((float)GetCurrentTimeSeconds()));
	Rgba color = Rgba::WHITE;
	color.setAlphaAsFloat(0.f);
	switch (m_type)
		{
		case ENTITY::SHIP:/* case ENTITY::BULLET:*/
			color = Rgba::LIME; color.setAlphaAsFloat(pulseAlpha); break;
		case ENTITY::ENEMY: color = Rgba::RED; color.setAlphaAsFloat(pulseAlpha); break;

		}
	g_theRenderer->drawLiquidDisc2(m_pos.x,m_pos.y,m_graphicsRadius*0.5f,16,color);
	}

void Entity::drawScreenAnimationFrame() const
	{
	if (m_type==ENTITY::WIN_ANIMATION)
		{
		Rgba color=Rgba::WHITE;
		float alpha=(float)m_spriteIndex/(float)WIN_ANIMATION_LAST_FRAME;
		color.setAlphaAsFloat(alpha);
		g_theRenderer->drawFilledBox(*g_theWorldBounds,color);
		}
	}

void Entity::setTarget(Entity* newtarget)
	{
	m_target=newtarget;
	}

Disc2 Entity::calcCosmeticDisc()
	{
	return Disc2(m_pos,m_graphicsRadius);
	}

void Entity::updateEnemyAI()
	{
	if(m_type!=ENTITY::ENEMY)
		return;
	Entity* target=m_target;
	if(target==nullptr)
		return;
	Vector2 disp=target->m_pos-m_pos;
	m_vel=disp;
	if (disp.CalcLengthSquared()>0.f)
		{
		m_vel=disp;
		m_vel.setLength(50.f);
		m_orientationDegrees=atan2Degrees(m_vel.y,m_vel.x);
		}
	}

void Entity::updateAnimation(float deltaSeconds)
	{
	if (m_isAnimated)
		{
		m_animationFrameTimerSeconds+=deltaSeconds;
		if (m_animationFrameTimerSeconds>m_animationFrameDurationSeconds)
			{
			m_animationFrameTimerSeconds=0.f;
			m_spriteIndex++;
			if(m_spriteIndex>=64)
				m_spriteIndex=0;
			}
		}
	}

void Entity::drawSprite() const
	{
	AABB2 bounds=calcGraphicsBounds();
	g_sprites->drawSprite(bounds,m_spriteIndex,Rgba::WHITE,m_orientationDegrees);
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
	updateEnemyAI();
	updateAnimation(deltaSeconds);
	move(deltaSeconds);
	rotate(deltaSeconds);
	updateInput(deltaSeconds);
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
	keepInWorld();
	}

void Entity::rotate(float /*deltaSeconds*/)
	{
// 	m_orientationDegrees += m_angularVelocity*deltaSeconds;
// 	m_orientationDegrees = (float)fmod(m_orientationDegrees, 360.0);
	}

void Entity::updateInput(float deltaSeconds)
	{
	updateTurningInput(/*deltaSeconds*/);
	updateShootingInput(deltaSeconds);
	}

void Entity::updateTurningInput()
	{
	if (m_type!=ENTITY::SHIP)
		return;
	XboxController& xbc=g_theInput->getXboxController(XCONTROLLER_0);
	float lstickX = xbc.getStickValue(XSTICK_LEFT,X_AXIS_2D);
	float lstickY = xbc.getStickValue(XSTICK_LEFT,Y_AXIS_2D);
	/*	m_accel = Vector2::ZERO;*/
	float xdir = 0.f;
	float ydir = 0.f;
	xdir=lstickX;
	ydir=lstickY;

	if (xdir != 0.f || ydir != 0.f)
		{
		float m=minOf(absval(xdir),absval(ydir));
		float M = maxOf(absval(xdir),absval(ydir));
		float speed=M+0.3f*m;
		speed=200.f;
		m_vel=Vector2(xdir,ydir);
		m_vel.setLength(speed);
		m_orientationDegrees=atan2Degrees(ydir,xdir);
		}
	else
		{
		m_vel=Vector2::ZERO;
		}
	}

void Entity::updateShootingInput(float deltaSeconds)
{
	if (m_type!=ENTITY::SHIP)
		return;
	XboxController& xbc=g_theInput->getXboxController(XCONTROLLER_0);
	float rstickX = xbc.getStickValue(XSTICK_RIGHT,X_AXIS_2D);
	float rstickY = xbc.getStickValue(XSTICK_RIGHT,Y_AXIS_2D);
	/*	m_accel = Vector2::ZERO;*/
	float xdir = 0.f;
	float ydir = 0.f;
	xdir=rstickX;
	ydir=rstickY;

	m_shotCooldownCountdownSeconds-=deltaSeconds;

	if ( (xdir != 0.f || ydir != 0.f) ) 
		{
		float m=minOf(absval(xdir),absval(ydir));
		float M = maxOf(absval(xdir),absval(ydir));
		float speed=M+0.3f*m;
		speed=100.f;
		//m_vel=Vector2::ZERO;
		Vector2 bulletvel=Vector2(xdir,ydir);
		bulletvel.setLength(1000.f);
		Vector2 bulletpos=m_pos/*+calcHeadingVector()*m_graphicsRadius*/;
		m_orientationDegrees=atan2Degrees(ydir,xdir);
		if (m_shotCooldownCountdownSeconds<=0.f)
			{
			g_theGame->spawnBullet(bulletpos,bulletvel,m_orientationDegrees);
			m_shotCooldownCountdownSeconds=m_shotCooldownDurationSeconds;
			}
		}
	else
		{
		}
	}

AABB2 Entity::calcGraphicsBounds() const
	{
	Vector2 disp(m_graphicsRadius,m_graphicsRadius);
	AABB2 result(m_pos-disp,m_pos+disp);
	return result;
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

void collideTwoEntities(Entity& a,Entity& b)
	{
	if(&a==&b)
		return;
	if(!a.doesCollideWith(b))
		return;
	Entity* bullet=nullptr;
	Entity* enemy=nullptr;
	Entity* playership=nullptr;
	if      (a.m_type==ENTITY::BULLET)       bullet=&a;
	else if (b.m_type==ENTITY::BULLET)       bullet=&b;
	if      (a.m_type==ENTITY::ENEMY)       enemy=&a;
	else if (b.m_type==ENTITY::ENEMY)       enemy=&b;
	if      (a.m_type==ENTITY::SHIP)       playership=&a;
	else if (b.m_type==ENTITY::SHIP)       playership=&b;
	if (enemy&&playership)
		{
		g_theGame->increaseTraumaBy(0.2f);
		enemy->markForDeath();
		g_theGame->spawnEnemies(2);
		g_theGame->spawnExplosionAt(enemy->m_pos,enemy->m_graphicsRadius);
		g_theAudio->playSound(g_theAudio->createOrGetSound(std::string("Data/Audio/EnemyKilled.wav")));

		if (g_theGame->isTraumaCritical())
			{
			g_theAudio->playSound(g_theAudio->createOrGetSound(std::string("Data/Audio/PlayerKilled.wav")));
			playership->markForDeath();
			g_theGame->spawnExplosionAt(playership->m_pos,playership->m_graphicsRadius*5.f);
			}
		//g_theAudio->playSound(g_theAudio->createOrGetSound(std::string("Data/Audio/EnemyKilled.wav")));
		}
	else if (enemy&&bullet)
		{
		enemy->markForDeath();
		bullet->markForDeath();
		g_theGame->spawnEnemies(2);
		g_theGame->spawnExplosionAt(enemy->m_pos,enemy->m_graphicsRadius);
		g_theAudio->playSound(g_theAudio->createOrGetSound(std::string("Data/Audio/EnemyKilled.wav")));
		}
	}