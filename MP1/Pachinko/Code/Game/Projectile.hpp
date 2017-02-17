#pragma once

#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Rgba.hpp"
#include "Engine/Math/Disc2.hpp"

const float DEFAULT_PROJECTILE_RADIUS=10.f;


class FixedObstacle;

class Projectile
	{
	friend class FixedObstacle;
	protected: // attributes
		float m_mass;
		Vector2 m_vel;
		Vector2 m_lastPos;
		float m_physicsRadius;
		float m_cosmeticRadius;
		Vector2 m_pos;
		Rgba m_color;
	public: // accessors
		float getPhysicsRadius() const { return m_physicsRadius; }
		const Vector2& getpos() const { return m_pos; }
		void setpos(const Vector2& pos) { m_pos=pos; }
		float getmass() const { return m_mass; }
		const Vector2& getVelocity() const { return m_vel; }
		void setVelocity(const Vector2& vel) { m_vel=vel; }
	public: // ctor and dtor
		Projectile(Vector2 pos=Vector2::ZERO,
			float physicsRadius=DEFAULT_PROJECTILE_RADIUS,float cosmeticRadius=DEFAULT_PROJECTILE_RADIUS);
	public: // methods
		virtual void render() const;
		virtual void update(float deltaSeconds);
		void move(float deltaSeconds);
		void accelerate(float deltaSeconds);
		void wrapAround();
		bool isCompletelyOutOfBounds();
		void teleportToOtherSide();
		void pullApartFrom(Projectile& other);
		bool doesCollideWithOtherProjectile(const Projectile& other);
		void transferMomentumWithOtherProjectile(Projectile& other);
		virtual void scaleBy(float scale);
	public:
		Disc2 getPhysicsDisc2() const;
	private:
		void calcActualVelocity(float deltaSeconds);
		void updateMassBasedOnPhysicsRadius();
	};

