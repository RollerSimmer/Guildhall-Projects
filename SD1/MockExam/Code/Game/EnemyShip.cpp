#include "Game/EnemyShip.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtil.hpp"

//TODO: add enemy tank AI

EnemyShip::EnemyShip(int indexIntoGameArray/*=0*/,const Vector2& pos/*=Vector2::ZERO*/,float radius/*=ENEMY_SHIP_STARTING_RADIUS*/,float speed/*=ENEMY_SHIP_STARTING_SPEED*/)
	: Ship(indexIntoGameArray,pos,radius,speed)
	, m_patrolChangeCountdownSeconds(PATROL_CHANGE_INTERVAL_IN_SECONDS)
	, m_patrolTargetOrientationDegrees(0.f)
	, m_targetShip(nullptr)
	{
	m_type=ENTITY_TYPE_ENEMY_SHIP;
	m_isActive=false;
	m_baseSpriteIndex=ENEMY_SHIP_SPRITE_INDEX;
	m_gunOrientationDegrees=0.f;
	m_gunGoalOrientationDegrees=0.f;
	m_isAiming=false;
	m_fractionSpeed=1.f;
	m_bulletCooldownSeconds=0.f;
	m_turningRate = ENEMY_SHIP_TURNING_RATE;
	initHitPoints(STARTING_ENEMY_SHIP_HIT_POINTS);
	m_patrolTargetOrientationDegrees=GetRandomFloatInRange(0.f,360.f);
	m_orientationDegrees=m_patrolTargetOrientationDegrees;
	m_treadTraction = 0.075f;
	m_isTurning=false;
	m_hasHealthBar=false;
	m_shouldDiscardOnDeath=false;
	m_secondsSinceDeath=0.f;
	m_firingSound = g_theAudio->createOrGetSound("Data/Audio/EnemyBulletShoot.ogg");
	m_explosionSound = g_theAudio->createOrGetSound("Data/Audio/Explosion.ogg");
	m_shouldExplodeOnDeath = true;
	m_firingData.m_accuracy = getBellRandInt(FiringData::MIN_SHOOTING_ACCURACY,FiringData::MAX_SHOOTING_ACCURACY,3);
	m_firingData.m_range = getBellRandInt(FiringData::MIN_SHOOTING_RANGE, FiringData::MAX_SHOOTING_RANGE,3);
	}


EnemyShip::~EnemyShip()
	{
	}

void EnemyShip::update(float deltaSeconds)
	{
	updateStart(deltaSeconds);
	if (!isDead())
		{
		m_bulletCooldownSeconds = maxOf(m_bulletCooldownSeconds - deltaSeconds, 0.f);
		updateAI(deltaSeconds);
		}
	updateEnd(deltaSeconds);
	}

void EnemyShip::accelerate(float deltaSeconds)
	{
	Entity::accelerate(deltaSeconds);
	}

void EnemyShip::render() const
	{
	if(isDead())
		return;
	Entity::render();
	if(g_doShowDebugGraphics)
		drawLineToTargetTank();
	}

void EnemyShip::capSpeed()
	{
	if (m_vel != Vector2::ZERO)
		{
		float topspeed = ENEMY_SHIP_TOP_SPEED*m_fractionSpeed;
		if (m_vel.CalcLengthSquared() > topspeed*topspeed)
			{
			m_vel.setLength(topspeed);
			}
		}
	}

void EnemyShip::updateAI(float deltaSeconds)
	{
	Ship* targetShip=g_theMap->findThePlayerShip();
	m_tankState = ENEMY_SHIP_STATE_PURSUING;
	m_targetShip=targetShip;
	if (targetShip != nullptr)
		{
		updatePursuit(deltaSeconds);
		}
	}

float EnemyShip::calcPatrolTimeScale()
	{
	float x=(PATROL_CHANGE_INTERVAL_IN_SECONDS-m_patrolChangeCountdownSeconds)/ PATROL_CHANGE_INTERVAL_IN_SECONDS;
	int exp=2;
	int amtRecursions=2;
	float t=smootherStepN(x,exp,exp, amtRecursions);
	float result=clampFloat(t,0.f,1.f);
	return result;
	}

