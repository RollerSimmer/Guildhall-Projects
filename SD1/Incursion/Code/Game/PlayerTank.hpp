#pragma once
#include "Game/Entity.hpp"
#include "Engine/Audio/Audio.hpp"

const float TANK_STARTING_RADIUS = LARGE_ENTITY_RADIUS;
const float TANK_STARTING_SPEED = 0.f;
const float TANK_SPEED_FACTOR = 4.f;
const float TANK_TURNING_RATE = 45.f*TANK_SPEED_FACTOR;
const float TANK_TOP_SPEED = 50.f*TANK_SPEED_FACTOR;
const float FULL_TANK_ACCELERATION = 37.f*TANK_SPEED_FACTOR;
const float TANK_GUN_TURNING_RATE = 45.f*TANK_SPEED_FACTOR;
const float MIN_SECONDS_BETWEEN_PLAYER_TANK_BULLET_FIRES = 1.f/TANK_SPEED_FACTOR;
const int STARTING_TANK_HIT_POINTS = 35;
const float PLAYER_TANK_HEALING_INTERVAL_IN_SECONDS = 0.5f;
const int PLAYER_TANK_HEALING_AMOUNT_PER_INTERVAL = 1;
const float PLAYER_TANK_RESPAWN_INVINCIBILITY_DURATION_SECONDS=3.f;

struct FiringData
	{
	static const int MAX_SHOOTING_RANGE = 600;
	static const int MIN_SHOOTING_RANGE = 100;
	static const int DEFAULT_SHOOTING_RANGE = 300;
	static const int MAX_SHOOTING_ACCURACY = 100;
	static const int MIN_SHOOTING_ACCURACY = 50;
	static const int DEFAULT_SHOOTING_ACCURACY = 75;
	static const float MAX_SHOOTING_ANGLE_DEVIATION_IN_DEGREES;
	int m_range;
	int m_accuracy;
	};

class PlayerTank :
	public Entity
	{
	protected:
		bool m_isActive;
		Texture* m_gunTexture;
		float m_gunOrientationDegrees;
		float m_gunGoalOrientationDegrees;
		bool m_isAiming;
		float m_fractionSpeed;
		float m_bulletCooldownSeconds;
		SoundID m_firingSound;
		FiringData m_firingData;
	public:
		PlayerTank(int indexIntoGameArray=0, const Vector2& pos=Vector2::ZERO, float radius=TANK_STARTING_RADIUS, float speed=TANK_STARTING_SPEED);
		~PlayerTank();
		virtual void render() const;
		virtual void update(float deltaSeconds);
		void updateInput(float deltaSeconds);
		void setactive(bool activeState=true);
		virtual void accelerate(float deltaSeconds);
		virtual void capSpeed();
		void drawGunSprite() const;
		void updateTurningInput();
		void updateAimingInput();
		void aimTowardsGoal(float deltaSeconds);
		Vector2 calcGunHeadingVector();
		virtual void respawn(const Vector2& newpos);
		void stopAiming();
		void startAimingTowards(float gunGoalOrientationDegrees);
		void fireBullet();
		float calcAimDeviationBasedOnAccuracy();
	private:
		float m_healingIntervalSecondsLeft;
	private:
		void healPlayerTank(float deltaSeconds);
	};

