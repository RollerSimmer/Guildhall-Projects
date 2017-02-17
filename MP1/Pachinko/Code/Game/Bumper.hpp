#pragma once
#include "Game/FixedObstacle.hpp"
#include "Engine/Math/Disc2.hpp"

const int AMT_BUMPER_EDGES = 24;

class Bumper : 	public FixedObstacle
	{
	friend class Projectile;
	protected:
		Disc2 m_shape;
	public:
		Bumper(const Disc2& shape);
		~Bumper();
	public:
		const Disc2& getshape() { return m_shape; }
		void setshape(const Disc2& shape) { m_shape=shape; }
		virtual void render() const;
		virtual bool doesCollideWith(const Projectile* projectile) const;
		virtual void processCollision(Projectile* projectile);
		virtual void pullApartFrom(Projectile* projectile);
		virtual void transferMomentum(Projectile* projectile);
	};

