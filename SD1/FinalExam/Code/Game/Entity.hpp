#pragma once

#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Rgba.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Disc2.hpp"

const float GRAPHICS_TO_PHYSICS_RADIUS_RATIO = 0.7071f;
const float DEFAULT_ROTATION_RATE_DEGREES = 90.f;
class Texture;

enum EntityType
	{
	ENTITY_TYPE_PLAYER_TANK,
	ENTITY_TYPE_GOAL,
	ENTITY_TYPE_TURRET,
	ENTITY_TYPE_BULLET,
	AMT_ENTITY_TYPES
	};

class Entity
	{
	public: //constants
		const int AMT_BOUNDS_CIRCLE_EDGES = 24;
		const float MAX_ENTITY_OUT_OF_BOUNDS_TIME = 10.f;
	public: //constructor
		Entity(const Vector2& pos, EntityType etype, bool isMounted, float radius, float speed, int hpCapacity, bool canOverlap=true);
	protected: // members
		EntityType m_type;

		Vector2 m_pos;
		Vector2 m_vel;
		float m_mass;
		float m_outOfBoundsCounterSeconds;
		float m_drawScale;

		int m_hitPoints;
		int m_hitPointCapacity;
		bool m_forgetAtDeath;

		//base orientation
		float m_orientationDegrees;

		bool m_isMounted;
		bool m_canOverlap;

		Rgba m_color;

		bool m_isSpinning;
		float m_spinRateDegrees;

		//Texture* m_texture;
		//int m_spriteIndex;

		//float m_angularVelocity;

		float m_cosmeticRadius;
		float m_physicsRadius;

		Texture* m_movingSpriteTexture;

	public: // accessors
		Vector2 getpos() const;
		void setVelocity(const Vector2& newVelocity);
		float getMass();
		void setRadii(float radius);
		float getGraphicsRadius();
		Vector2 getVelocity();
		float getSpeed();
		float getOrientationDegrees();
		float getPhysicsRadius(){ return m_physicsRadius; }
		Disc2 calcPhysicsDisc() { return Disc2(m_pos,m_physicsRadius); }

	public: // methods
		// virtual nethods
		virtual void render() const;
		virtual void update(float deltaSeconds);

		// regular methods
		void drawGraphicsShell() const;
		void drawPhysicsShell() const;
		void move(float deltaSeconds);
		void wrapAround();
		bool isCompletelyOutOfBounds();
		bool isPartiallyOutOfBounds();
		void teleportToOtherSide();
		void setSpeedFromCurrentHeading(float speed);
		void setSpeed(float speed);
		Vector2 calcHeadingVector();
		void drawVelocityVector() const;
		Vector2 calcMomentum();
		void incOutOfBoundsCounter(float deltaSeconds);
		void resetOutOfBoundsCounter();
		bool hasBeenOutOfBoundsTooLong();
		void adjustRotation(float degrees);
		void adjustPosition(const Vector2& displacement);
		void setDrawScale(float scale);
		void drawMovingSprite() const;

		// collision stuff
		void pushAwayFrom(const AABB2& forbiddenZone);
		bool doesCollideWith(const Entity& other);
//		void pushAwayFrom(const AABB2& forbiddenZone);

		void startSpinning();
		void stopSpinning();
		void updateSpin(float deltaSeconds);


		void processCollisionWithOther(Entity& other);
	public: // friend functions
		friend void separateEntities(Entity& a, Entity& b);
	};