#include "Game/EnemyBullet.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Game/PlayerTank.hpp"
#include "Game/GameCommon.hpp"

EnemyBullet::EnemyBullet(int indexIntoGameArray /*= 0*/, const Vector2& pos /*= Vector2::ZERO*/, float radius /*= 1.f*/, const Vector2& vel /*= Vector2::ZERO*/, float orientationDegrees /*= 0.f*/)
	: PlayerBullet(indexIntoGameArray, pos, radius, vel,orientationDegrees)
	{
	m_type=ENTITY_TYPE_ENEMY_BULLET;
	initHitPoints(ENEMY_BULLET_STARTING_HIT_POINT_CAPACITY);
	m_explosionSound = MISSING_SOUND_ID;
	m_shouldExplodeOnDeath = true;
	}

EnemyBullet::~EnemyBullet()
	{
	}

void EnemyBullet::render() const
	{
	PlayerBullet::render();
	}

void EnemyBullet::update(float deltaSeconds)
	{
	updateStart(deltaSeconds);
	if (!isDead())
		{
		accelerate(deltaSeconds);
		move(deltaSeconds);
		checkAndProcessPlayerHits();
		}
	updateEnd(deltaSeconds);
	}

void EnemyBullet::accelerate(float deltaSeconds)
	{
	PlayerBullet::accelerate(deltaSeconds);
	}

void EnemyBullet::move(float deltaSeconds)
	{
	PlayerBullet::move(deltaSeconds);
	}

void EnemyBullet::respawn(const Vector2& newpos)
	{
	PlayerBullet::respawn(newpos);
	}

void EnemyBullet::checkAndProcessPlayerHits()
	{
	PlayerTank* hitTank=nullptr;
	if (g_theMap->doesBulletHitAnyPlayerTank(this, hitTank) && !hitTank->isInvincible())
		{
		setHitPoints(0);
		int damage = GetRandomIntInRange(PLAYER_BULLET_DAMAGE_TO_ENEMY_TANK_AVERAGE - PLAYER_BULLET_DAMAGE_DEVIATION,
			PLAYER_BULLET_DAMAGE_TO_ENEMY_TANK_AVERAGE + PLAYER_BULLET_DAMAGE_DEVIATION);
		hitTank->doDamage(damage);
		hitTank->updateEnd(0.f);
		Game* theGame=g_theApp->getGame();
		theGame->changeScoreBy(PLAYER_DAMAGE_NEGATIVE_SCORE_FACTOR*damage);
		g_theMap->spawnFloatingScoreAt(m_pos, -damage);
		bool recentlyDiedState=hitTank->didJustDie();
		if (hitTank->isDead())
			{
			theGame->changeScoreBy(-POINTS_DEDUCTED_FOR_PLAYER_KILL);
			theGame->startScreenShake();
			g_theMap->spawnFloatingScoreAt(m_pos, -POINTS_DEDUCTED_FOR_PLAYER_KILL);
			}
		while (recentlyDiedState)
			break;
		}
	}
