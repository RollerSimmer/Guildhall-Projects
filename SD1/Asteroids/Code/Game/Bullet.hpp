#pragma once

#include "Game/Entity.hpp"

const float BULLET_GRAPHICAL_RADIUS=2.5f;
const float BULLET_LIFESPAN = 1.f;
const int AMT_BULLET_SIDES = 8;
const float BULLET_MASS = 0.1f;

class Bullet: public Entity
	{
	private:
		float m_secondsLeftToLive;		
		bool m_isDead;
	public:
		Bullet(const Vector2& pos, Vector2& heading, float speed);
		Bullet(const Vector2& pos, Vector2& velocity);
		virtual void render() const;
		virtual void update(float deltaSeconds);
		void getOlder(float deltaSeconds);
		void deathCheck();
		float getTimeLeftToLive();
		bool isDead();
	};