#pragma once
#include "Entity.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Audio/Audio.hpp"

const float SHIP_STARTING_RADIUS = LARGE_ENTITY_RADIUS;
const float SHIP_STARTING_SPEED = 0.f;
const float SHIP_SPEED_FACTOR = 5.f;
const float SHIP_TURNING_RATE = 45.f*SHIP_SPEED_FACTOR;
const float SHIP_TOP_SPEED = 50.f*SHIP_SPEED_FACTOR;
const float FULL_SHIP_ACCELERATION = 37.f*SHIP_SPEED_FACTOR;
const float SHIP_GUN_TURNING_RATE = 45.f*SHIP_SPEED_FACTOR;
const float MIN_SECONDS_BETWEEN_PLAYER_SHIP_BULLET_FIRES = 0.5f/SHIP_SPEED_FACTOR;
const int STARTING_SHIP_HIT_POINTS = 5;
const float PLAYER_SHIP_HEALING_INTERVAL_IN_SECONDS = 0.5f;
const int PLAYER_SHIP_HEALING_AMOUNT_PER_INTERVAL = 1;

const int PLAYER_SHIP_SPRITE_INDEX=0;

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


class Ship:
	public Entity
	{
	public:
		Ship(int indexIntoGameArray=0,const Vector2& pos=Vector2::ZERO,float radius=SHIP_STARTING_RADIUS,float speed=SHIP_STARTING_SPEED);
		//~Ship();
		virtual void render() const;
		virtual void update(float deltaSeconds);
		void updateInput(float deltaSeconds);
		void setactive(bool activeState=true);
		virtual void accelerate(float deltaSeconds);
		virtual void capSpeed();
		void drawGunSprite() const;
		void updateTurningInput();
		void updateAimingInput(float deltaSeconds);
		void aimTowardsGoal(float deltaSeconds);
		Vector2 calcGunHeadingVector();
		virtual void respawn(const Vector2& newpos);
		void stopAiming();
		void startAimingTowards(float gunGoalOrientationDegrees);
		void fireBullet();
		float calcAimDeviationBasedOnAccuracy();
	protected:
		bool m_isActive;
		float m_gunOrientationDegrees;
		float m_gunGoalOrientationDegrees;
		bool m_isAiming;
		float m_fractionSpeed;
		float m_bulletCooldownSeconds;
		SoundID m_firingSound;
		FiringData m_firingData;
	};
