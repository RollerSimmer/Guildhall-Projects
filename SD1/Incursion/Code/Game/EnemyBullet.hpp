#pragma once
#include "Game/PlayerBullet.hpp"

const int ENEMY_BULLET_STARTING_HIT_POINT_CAPACITY = 1;

class EnemyBullet : public PlayerBullet
	{
	public:
		EnemyBullet(int indexIntoGameArray = 0, const Vector2& pos = Vector2::ZERO, float radius = 1.f, const Vector2& vel = Vector2::ZERO, float orientationDegrees = 0.f);
		~EnemyBullet();
		virtual void render() const;
		virtual void update(float deltaSeconds);
		virtual void accelerate(float deltaSeconds);
		virtual void move(float deltaSeconds);
		virtual void respawn(const Vector2& newpos);
	private:
		void checkAndProcessPlayerHits();
	};


