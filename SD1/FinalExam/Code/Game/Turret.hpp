#pragma once
#include "Game/Entity.hpp"
class Turret: 	public Entity
	{
	public:
		Turret(const Vector2& pos);
		static Turret* createAimingTurret(const Vector2& pos);
		static Turret* createHomingTurret(const Vector2& pos);

		float m_cooldownSeconds;
		Texture* m_mountedSpriteTexture;

		bool m_canAim;
		bool m_doesShootSeekers;

		float m_shootingIntervalSeconds;

		virtual void update(float deltaSeconds);
		virtual void render() const;
		virtual void shoot();
		void drawMountedSprite() const;
		void drawDarkMovingSprite() const;

		bool doesHaveLineOfSightToPlayer();
	private:
		void aim();
	};

