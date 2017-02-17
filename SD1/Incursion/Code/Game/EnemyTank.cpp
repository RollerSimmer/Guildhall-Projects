#include "EnemyTank.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtil.hpp"

//TODO: add enemy tank AI

EnemyTank::EnemyTank(int indexIntoGameArray/*=0*/,const Vector2& pos/*=Vector2::ZERO*/,float radius/*=ENEMY_TANK_STARTING_RADIUS*/,float speed/*=ENEMY_TANK_STARTING_SPEED*/)
	: PlayerTank(indexIntoGameArray,pos,radius,speed)
	, m_patrolChangeCountdownSeconds(PATROL_CHANGE_INTERVAL_IN_SECONDS)
	, m_patrolTargetOrientationDegrees(0.f)
	, m_targetTank(nullptr)
	{
	m_type=ENTITY_TYPE_ENEMY_TANK;
	m_isActive=false;
	m_gunTexture=nullptr;
	m_gunOrientationDegrees=0.f;
	m_gunGoalOrientationDegrees=0.f;
	m_isAiming=false;
	m_fractionSpeed=1.f;
	m_bulletCooldownSeconds=0.f;
	m_turningRate = ENEMY_TANK_TURNING_RATE;
	initHitPoints(STARTING_ENEMY_TANK_HIT_POINTS);
	m_gunTexture=nullptr;
	m_baseTexture = g_theRenderer->CreateOrGetTexture("Data/Images/EnemyTank4.png");
	m_patrolTargetOrientationDegrees=GetRandomFloatInRange(0.f,360.f);
	m_orientationDegrees=m_patrolTargetOrientationDegrees;
	m_treadTraction = 0.075f;
	cyclePatrol();
	m_isTurning=false;
	m_hasHealthBar=true;
	m_shouldDiscardOnDeath=false;
	m_secondsSinceDeath=0.f;
	m_firingSound = g_theAudio->createOrGetSound("Data/Audio/EnemyBulletShoot.ogg");
	m_explosionSound = g_theAudio->createOrGetSound("Data/Audio/Explosion.ogg");
	m_shouldExplodeOnDeath = true;
	m_firingData.m_accuracy = getBellRandInt(FiringData::MIN_SHOOTING_ACCURACY,FiringData::MAX_SHOOTING_ACCURACY,3);
	m_firingData.m_range = getBellRandInt(FiringData::MIN_SHOOTING_RANGE, FiringData::MAX_SHOOTING_RANGE,3);
	}


EnemyTank::~EnemyTank()
	{
	}

void EnemyTank::update(float deltaSeconds)
	{
	updateStart(deltaSeconds);
	if (!isDead())
		{
		m_bulletCooldownSeconds = maxOf(m_bulletCooldownSeconds - deltaSeconds, 0.f);
		updateAI(deltaSeconds);
		}
	updateEnd(deltaSeconds);
	}

void EnemyTank::accelerate(float deltaSeconds)
	{
	Entity::accelerate(deltaSeconds);
	}

void EnemyTank::render() const
	{
	if(isDead())
		return;
	Entity::render();
	if(g_doShowDebugGraphics)
		drawLineToTargetTank();
	}

void EnemyTank::capSpeed()
	{
	if (m_vel != Vector2::ZERO)
		{
		float topspeed = ENEMY_TANK_TOP_SPEED*m_fractionSpeed;
		if (m_vel.CalcLengthSquared() > topspeed*topspeed)
			{
			m_vel.setLength(topspeed);
			}
		}
	}

void EnemyTank::updateAI(float deltaSeconds)
	{
	PlayerTank* targetTank=nullptr;
	if (canSeePlayerTank(targetTank))
		{
		m_tankState = ENEMY_TANK_STATE_PURSUING;
		m_targetTank=targetTank;
		if (targetTank != nullptr)
			{
			updatePursuit(deltaSeconds);
			}
		}
	else
		{
		m_tankState = ENEMY_TANK_STATE_PATROLLING;
		updatePatrol(deltaSeconds);
		}
	}

