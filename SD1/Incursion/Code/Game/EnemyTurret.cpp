#include "Game/EnemyTurret.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Game/GameCommon.hpp"

//TODO: finish implementing EnemyTurret class

EnemyTurret::EnemyTurret(int indexIntoGameArray /*= 0*/, const Vector2& pos /*= Vector2::ZERO*/, float radius /*= ENEMY_TANK_STARTING_RADIUS*/)
	: EnemyTank(indexIntoGameArray,pos,radius,0.f)
	{
	m_type=ENTITY_TYPE_ENEMY_TURRET;
	m_hasHealthBar = true;
	m_shouldDiscardOnDeath = false;
	m_treadTraction = 1.f;
	m_turningRate = ENEMY_TURRET_FIXED_BASE_TURNING_RATE;
	m_gunOrientationDegrees = m_gunGoalOrientationDegrees = fixDegrees180(GetRandomFloatInRange(0.f, 360.f));
	m_orientationDegrees = m_goalOrientationDegrees = ENEMY_TURRET_FIXED_BASE_ORIENTATION;
	m_baseTexture = g_theRenderer->CreateOrGetTexture("Data/Images/EnemyTurretBase.png");
	m_gunTexture = g_theRenderer->CreateOrGetTexture("Data/Images/EnemyTurretTop.png");
	m_turretState = ENEMY_TURRET_STATE_SEEKING;
	m_firingSound = g_theAudio->createOrGetSound("Data/Audio/EnemyBulletShoot.ogg");
	m_explosionSound = g_theAudio->createOrGetSound("Data/Audio/Explosion.ogg");
	m_shouldExplodeOnDeath = true;
	m_isMoveable=false;
	cyclePatrol();
	}

void EnemyTurret::update(float deltaSeconds)
	{
	updateStart(deltaSeconds);
	if (!isDead())
		{
		m_bulletCooldownSeconds = maxOf(m_bulletCooldownSeconds - deltaSeconds, 0.f);
		updateInput(deltaSeconds);
		updateAI(deltaSeconds);
		}
	updateEnd(deltaSeconds);
	}

void EnemyTurret::updatePatrol(float deltaSeconds)
	{
	if(m_turretState != ENEMY_TURRET_STATE_SEEKING)
		cyclePatrol();
	m_turretState = ENEMY_TURRET_STATE_SEEKING;

	float ds = deltaSeconds;
	while (ds > PATROL_CHANGE_INTERVAL_IN_SECONDS)
		ds -= PATROL_CHANGE_INTERVAL_IN_SECONDS;
	if (ds >= m_patrolChangeCountdownSeconds)
		{
		cyclePatrol();
		ds = 0.f;
		}
	float progress = calcPatrolTimeScale();
	m_gunOrientationDegrees = m_patrolTargetOrientationDegrees*progress
		                    + m_patrolStartOrientationDegrees*(1.f - progress);
	m_vel=Vector2::ZERO;
	m_isAiming = false;
	m_patrolChangeCountdownSeconds -= ds;
	}

void EnemyTurret::updatePursuit(float deltaSeconds)
	{
	m_turretState = ENEMY_TURRET_STATE_AIMING;
	m_vel = Vector2::ZERO;
	//TODO: continue making this updatePursuit method
	if (m_targetTank && !m_targetTank->isDead())
		{
		Vector2 displacement = m_targetTank->getpos()-m_pos;
		m_gunGoalOrientationDegrees=displacement.calcHeadingDegrees();
		m_isAiming=true;
		}

	if (m_isAiming)
		aimTowardsGoal(deltaSeconds);

	if(shouldShootAtPlayer())
		shootAtPlayer();
	}

void EnemyTurret::cyclePatrol()
	{
	m_patrolTargetOrientationDegrees = fixDegrees180(GetRandomFloatInRange(m_gunOrientationDegrees - 90.f, m_gunOrientationDegrees + 90.f));
	m_patrolStartOrientationDegrees = m_gunOrientationDegrees;
	m_patrolChangeCountdownSeconds = PATROL_CHANGE_INTERVAL_IN_SECONDS;
	}

void EnemyTurret::accelerate(float )
	{
	
	}