void EnemyShip::fixHeading()
	{
// 	if(m_vel.CalcLengthSquared()>0.f)
// 		m_orientationDegrees=m_goalOrientationDegrees=m_patrolStartOrientationDegrees=m_patrolTargetOrientationDegrees=getVelocityAngleDegrees();
	}

void EnemyShip::respawn(const Vector2& newpos)
	{
	m_pos=newpos;
	m_vel.SetXY(0.f,0.f);
	m_patrolChangeCountdownSeconds = PATROL_CHANGE_INTERVAL_IN_SECONDS;
	m_patrolTargetOrientationDegrees = 0.f;
	m_targetShip = nullptr;
	m_isActive = false;
	m_gunOrientationDegrees = 0.f;
	m_gunGoalOrientationDegrees = 0.f;
	m_isAiming = false;
	m_fractionSpeed = 1.f;
	m_bulletCooldownSeconds = 0.f;
	initHitPoints(STARTING_ENEMY_SHIP_HIT_POINTS);
	m_patrolTargetOrientationDegrees = GetRandomFloatInRange(0.f, 360.f);
	m_orientationDegrees = m_patrolTargetOrientationDegrees;
	m_treadTraction = 0.075f;
	m_isTurning = false;
	m_hasHealthBar = true;
	m_shouldDiscardOnDeath = false;	
	m_secondsSinceDeath = 0.f;
	}

Vector2 EnemyShip::calcPatrolTopVector()
	{
	float x=cosDegrees(m_patrolTargetOrientationDegrees);
	float y=-sinDegrees(m_patrolTargetOrientationDegrees);
	Vector2 result(x,y);
	result.setLength(ENEMY_SHIP_PATROL_SPEED);
	return result;
	}

void EnemyShip::drawLineToTargetTank() const
	{
	Rgba targetLineColor=Rgba::LIME;
	float linethickness=2.f;
	if (m_tankState==ENEMY_SHIP_STATE_PURSUING && m_targetShip)
		{
		g_theRenderer->drawLine(LineSegment2(m_pos,m_targetShip->getpos()),targetLineColor,linethickness);
		}
	}

void EnemyShip::updatePursuit(float deltaSeconds)
	{
	// TODO: fix turning rate so that it is faster
	if(m_targetShip==nullptr)
		return;
	Vector2 goalHeading= m_targetShip->getpos() - m_pos;
	goalHeading.Normalize();
	float goalOrientationDegrees=goalHeading.calcHeadingDegrees();
// 	if (goalOrientationDegrees!=m_orientationDegrees)
// 		m_vel = m_vel*0.5f;
	m_accel=Vector2::ZERO;
	m_vel=goalHeading;
	m_vel.setLength(ENEMY_SHIP_PURSUIT_SPEED);
	accelerate(deltaSeconds);
	//m_goalOrientationDegrees=m_vel.calcHeadingDegrees();
	m_orientationDegrees = goalOrientationDegrees;
	move(deltaSeconds);
	//capSpeed();
	//shootAtPlayer();
	}

void EnemyShip::shootAtPlayer()
	{
	// enemies don't shoot at player in this game
	}

bool EnemyShip::shouldShootAtPlayer()
	{
	static const float degreeTolerance = 10.f;
	static const float minProjectionLength = Vector2(cosDegrees(degreeTolerance), sinDegrees(degreeTolerance)).dotProduct(Vector2(1.f, 0.f));
	if (m_tankState == ENEMY_SHIP_STATE_PURSUING && m_targetShip != nullptr)
		{
		Vector2 targetDisplacement = m_targetShip->getpos() - m_pos;
		Vector2 targetDir = targetDisplacement.getNormalized();
		m_currentHeading = calcHeadingVector();
		float dot = targetDir.dotProduct(m_currentHeading);
		bool result=dot>minProjectionLength;
		return result;
		}
	else
		return false;
	}
