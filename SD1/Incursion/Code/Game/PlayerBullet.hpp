#pragma once
#include "Game/Entity.hpp"


const int BULLET_STARTING_HIT_POINTS = 3;
const float PLAYER_BULLET_STARTING_SPEED = 600.f;
const float PLAYER_BULLET_RADIUS = 7.f;
const int PLAYER_BULLET_DAMAGE_TO_ENEMY_TANK_AVERAGE = 5;
const int PLAYER_BULLET_DAMAGE_DEVIATION = 5;

class PlayerBullet :
	public Entity
	{
	public:
		PlayerBullet(int indexIntoGameArray =0, const Vector2& pos=Vector2::ZERO, float radius=1.f, const Vector2& vel=Vector2::ZERO, float orientationDegrees=0.f);
		~PlayerBullet();
		virtual void render() const;
		virtual void update(float deltaSeconds);
		virtual void accelerate(float deltaSeconds);
		virtual void move(float deltaSeconds);
		virtual void respawn(const Vector2& newpos);
		void processBulletSolidTileCollisions();
	private:
		void checkAndProcessEnemyHits();
	};