void EnemyTurret::render() const
	{
	if (isDead())
		return;
	Entity::render();
	drawGunSprite();
	if (g_doShowDebugGraphics)
		drawLineToTargetTank();
	}

void EnemyTurret::capSpeed()
	{
	m_vel.setLength(0.f);
	}

void EnemyTurret::respawn(const Vector2& newpos)
	{
	m_pos=newpos;
	m_hasHealthBar = true;
	m_treadTraction = 1.f;
	m_turningRate = ENEMY_TURRET_FIXED_BASE_TURNING_RATE;
	m_gunOrientationDegrees = m_gunGoalOrientationDegrees = fixDegrees180(GetRandomFloatInRange(0.f, 360.f));
	m_pos = newpos;
	m_vel.SetXY(0.f, 0.f);
	m_patrolChangeCountdownSeconds = PATROL_CHANGE_INTERVAL_IN_SECONDS;
	m_patrolTargetOrientationDegrees = 0.f;
	m_targetTank = nullptr;
	m_isActive = false;
	m_isAiming = false;
	m_fractionSpeed = 1.f;
	m_bulletCooldownSeconds = 0.f;
	initHitPoints(STARTING_ENEMY_TURRET_HIT_POINTS);
	m_patrolTargetOrientationDegrees = GetRandomFloatInRange(0.f, 360.f);
	m_orientationDegrees = m_patrolTargetOrientationDegrees;
	m_treadTraction = 0.075f;
	m_isTurning = false;
	m_hasHealthBar = true;
	m_shouldDiscardOnDeath = false;
	m_secondsSinceDeath = 0.f;
	cyclePatrol();
	}

void EnemyTurret::updateAI(float deltaSeconds)
	{
	PlayerTank* targetTank = nullptr;
	if (canSeePlayerTank(targetTank))
		{
		m_targetTank = targetTank;
		if (targetTank != nullptr)
			{
			updatePursuit(deltaSeconds);
			}
		}
	else
		{
		updatePatrol(deltaSeconds);
		}
	}

void EnemyTurret::shootAtPlayer()
	{
	float svOrientationDegrees = m_gunOrientationDegrees;
	float degreeDeviation = calcAimDeviationBasedOnAccuracy();
	m_gunOrientationDegrees = m_gunOrientationDegrees += GetRandomFloatInRange(-degreeDeviation, degreeDeviation);
	if (m_bulletCooldownSeconds == 0 && shouldShootAtPlayer())
		{
		Vector2 gunTipPos = m_pos + m_currentGunHeading*m_cosmeticRadius;
		g_theMap->spawnEnemyBullet(m_currentGunHeading, gunTipPos, m_gunOrientationDegrees);
		g_theAudio->playSound(m_firingSound);
		m_bulletCooldownSeconds = MIN_SECONDS_BETWEEN_ENEMY_TURRET_BULLET_FIRES;
		}
	m_gunOrientationDegrees = svOrientationDegrees;	
	}

bool EnemyTurret::shouldShootAtPlayer()
	{
	static const float degreeTolerance = 10.f;
	static const float minProjectionLength = Vector2(cosDegrees(degreeTolerance), sinDegrees(degreeTolerance)).dotProduct(Vector2(1.f, 0.f));
	if (m_turretState == ENEMY_TURRET_STATE_AIMING && m_targetTank != nullptr)
		{
		Vector2 targetDisplacement = m_targetTank->getpos() - m_pos;
		Vector2 targetDir = targetDisplacement.getNormalized();
		m_currentGunHeading = calcGunHeadingVector();
		float dot = targetDir.dotProduct(m_currentGunHeading);
		bool result = dot > minProjectionLength;
		return result;
		}
	else
		return false;
	}

void EnemyTurret::drawLineToTargetTank() const
	{
	Rgba targetLineColor = Rgba::LIME;
	float linethickness = 2.f;
	if (m_turretState == ENEMY_TURRET_STATE_AIMING && m_targetTank)
		{
		g_theRenderer->drawLine(LineSegment2(m_pos, m_targetTank->getpos()), targetLineColor, linethickness);
		}
	}

