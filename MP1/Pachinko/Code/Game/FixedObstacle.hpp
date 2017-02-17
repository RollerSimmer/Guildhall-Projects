#pragma once

class Projectile;

class FixedObstacle
	{
	friend class Projectile;
	protected: // attributes
	public: //accessors
	public: //constructors and destructors
		FixedObstacle();
	public: // methods
		virtual void render() const;
		virtual bool doesCollideWith(const Projectile* projectile) const;
		virtual void processCollision(Projectile* projectile);
		virtual void pullApartFrom(Projectile* projectile);
		virtual void transferMomentum(Projectile* projectile);
	};