bool EnemyTank::canSeePlayerTank(PlayerTank*& targetTank)
	{
	bool result=false;
    float minDistSquared=(float)1E10;
	int indexOfClosestSeenTank=-1;
	std::vector<PlayerTank*> playertanks=g_theMap->findAllLivingPlayerTanks();
	for (int itank = 0; itank < (int)playertanks.size() ; itank++)
		{
		Vector2 curEnemyPos= playertanks[itank]->getpos();
		float maxDistance = (float)m_firingData.m_range;
		if (playertanks[itank] && !playertanks[itank]->isDead() && g_theMap->hasLineOfSight(m_pos, curEnemyPos, maxDistance))
			{
			float curEnemyDistSquared= CalcDistanceSquared(m_pos,curEnemyPos);
			if (curEnemyDistSquared<minDistSquared)
				{
				minDistSquared=curEnemyDistSquared;
				indexOfClosestSeenTank=itank;
				result=true;
				}
			}
		}
	if (result)
		{
		targetTank=playertanks[indexOfClosestSeenTank];
		}
	return result;	
	}

void EnemyTank::updatePatrol(float deltaSeconds)
	{
	float ds=deltaSeconds;
	while(ds>PATROL_CHANGE_INTERVAL_IN_SECONDS) 
		ds-=PATROL_CHANGE_INTERVAL_IN_SECONDS;
	if (ds >= m_patrolChangeCountdownSeconds)
		{
		cyclePatrol();
		ds=0.f;
		}
	float progress = calcPatrolTimeScale();
	m_vel = m_patrolTopVel*progress + m_patrolBaseVel*(1.f - progress);
	m_goalOrientationDegrees=m_vel.calcHeadingDegrees();
	m_isTurning=true;
	//m_goalOrientationDegrees=m_patrolTargetOrientationDegrees*progress + m_patrolStartOrientationDegrees*progress;
	m_patrolChangeCountdownSeconds-=ds;
	turnTowardsGoal(deltaSeconds);
	move(deltaSeconds);
	capSpeed();
	applyTraction();
	}

void EnemyTank::cyclePatrol()
	{
	fixOrientation();
// 	while (false)
// 		{
// 		if (m_orientationDegrees < -90.f)
// 			break;
// 		break;
// 		}
	m_patrolTargetOrientationDegrees=fixDegrees180(GetRandomFloatInRange(m_orientationDegrees-90.f,m_orientationDegrees+90.f));
	m_patrolStartOrientationDegrees=m_orientationDegrees;
	m_patrolTopVel=calcPatrolTopVector();
	m_patrolChangeCountdownSeconds=PATROL_CHANGE_INTERVAL_IN_SECONDS;
	}

float EnemyTank::calcPatrolTimeScale()
	{
	float x=(PATROL_CHANGE_INTERVAL_IN_SECONDS-m_patrolChangeCountdownSeconds)/ PATROL_CHANGE_INTERVAL_IN_SECONDS;
	int exp=2;
	int amtRecursions=2;
	float t=smootherStepN(x,exp,exp, amtRecursions);
	float result=clampFloat(t,0.f,1.f);
	return result;
	}

void EnemyTank::fixHeading()
	{
// 	if(m_vel.CalcLengthSquared()>0.f)
// 		m_orientationDegrees=m_goalOrientationDegrees=m_patrolStartOrientationDegrees=m_patrolTargetOrientationDegrees=getVelocityAngleDegrees();
	}

