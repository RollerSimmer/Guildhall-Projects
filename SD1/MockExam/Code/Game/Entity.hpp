#pragma once

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Audio/Audio.hpp"
#include "Engine/Renderer/Rgba.hpp"

const float GRAPHICS_TO_PHYSICS_RADIUS_RATIO = 0.7071f;

enum EntityType
	{
	ENTITY_TYPE_PLAYER_SHIP,
	ENTITY_TYPE_ENEMY_SHIP,
	ENTITY_TYPE_PLAYER_BULLET,
	ENTITY_TYPE_ENEMY_BULLET,
	ENTITY_TYPE_PARTICLE,
// 	ENTITY_TYPE_FLOATING_TEXT,
	ENTITY_TYPE_EXPLOSION,
	ENTITY_TYPE_GENERIC,
	AMT_ENTITY_TYPES,
	};

const float LARGE_ENTITY_RADIUS = 25.f;
const float ENTITY_HEALTH_BAR_WIDTH = 40.f;
const float ENTITY_HEALTH_BAR_HEIGHT = 5.f;
const float DEFAULT_ENTITY_TURNING_RATE = 180.f;
const Vector2 DEAD_ENTITY_POSITION = Vector2(-1000000.f,-1000000.f);
const float ENTITY_DEATH_TIME_OUT_IN_SECONDS = 5.f;
const float LARGE_ENTITY_EXPLOSION_DURATION_IN_SECONDS = 2.f;

class Entity
	{
	public: //constants
		const int AMT_BOUNDS_CIRCLE_EDGES = 24;
		const float MAX_ENTITY_OUT_OF_BOUNDS_TIME = 10.f;
	public: //constructor
		Entity(int indexIntoGameArray, EntityType mytype, const Vector2& pos, float radius, float speed);
	protected: // members
		int m_indexIntoGameArray;
		Vector2 m_pos;
		float m_elevation; // basically z order
		Vector2 m_prevPos;
		Vector2 m_vel;
		float m_orientationDegrees;
		float m_goalOrientationDegrees;
		float m_angularVelocity;
		float m_cosmeticRadius;
		float m_physicsRadius;
		float m_mass;
		float m_drawingScale;
		EntityType m_type;
		bool m_isTurning;
		float m_turningRate;
		Vector2 m_accel;

		int m_hitPointsLastFrame;
		int m_hitPoints;
		int m_hitPointCapacity;
		float m_treadTraction;
		bool m_hasHealthBar;
		bool m_shouldDiscardOnDeath;
		float m_secondsSinceDeath;
		bool m_shouldExplodeOnDeath;
		SoundID m_explosionSound;
		float m_explosionSize;
		Rgba m_primaryColor;
		Rgba m_secondaryColor;
		bool m_isMoveable;
		bool m_canOverlap;
		bool m_shouldKeepInWorld;

		int m_baseSpriteIndex;

	public: // accessors
		Vector2 getpos() const;
		void setVelocity(const Vector2& newVelocity);
		float getMass();
		void setRadii(float radius);
		float getGraphicsRadius();
		float getPhysicsRadius() const { return m_physicsRadius; }
		Vector2 getVelocity();
		float getSpeed();
		float getOrientationDegrees();
		EntityType gettype() const { return m_type; }
		void changeHitPointsBy(int hpdelta);
		void setHitPoints(int newHitPoints);
		void initHitPoints(int hitPointCapacity);
		float getElevation() const { return m_elevation; }
		bool canOverlap() const { return m_canOverlap; }
		bool isMoveable() const { return m_isMoveable; }

	public: // methods
		virtual void render() const;
			void drawHealthBar() const;
			void drawGraphicsShell() const;
			void drawPhysicsShell() const;
			void drawVelocityVector() const;
			void drawHeading() const;
			void drawGoalHeading() const;
		virtual void move(float deltaSeconds);
		void rotate(float deltaSeconds);
		void wrapAround();
		virtual void update(float deltaSeconds);
			void updateStart(float deltaSeconds);
			void applyTraction();
			void updateDeathCounter(float deltaSeconds);
			void updateEnd(float deltaSeconds);
		bool isCompletelyOutOfBounds();
		bool isPartiallyOutOfBounds();
		void teleportToOtherSide();
		void setSpeedFromCurrentHeading(float speed);
		void setSpeed(float speed);
		Vector2 calcHeadingVector() const;
		Vector2 calcGoalVector() const;
		bool doesCollideWith(const Entity& other);
		Vector2 calcMomentum();
		void adjustRotation(float degrees);
		void adjustPosition(const Vector2& displacement);
		void setDrawScale(float scale);

		Disc2 calcCosmeticBounds() const;
		Disc2 calcPhysicsBounds() const;
		AABB2 calcCosmeticBoundingBox() const;

		void halt();
		virtual bool isDead() const;
		virtual bool didJustDie() const;
		void changeDirectionX();
		void changeDirectionY();

		void startTurningTowards(float goalOrientationDegrees);
		void stopTurning();
		void turnTowardsGoal(float deltaSeconds);
		virtual void accelerate(float deltaSeconds);

		void pushAwayFrom(const AABB2& forbiddenZone);
		void checkAndPushAwayFrom(const AABB2& forbiddenZone);
		void drawBaseSprite() const;
		float getVelocityAngleDegrees();

		void fixOrientation();

		void doDamage(int damage);
		float howLongSinceDeathInSeconds();
		float shouldBeDiscardedAtTimeOfDeath();
		void setApartTheDead();

		virtual void respawn(const Vector2& newpos);
		bool shouldBeRespawned();
		void explode();

		static bool compareEntitiesForSort(Entity* one, Entity* another);

	public: // friend functions
		friend void separateTwoEntities(Entity& a, Entity& b);
		friend void processEnemyCollisionAfterCheck(Entity& a,Entity& b);
		friend bool checkCollsionBetweenTwoEntities(Entity& a,Entity& b);
		friend void checkAndCollideTwoEntities(Entity& a,Entity& b);

	public:
private:
	void startHitPointFrame();
	void keepInWorld();
	};