#pragma once
#include "Game/Ship.hpp"

const float ENEMY_SHIP_STARTING_RADIUS = LARGE_ENTITY_RADIUS;
const float ENEMY_SHIP_STARTING_SPEED = 0.f;
const float ENEMY_SHIP_SPEED_FACTOR = 2.f;
const float ENEMY_SHIP_TURNING_RATE = 45.f*ENEMY_SHIP_SPEED_FACTOR;
const float ENEMY_SHIP_TOP_SPEED = 10.f*ENEMY_SHIP_SPEED_FACTOR;
const float ENEMY_SHIP_PATROL_SPEED = 0.75f*ENEMY_SHIP_TOP_SPEED;
const float FULL_ENEMY_SHIP_ACCELERATION = 15.f*ENEMY_SHIP_SPEED_FACTOR;
const float ENEMY_SHIP_GUN_TURNING_RATE = 90.f;
const float MIN_SECONDS_BETWEEN_ENEMY_SHIP_BULLET_FIRES = 1.f/ENEMY_SHIP_SPEED_FACTOR;
const int STARTING_ENEMY_SHIP_HIT_POINTS = 1;
const float PATROL_CHANGE_INTERVAL_IN_SECONDS = 3.f;
const float ENEMY_SHIP_MAX_VISIBILITY_DISTANCE=500.f;
const float ENEMY_SHIP_PURSUIT_SPEED = ENEMY_SHIP_TOP_SPEED;
const int ENEMY_SHIP_SPRITE_INDEX=9;

enum EnemyTankState
	{
	ENEMY_SHIP_STATE_PATROLLING,
	ENEMY_SHIP_STATE_PURSUING,
	AMT_ENEMY_SHIP_STATES
	};

class EnemyShip : public Ship
	{
	public:
		EnemyShip(int indexIntoGameArray=0,const Vector2& pos=Vector2::ZERO,float radius=ENEMY_SHIP_STARTING_RADIUS,float speed=ENEMY_SHIP_STARTING_SPEED);
		~EnemyShip();
		virtual void update(float deltaSeconds);
		virtual void accelerate(float deltaSeconds);
		virtual void render() const;
		virtual void capSpeed();
		virtual void updateAI(float deltaSeconds);
			virtual void updatePursuit(float deltaSeconds);
		float calcPatrolTimeScale();
		void fixHeading();
		virtual void respawn(const Vector2& newpos);
	protected:
		float m_patrolTargetOrientationDegrees;
		float m_patrolStartOrientationDegrees;
		Vector2 m_patrolTopVel;
		Vector2 m_patrolBaseVel;
		float m_patrolChangeCountdownSeconds;
		Ship* m_targetShip;
		Vector2 calcPatrolTopVector();
		virtual void drawLineToTargetTank() const;
		virtual void shootAtPlayer();
		virtual bool shouldShootAtPlayer();
	private:
		EnemyTankState m_tankState;
		Vector2 m_currentHeading;
	};