void EnemyTank::respawn(const Vector2& newpos)
	{
	m_pos=newpos;
	m_vel.SetXY(0.f,0.f);
	m_patrolChangeCountdownSeconds = PATROL_CHANGE_INTERVAL_IN_SECONDS;
	m_patrolTargetOrientationDegrees = 0.f;
	m_targetTank = nullptr;
	m_isActive = false;
	m_gunOrientationDegrees = 0.f;
	m_gunGoalOrientationDegrees = 0.f;
	m_isAiming = false;
	m_fractionSpeed = 1.f;
	m_bulletCooldownSeconds = 0.f;
	initHitPoints(STARTING_ENEMY_TANK_HIT_POINTS);
	m_patrolTargetOrientationDegrees = GetRandomFloatInRange(0.f, 360.f);
	m_orientationDegrees = m_patrolTargetOrientationDegrees;
	m_treadTraction = 0.075f;
	cyclePatrol();
	m_isTurning = false;
	m_hasHealthBar = true;
	m_shouldDiscardOnDeath = false;	
	m_secondsSinceDeath = 0.f;
	}

Vector2 EnemyTank::calcPatrolTopVector()
	{
	float x=cosDegrees(m_patrolTargetOrientationDegrees);
	float y=-sinDegrees(m_patrolTargetOrientationDegrees);
	Vector2 result(x,y);
	result.setLength(ENEMY_TANK_PATROL_SPEED);
	return result;
	}

void EnemyTank::drawLineToTargetTank() const
	{
	Rgba targetLineColor=Rgba::LIME;
	float linethickness=2.f;
	if (m_tankState==ENEMY_TANK_STATE_PURSUING && m_targetTank)
		{
		g_theRenderer->drawLine(LineSegment2(m_pos,m_targetTank->getpos()),targetLineColor,linethickness);
		}
	}

void EnemyTank::updatePursuit(float deltaSeconds)
	{
	// TODO: fix turning rate so that it is faster
	if(m_targetTank==nullptr)
		return;
	Vector2 goalHeading= m_targetTank->getpos() - m_pos;
	goalHeading.Normalize();
	float goalOrientationDegrees=goalHeading.calcHeadingDegrees();
// 	if (goalOrientationDegrees!=m_orientationDegrees)
// 		m_vel = m_vel*0.5f;
	m_accel=goalHeading;
	m_accel.setLength(ENEMY_TANK_PURSUIT_ACCELERATION_RATE);
	accelerate(deltaSeconds);
	//m_goalOrientationDegrees=m_vel.calcHeadingDegrees();
	m_goalOrientationDegrees = goalOrientationDegrees;
	m_isTurning=true;
	turnTowardsGoal(deltaSeconds);
	move(deltaSeconds);
	capSpeed();
	applyTraction();
	shootAtPlayer();
	}

void EnemyTank::shootAtPlayer()
	{
	float svOrientationDegrees = m_orientationDegrees;
	float degreeDeviation = calcAimDeviationBasedOnAccuracy();
	m_orientationDegrees = m_orientationDegrees += GetRandomFloatInRange(-degreeDeviation, degreeDeviation);
	if (m_bulletCooldownSeconds == 0  && shouldShootAtPlayer())
		{
		Vector2 gunTipPos=m_pos+m_currentHeading*m_cosmeticRadius;
		g_theMap->spawnEnemyBullet(m_currentHeading,gunTipPos,m_orientationDegrees);
		g_theAudio->playSound(m_firingSound);
		m_bulletCooldownSeconds = MIN_SECONDS_BETWEEN_ENEMY_TANK_BULLET_FIRES;
		}
	m_orientationDegrees = svOrientationDegrees;
	}

bool EnemyTank::shouldShootAtPlayer()
	{
	static const float degreeTolerance = 10.f;
	static const float minProjectionLength = Vector2(cosDegrees(degreeTolerance), sinDegrees(degreeTolerance)).dotProduct(Vector2(1.f, 0.f));
	if (m_tankState == ENEMY_TANK_STATE_PURSUING && m_targetTank != nullptr)
		{
		Vector2 targetDisplacement = m_targetTank->getpos() - m_pos;
		Vector2 targetDir = targetDisplacement.getNormalized();
		m_currentHeading = calcHeadingVector();
		float dot = targetDir.dotProduct(m_currentHeading);
		bool result=dot>minProjectionLength;
		return result;
		}
	else
		return false;
	}
