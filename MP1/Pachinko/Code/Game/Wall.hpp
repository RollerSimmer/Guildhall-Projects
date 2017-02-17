#pragma once
#include "Game/FixedObstacle.hpp"
#include "Engine/Math/AABB2.hpp"
class Projectile;
class Wall : public FixedObstacle
	{
	public:
		Wall(float xcenter,float ycenter,float xradius,float yradius);
		~Wall();
		virtual void render() const;
		virtual bool doesCollideWith(const Projectile* projectile) const;
		virtual void processCollision(Projectile* projectile);
		virtual void pullApartFrom(Projectile* projectile);
		virtual void transferMomentum(Projectile* projectile);
		int calcProjectileZone(Projectile* projectile);
	protected:
		AABB2 m_shape;
	};

