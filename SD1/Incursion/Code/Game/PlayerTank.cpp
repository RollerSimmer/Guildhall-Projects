#include "Game/PlayerTank.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtil.hpp"

const float FiringData::MAX_SHOOTING_ANGLE_DEVIATION_IN_DEGREES=45.f;

PlayerTank::PlayerTank (int indexIntoGameArray/*=0*/, const Vector2& pos/*=Vector2::ZERO*/, float radius/*=TANK_STARTING_RADIUS*/, float speed/*=TANK_STARTING_SPEED*/) 
	: Entity(indexIntoGameArray, ENTITY_TYPE_PLAYER_TANK,pos,radius,speed)
	, m_isActive(false)
	, m_gunTexture(nullptr)
	, m_gunOrientationDegrees(0.f)
	, m_gunGoalOrientationDegrees(0.f)
	, m_isAiming(false)
	, m_fractionSpeed(1.f)
	, m_bulletCooldownSeconds(0.f)
	, m_healingIntervalSecondsLeft(0.f)
	{
	initHitPoints(STARTING_TANK_HIT_POINTS);
	m_hasHealthBar=true;
	m_turningRate=TANK_TURNING_RATE;
	m_treadTraction=0.075f;
	m_gunTexture = g_theRenderer->CreateOrGetTexture("Data/Images/PlayerTankTop.png");
	m_baseTexture = g_theRenderer->CreateOrGetTexture("Data/Images/PlayerTankBase.png");
	m_orientationDegrees=m_goalOrientationDegrees=GetRandomFloatInRange(0.f,360.f);
	m_shouldDiscardOnDeath=false;
	m_firingSound = g_theAudio->createOrGetSound("Data/Audio/PlayerBulletShoot.ogg");
	m_explosionSound=g_theAudio->createOrGetSound("Data/Audio/Explosion.ogg");
	m_shouldExplodeOnDeath = true;
	m_firingData.m_accuracy = FiringData::DEFAULT_SHOOTING_ACCURACY;
	m_firingData.m_range = FiringData::DEFAULT_SHOOTING_RANGE;
// 	m_canOverlap=false;
// 	m_isMoveable=true;
	}


PlayerTank::~PlayerTank()
	{
	}

void PlayerTank::render() const
	{
	if(isDead())
		return;
	Entity::render();
	drawGunSprite();
	}

void PlayerTank::update(float deltaSeconds)
	{
	updateStart(deltaSeconds);
	if (!isDead())
		{
		m_bulletCooldownSeconds = maxOf(m_bulletCooldownSeconds - deltaSeconds, 0.f);
		updateInput(deltaSeconds);
		accelerate(deltaSeconds);
		move(deltaSeconds);
		if (m_isTurning)
			turnTowardsGoal(deltaSeconds);
		capSpeed();
		applyTraction();
		healPlayerTank(deltaSeconds);
		}
	updateEnd(deltaSeconds);
	}

void PlayerTank::updateInput(float deltaSeconds)
	{
	if (!m_isActive)
		return;
	bool isCtrlPressed=g_theInput->isKeyDown(KEYCODE_CONTROL);
	updateTurningInput();
	updateAimingInput();
	if(m_isAiming)
		aimTowardsGoal(deltaSeconds);
	const float  RIGHT_TRIGGER_SHOOT_THRESHOLD = 0.25f;
	if (g_theInput->isKeyDown(KEYCODE_SPACE) 
		|| g_theInput->getXboxController(XCONTROLLER_0).getTriggerValue(XBOX_TRIGGER_RIGHT) >= RIGHT_TRIGGER_SHOOT_THRESHOLD)
		fireBullet();
	if (g_theInput->wasKeyJustPressed('G') && isCtrlPressed)
		toggleEndlessInvincibility();
	}

void PlayerTank::setactive(bool activeState)
	{
	m_isActive=activeState;
	}

void PlayerTank::accelerate(float deltaSeconds)
	{
	if (m_accel.CalcLengthSquared() > 0.f)
		Entity::accelerate(deltaSeconds);
	else
		halt();
	}

