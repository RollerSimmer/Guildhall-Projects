#include "Game/PlayerBullet.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Game/EnemyTank.hpp"
#include "Game/GameCommon.hpp"

PlayerBullet::PlayerBullet(int indexIntoGameArray /*=0*/, const Vector2& pos/*=Vector2::ZERO*/, float radius/*=1.f*/, const Vector2& vel/*=Vector2::ZERO*/, float orientationDegrees/*=0.f*/)
	: Entity(indexIntoGameArray,ENTITY_TYPE_PLAYER_BULLET, pos,radius, PLAYER_BULLET_STARTING_SPEED)
	{
	initHitPoints(BULLET_STARTING_HIT_POINTS);
	m_hasHealthBar = false;
	m_vel=vel;
	m_accel=Vector2::ZERO;
	m_orientationDegrees=m_goalOrientationDegrees=orientationDegrees;
	m_baseTexture=g_theRenderer->CreateOrGetTexture("Data/Images/Bullet.png");
	m_shouldDiscardOnDeath=true;
	m_secondsSinceDeath = 0.f;
	m_explosionSound=MISSING_SOUND_ID;
	m_shouldExplodeOnDeath = true;
	m_canOverlap=true;
	}


PlayerBullet::~PlayerBullet()
	{

	}

void PlayerBullet::render() const
	{
	if(isDead())
		return;
	Entity::render();
	}

void PlayerBullet::update(float deltaSeconds)
	{
	updateStart(deltaSeconds);
	if (!isDead())
		{
		accelerate(deltaSeconds);
		move(deltaSeconds);
		checkAndProcessEnemyHits();
		}
	updateEnd(deltaSeconds);
	}

void PlayerBullet::accelerate(float deltaSeconds)
	{
	Entity::accelerate(deltaSeconds);
	}

void PlayerBullet::move(float deltaSeconds)
	{
	Entity::move(deltaSeconds);
	processBulletSolidTileCollisions();
	}

void PlayerBullet::respawn(const Vector2& newpos)
	{
	m_pos = newpos;
	m_secondsSinceDeath = 0.f;
	}

void PlayerBullet::processBulletSolidTileCollisions()
	{
	Axis2 axis;
	AABB2 tilebounds=AABB2(Vector2(0.f, 0.f),Vector2(0.f, 0.f));
	if (g_theMap->doesEntityHitAnySolidTile(this,axis,tilebounds))
		{
		BoxEdge hitEdge=tilebounds.getIntersectionEdge(LineSegment2(m_prevPos,m_pos));
		Vector2 tmp=m_prevPos;
		m_pos=m_prevPos;
		m_prevPos=tmp;
		pushAwayFrom(tilebounds);
  		changeHitPointsBy(-1);
		switch (hitEdge)
			{
			case BOX_EDGE_BOTTOM: case BOX_EDGE_TOP:
				changeDirectionY();
				break;
			case BOX_EDGE_LEFT: case BOX_EDGE_RIGHT:
				changeDirectionX();
				break;
			default:
				if(axis==X_AXIS_2D)
					changeDirectionY();
				else if(axis==Y_AXIS_2D)
					changeDirectionX();
				break;
			}
		}
	}

void PlayerBullet::checkAndProcessEnemyHits()
	{
	EnemyTank* hitTank;
	if(g_theMap->doesBulletHitAnyTank(this,hitTank))
		{
		setHitPoints(0);
		int damage=GetRandomIntInRange(PLAYER_BULLET_DAMAGE_TO_ENEMY_TANK_AVERAGE - PLAYER_BULLET_DAMAGE_DEVIATION,
			                           PLAYER_BULLET_DAMAGE_TO_ENEMY_TANK_AVERAGE + PLAYER_BULLET_DAMAGE_DEVIATION);
		hitTank->doDamage(damage);
		hitTank->updateEnd(0.f);
		g_theApp->getGame()->changeScoreBy(ENEMY_DAMAGE_POSITIVE_SCORE_FACTOR*damage);
		g_theMap->spawnFloatingScoreAt(m_pos, damage);
		if (hitTank->isDead())
			{
			g_theApp->getGame()->changeScoreBy(POINTS_FOR_ENEMY_KILL);
			g_theMap->spawnFloatingScoreAt(m_pos, POINTS_FOR_ENEMY_KILL);
			}
		}
	}
