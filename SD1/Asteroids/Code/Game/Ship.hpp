#pragma once

#include "Game/Entity.hpp"

const float DEFAULT_SHIP_LENGTH=100.f;

class Ship: public Entity
	{
	public: // constants
		const float SHIP_ACCELERATION_RATE = 400.f;
		const float SHIP_TURNING_RATE = 180.f;
	protected:
		Vector2 m_accel;
		float m_analogThrust;
	public:
		Ship(Vector2 pos,Vector2 vel=Vector2::ZERO,float mass=0.f,float headingDegrees=90.f,
			 float radius=75.f);
	public:
		Ship(Vector2& pos,float headingDegrees);
		virtual void update(float deltaSeconds);
		virtual void render() const;
		void accelerate(float deltaSeconds);
	public:
		void drawShip() const;
		void drawThrust() const;
		void readInput();
		void growShipByInput();
		void activateMaxForwardThrust();
		void stopThrusting();

	};