void PlayerTank::capSpeed()
	{
	if (m_vel != Vector2::ZERO)
		{
		float topspeed=TANK_TOP_SPEED*m_fractionSpeed;
		if (m_vel.CalcLengthSquared() > topspeed*topspeed)
			{
			m_vel.setLength(topspeed);
			}
		}
	}

void PlayerTank::drawGunSprite() const
	{
	if (m_gunTexture)
		{
		Rgba color = Rgba::WHITE;
		if (m_isInvincibleModeActive)
			color.setAlphaAsFloat(0.5f);
		AABB2 bounds(m_pos, m_cosmeticRadius, m_cosmeticRadius);
		g_theRenderer->drawTexturedAABB2(bounds, *m_gunTexture, Vector2(0.f, 1.f), Vector2(1.f, 0.f), color,
			m_gunOrientationDegrees);
		}
	}

void PlayerTank::updateTurningInput()
	{
	if(!m_isActive)
		return;
	XboxController& xbc=g_theInput->getXboxController(XCONTROLLER_0);
	float lstickX = xbc.getStickValue(XSTICK_LEFT, X_AXIS_2D);
	float lstickY = xbc.getStickValue(XSTICK_LEFT, Y_AXIS_2D);
	m_accel = Vector2::ZERO;
	float xdir = 0.f;
	float ydir = 0.f;
	xdir=lstickX;
	ydir=lstickY;
	if (g_theInput->isKeyDown('W'))
		ydir = 1.f;
	if (g_theInput->isKeyDown('S'))
		ydir = -1.f;
	if (g_theInput->isKeyDown('A'))
		xdir = -1.f;
	if (g_theInput->isKeyDown('D'))
		xdir = 1.f;
	if (xdir != 0.f || ydir != 0.f)
		{
		float m=minOf(absval(xdir),absval(ydir));
		float M = maxOf(absval(xdir), absval(ydir));
		m_fractionSpeed=M+0.3f*m;
		float goalOrientationDegrees = atan2Degrees(ydir, xdir);
		startTurningTowards(goalOrientationDegrees);
		m_accel = calcHeadingVector();
		m_accel.setLength(FULL_TANK_ACCELERATION);
		}
	else
		stopTurning();
	}

void PlayerTank::updateAimingInput()
{
	if (!m_isActive)
		return;
	XboxController& xbc=g_theInput->getXboxController(XCONTROLLER_0);
	float rstickX = xbc.getStickValue(XSTICK_RIGHT,X_AXIS_2D);
	float rstickY = xbc.getStickValue(XSTICK_RIGHT,Y_AXIS_2D);
	float xdir = 0.f;
	float ydir = 0.f;
	xdir=rstickX;
	ydir=rstickY;
	if (g_theInput->isKeyDown('I'))
		ydir = 1.f;
	if (g_theInput->isKeyDown('K'))
		ydir = -1.f;
	if (g_theInput->isKeyDown('J'))
		xdir = -1.f;
	if (g_theInput->isKeyDown('L'))
		xdir = 1.f;
	if (xdir != 0.f || ydir != 0.f)
		{
		float m=minOf(absval(xdir),absval(ydir));
		float M = maxOf(absval(xdir),absval(ydir));
		//m_fractionSpeed=M+0.3f*m;
		float goalOrientationDegrees = atan2Degrees(ydir,xdir);
		startAimingTowards(goalOrientationDegrees);
		}
	else
		stopAiming();
	}

void PlayerTank::aimTowardsGoal(float deltaSeconds)
	{
	float less180 = m_gunGoalOrientationDegrees - 180.f;
	float more180 = m_gunGoalOrientationDegrees + 180.f;
	float degreesLeft = m_gunGoalOrientationDegrees - m_gunOrientationDegrees;
	if (degreesLeft != 0.f)
		{
		float angStep = m_turningRate*deltaSeconds;
		while (m_gunOrientationDegrees < less180)
			m_gunOrientationDegrees += 360.f;
		while (m_gunOrientationDegrees > more180)
			m_gunOrientationDegrees -= 360.f;
		float sign = getSign(degreesLeft);
		angStep = minOf(absval(degreesLeft), angStep);
		angStep *= sign;
		m_gunOrientationDegrees += angStep;
		}
	}

