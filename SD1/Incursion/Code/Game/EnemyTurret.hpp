#pragma once
#include "Game/EnemyTank.hpp"

const float ENEMY_TURRET_SPEED_FACTOR = 2.f;
const float ENEMY_TURRET_FIXED_BASE_ORIENTATION = 0.f;
const float ENEMY_TURRET_FIXED_BASE_TURNING_RATE = 90.f;
const int ENEMY_TURRET_HIT_POINT_CAPACITY = 15;
const float MIN_SECONDS_BETWEEN_ENEMY_TURRET_BULLET_FIRES = 1.f/ENEMY_TURRET_SPEED_FACTOR;
const float ENEMY_TURRET_AIMING_RATE = 45.f*ENEMY_TURRET_SPEED_FACTOR;
const int STARTING_ENEMY_TURRET_HIT_POINTS = 15;

enum EnemyTurretState
	{
	ENEMY_TURRET_STATE_AIMING,
	ENEMY_TURRET_STATE_SEEKING,
	AMT_ENEMY_TURRET_STATES
	};

class EnemyTurret : public EnemyTank
	{
	public:
		EnemyTurret(int indexIntoGameArray = 0, const Vector2& pos = Vector2::ZERO, float radius = ENEMY_TANK_STARTING_RADIUS);
		virtual void update(float deltaSeconds);
			virtual void updatePatrol(float deltaSeconds);
			virtual void updatePursuit(float deltaSeconds);
			virtual void cyclePatrol();
		virtual void accelerate(float deltaSeconds);
		virtual void render() const;
		virtual void capSpeed();
		virtual void respawn(const Vector2& newpos);
		virtual void updateAI(float deltaSeconds);
		virtual void shootAtPlayer();
		virtual bool shouldShootAtPlayer();
		virtual void drawLineToTargetTank() const;
	private: 
		EnemyTurretState m_turretState;
		Vector2 m_currentGunHeading;
	};

