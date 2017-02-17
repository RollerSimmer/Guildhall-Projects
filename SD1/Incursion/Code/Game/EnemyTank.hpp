#pragma once
#include "Game/PlayerTank.hpp"

const float ENEMY_TANK_STARTING_RADIUS = LARGE_ENTITY_RADIUS;
const float ENEMY_TANK_STARTING_SPEED = 0.f;
const float ENEMY_TANK_SPEED_FACTOR = 2.f;
const float ENEMY_TANK_TURNING_RATE = 45.f*ENEMY_TANK_SPEED_FACTOR;
const float ENEMY_TANK_TOP_SPEED = 40.f*ENEMY_TANK_SPEED_FACTOR;
const float ENEMY_TANK_PATROL_SPEED = 0.75f*ENEMY_TANK_TOP_SPEED;
const float FULL_ENEMY_TANK_ACCELERATION = 15.f*ENEMY_TANK_SPEED_FACTOR;
const float ENEMY_TANK_GUN_TURNING_RATE = 90.f;
const float MIN_SECONDS_BETWEEN_ENEMY_TANK_BULLET_FIRES = 1.f/ENEMY_TANK_SPEED_FACTOR;
const int STARTING_ENEMY_TANK_HIT_POINTS = 15;
const float PATROL_CHANGE_INTERVAL_IN_SECONDS = 3.f;
const float ENEMY_TANK_MAX_VISIBILITY_DISTANCE=500.f;
const float ENEMY_TANK_PURSUIT_ACCELERATION_RATE = 2.f*FULL_ENEMY_TANK_ACCELERATION;

enum EnemyTankState
	{
	ENEMY_TANK_STATE_PATROLLING,
	ENEMY_TANK_STATE_PURSUING,
	AMT_ENEMY_TANK_STATES
	};

class EnemyTank :
	public PlayerTank
	{
	public:
		EnemyTank(int indexIntoGameArray=0,const Vector2& pos=Vector2::ZERO,float radius=ENEMY_TANK_STARTING_RADIUS,float speed=ENEMY_TANK_STARTING_SPEED);
		~EnemyTank();
		virtual void update(float deltaSeconds);
		virtual void accelerate(float deltaSeconds);
		virtual void render() const;
		virtual void capSpeed();
		virtual void updateAI(float deltaSeconds);
			bool canSeePlayerTank(PlayerTank*& targetTank);
			virtual void updatePatrol(float deltaSeconds);
			virtual void updatePursuit(float deltaSeconds);
			virtual void cyclePatrol();
		float calcPatrolTimeScale();
		void fixHeading();
		virtual void respawn(const Vector2& newpos);
	protected:
		float m_patrolTargetOrientationDegrees;
		float m_patrolStartOrientationDegrees;
		Vector2 m_patrolTopVel;
		Vector2 m_patrolBaseVel;
		float m_patrolChangeCountdownSeconds;
		PlayerTank* m_targetTank;
		Vector2 calcPatrolTopVector();
		virtual void drawLineToTargetTank() const;
		virtual void shootAtPlayer();
		virtual bool shouldShootAtPlayer();
	private:
		EnemyTankState m_tankState;
		Vector2 m_currentHeading;
	};