Vector2 PlayerTank::calcGunHeadingVector()
	{
	Vector2 result(cosDegrees(m_gunOrientationDegrees), sinDegrees(m_gunOrientationDegrees));
	result.Normalize();
	return result;
	}

void PlayerTank::respawn(const Vector2& newpos)
	{
	m_pos=newpos;
	m_vel=Vector2::ZERO;
	m_gunOrientationDegrees=0.f;
	m_gunGoalOrientationDegrees=0.f;
	m_isAiming=false;
	m_fractionSpeed=1.f;
	m_bulletCooldownSeconds=0.f;
	initHitPoints(STARTING_TANK_HIT_POINTS);
	m_hasHealthBar = true;
	m_turningRate = TANK_TURNING_RATE;
	m_treadTraction = 0.075f;
	m_gunTexture = g_theRenderer->CreateOrGetTexture("Data/Images/PlayerTankTop.png");
	m_baseTexture = g_theRenderer->CreateOrGetTexture("Data/Images/PlayerTankBase.png");
	m_orientationDegrees = m_goalOrientationDegrees = GetRandomFloatInRange(0.f, 360.f);
	m_shouldDiscardOnDeath = false;
	m_secondsSinceDeath = 0.f;
	startTemporaryInvincibility(PLAYER_TANK_RESPAWN_INVINCIBILITY_DURATION_SECONDS);
	}

void PlayerTank::stopAiming()
	{
	m_gunGoalOrientationDegrees = m_gunOrientationDegrees;
	m_isAiming = false;
	}

void PlayerTank::startAimingTowards(float gunGoalOrientationDegrees)
	{
	if (gunGoalOrientationDegrees != m_gunOrientationDegrees)
		{
		m_gunGoalOrientationDegrees = gunGoalOrientationDegrees;
		m_isAiming = true;
		}
	}

void PlayerTank::fireBullet()
	{
	if (m_bulletCooldownSeconds == 0)
		{
		float svGunOrientationDegrees = m_gunOrientationDegrees;
		float aimDeviation = calcAimDeviationBasedOnAccuracy();
		m_gunOrientationDegrees += getBellRandFloat(-aimDeviation, aimDeviation,3);
			{
			Vector2 heading = calcGunHeadingVector();
			Vector2 gunTipPos = m_pos + heading*m_cosmeticRadius;
			g_theMap->spawnPlayerBullet(heading, gunTipPos, m_gunOrientationDegrees);
			g_theAudio->playSound(m_firingSound);
			m_bulletCooldownSeconds = MIN_SECONDS_BETWEEN_PLAYER_TANK_BULLET_FIRES;
			}
		m_gunOrientationDegrees=svGunOrientationDegrees;
		}
	}

float PlayerTank::calcAimDeviationBasedOnAccuracy()
	{
	float scale=1.f-(float)m_firingData.m_accuracy/(float)FiringData::MAX_SHOOTING_ACCURACY;
	float result=scale*FiringData::MAX_SHOOTING_ANGLE_DEVIATION_IN_DEGREES;
	return result;
	}

void PlayerTank::healPlayerTank(float deltaSeconds)
	{
	m_healingIntervalSecondsLeft-=deltaSeconds;
	if (m_healingIntervalSecondsLeft < 0.f)
		m_healingIntervalSecondsLeft=0.f;
	if (m_healingIntervalSecondsLeft == 0.f)
		{
		changeHitPointsBy(PLAYER_TANK_HEALING_AMOUNT_PER_INTERVAL);
		if(m_hitPoints>m_hitPointCapacity)
			setHitPoints(m_hitPointCapacity);
		m_healingIntervalSecondsLeft = PLAYER_TANK_HEALING_INTERVAL_IN_SECONDS;
		}
	}
