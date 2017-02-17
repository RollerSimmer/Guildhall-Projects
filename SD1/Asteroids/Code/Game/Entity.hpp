#pragma once

#include "Engine/Math/Vector2.hpp"

const float GRAPHICS_TO_PHYSICS_RADIUS_RATIO = 0.7071f;

class Entity
	{
	public: //constants
		const int AMT_BOUNDS_CIRCLE_EDGES = 24;
		const float MAX_ENTITY_OUT_OF_BOUNDS_TIME = 10.f;
	public: //constructor
		Entity(const Vector2& pos, float radius, float speed);
	protected: // members
		Vector2 m_pos;
		Vector2 m_vel;
		float m_orientationDegrees;
		float m_angularVelocity;
		float m_graphicsRadius;
		float m_physicsRadius;
		float m_mass;
		float m_outOfBoundsCounterSeconds;
		float m_drawScale;
	public: // accessors
		Vector2& getpos();
		void setVelocity(const Vector2& newVelocity);
		float getMass();
		void setRadii(float radius);
		float getGraphicsRadius();
		Vector2 getVelocity();
		float getSpeed();
		float getOrientationDegrees();
	public: // methods
		virtual void render() const;
		void drawGraphicsShell() const;
		void drawPhysicsShell() const;
		void move(float deltaSeconds);
		void rotate(float deltaSeconds);
		void wrapAround();
		virtual void update(float deltaSeconds);
		bool isCompletelyOutOfBounds();
		bool isPartiallyOutOfBounds();
		void teleportToOtherSide();
		void setSpeedFromCurrentHeading(float speed);
		void setSpeed(float speed);
		Vector2 calcHeadingVector();
		bool doesCollideWith(const Entity& other);
		void drawVelocityVector() const;
		Vector2 calcMomentum();
		void incOutOfBoundsCounter(float deltaSeconds);
		void resetOutOfBoundsCounter();
		bool hasBeenOutOfBoundsTooLong();
		void adjustRotation(float degrees);
		void adjustPosition(const Vector2& displacement);
		void setDrawScale(float scale);
	public: // friend functions
		friend void separateEntities(Entity& a, Entity& b);
	};