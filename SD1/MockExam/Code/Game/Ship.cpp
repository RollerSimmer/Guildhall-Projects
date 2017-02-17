#include "Game/Ship.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtil.hpp"
#include <cassert>

const float FiringData::MAX_SHOOTING_ANGLE_DEVIATION_IN_DEGREES=45.f;

Ship::Ship(int indexIntoGameArray/*=0*/,const Vector2& pos/*=Vector2::ZERO*/,float radius/*=TANK_STARTING_RADIUS*/,float speed/*=TANK_STARTING_SPEED*/)
	: Entity(indexIntoGameArray,ENTITY_TYPE_PLAYER_SHIP,pos,radius,speed)
	{
	m_baseSpriteIndex=PLAYER_SHIP_SPRITE_INDEX;
	initHitPoints(STARTING_SHIP_HIT_POINTS);
	m_turningRate=SHIP_TURNING_RATE;
	m_treadTraction=0.075f;
	m_orientationDegrees=m_goalOrientationDegrees=GetRandomFloatInRange(0.f,360.f);
	m_shouldDiscardOnDeath=false;
	m_firingSound = g_theAudio->createOrGetSound("Data/Audio/PlayerShoot.wav");
	assert(m_firingSound!=MISSING_SOUND_ID);
	m_explosionSound=g_theAudio->createOrGetSound("Data/Audio/PlayerKilled.wav");
	assert(m_explosionSound!=MISSING_SOUND_ID);
	m_shouldExplodeOnDeath = true;
	m_firingData.m_accuracy = FiringData::DEFAULT_SHOOTING_ACCURACY;
	m_firingData.m_range = FiringData::DEFAULT_SHOOTING_RANGE;
	m_bulletCooldownSeconds=0.f;
	m_shouldDiscardOnDeath=true;
	}

void Ship::render() const
	{
	if (isDead())
		return;
	Entity::render();
	}

void Ship::update(float deltaSeconds)
	{
	updateStart(deltaSeconds);
	if (!isDead())
		{
		m_bulletCooldownSeconds = maxOf(m_bulletCooldownSeconds - deltaSeconds,0.f);
		updateInput(deltaSeconds);
		accelerate(deltaSeconds);
		move(deltaSeconds);
		if (m_isTurning)
			turnTowardsGoal(deltaSeconds);
		capSpeed();
		//applyTraction();
		//healPlayerTank(deltaSeconds);
		}
	updateEnd(deltaSeconds);
	}

void Ship::updateInput(float deltaSeconds)
	{
	if (!m_isActive)
		return;
	updateTurningInput();
	updateAimingInput(deltaSeconds);
	if (m_isAiming)
		aimTowardsGoal(deltaSeconds);
	const float  RIGHT_TRIGGER_SHOOT_THRESHOLD = 0.25f;
// 	if (g_theInput->isKeyDown(KEYCODE_SPACE)
// 		|| g_theInput->getXboxController(XCONTROLLER_0).getTriggerValue(XBOX_TRIGGER_RIGHT) >= RIGHT_TRIGGER_SHOOT_THRESHOLD)
// 		fireBullet();
	//TODO: change trigger shooting code to right stick shooting code
	}

void Ship::setactive(bool /*activeState*//*=true*/)
	{

	}

void Ship::accelerate(float /*deltaSeconds*/)
	{
// 	if (m_accel.CalcLengthSquared() > 0.f)
// 		Entity::accelerate(deltaSeconds);
// 	else
// 		halt();
	}

void Ship::capSpeed()
	{
	if (m_vel != Vector2::ZERO)
		{
		float topspeed=SHIP_TOP_SPEED*m_fractionSpeed;
		if (m_vel.CalcLengthSquared() > topspeed*topspeed)
			{
			m_vel.setLength(topspeed);
			}
		}
	}

void Ship::drawGunSprite() const
	{
	// no gun needed
	}

void Ship::updateTurningInput()
	{
	m_accel=Vector2::ZERO;  // input causes instance speed changes
	if (!m_isActive)
		return;
	XboxController& xbc=g_theInput->getXboxController(XCONTROLLER_0);
	float lstickX = xbc.getStickValue(XSTICK_LEFT,X_AXIS_2D);
	float lstickY = xbc.getStickValue(XSTICK_LEFT,Y_AXIS_2D);
	m_accel = Vector2::ZERO;
	float xdir = 0.f;
	float ydir = 0.f;
	xdir=lstickX;
	ydir=lstickY;
	if (xdir != 0.f || ydir != 0.f)
		{
		float m=minOf(absval(xdir),absval(ydir));
		float M = maxOf(absval(xdir),absval(ydir));
		m_fractionSpeed=M+0.3f*m;
		float goalOrientationDegrees = atan2Degrees(ydir,xdir);
		m_goalOrientationDegrees=m_orientationDegrees=goalOrientationDegrees;
		m_vel=Vector2(1.f,0.f);
		m_vel.SetHeadingDegrees(m_goalOrientationDegrees);
		m_vel.setLength(FULL_SHIP_ACCELERATION*m_fractionSpeed);
		}
	else
		{
		stopTurning();
		m_vel=m_accel=Vector2::ZERO;
		}
	}

void Ship::updateAimingInput(float /*deltaSeconds*/)
	{
	if (!m_isActive)
		return;
	XboxController& xbc=g_theInput->getXboxController(XCONTROLLER_0);
	float rstickX = xbc.getStickValue(XSTICK_RIGHT,X_AXIS_2D);
	float rstickY = xbc.getStickValue(XSTICK_RIGHT,Y_AXIS_2D);
	m_accel = Vector2::ZERO;
	float xdir = 0.f;
	float ydir = 0.f;
	xdir=rstickX;
	ydir=rstickY;
	if (xdir != 0.f || ydir != 0.f)
		{
		float goalOrientationDegrees = atan2Degrees(ydir,xdir);
		m_goalOrientationDegrees=m_orientationDegrees=goalOrientationDegrees;
		fireBullet();
		}
	else
		{
		//stopTurning();
		}
	}

void Ship::aimTowardsGoal(float /*deltaSeconds*/)
	{

	}

Vector2 Ship::calcGunHeadingVector()
	{
	//TODO
	return Vector2::ZERO;
	}

void Ship::respawn(const Vector2& /*newpos*/)
	{

	}

void Ship::stopAiming()
	{

	}

void Ship::startAimingTowards(float /*gunGoalOrientationDegrees*/)
	{

	}

void Ship::fireBullet()
	{
	if (m_bulletCooldownSeconds == 0)
		{
		Vector2 heading = calcHeadingVector();
		Vector2 gunTipPos = m_pos + heading*m_cosmeticRadius;
		g_theMap->spawnPlayerBullet(heading,gunTipPos,m_orientationDegrees);
		//TODO: get bullet firing sound to work
		g_theAudio->playSound(m_firingSound,20.f);
		m_bulletCooldownSeconds = MIN_SECONDS_BETWEEN_PLAYER_SHIP_BULLET_FIRES;
	
		}
	}

float Ship::calcAimDeviationBasedOnAccuracy()
	{
	float scale=1.f-(float)m_firingData.m_accuracy/(float)FiringData::MAX_SHOOTING_ACCURACY;
	float result=scale*FiringData::MAX_SHOOTING_ANGLE_DEVIATION_IN_DEGREES;
	return result;
	}
