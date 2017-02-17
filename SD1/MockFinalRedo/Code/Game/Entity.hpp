#pragma once

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/AABB2.hpp"

const float GRAPHICS_TO_PHYSICS_RADIUS_RATIO = 0.7071f;

namespace ENTITY
	{
	enum TYPE
		{
		SHIP,
		ENEMY,
		BULLET,
		EXPLOSION,
		DEATH_ANIMATION,
		WIN_ANIMATION,
		};
	};

const float ENEMY_COSMETIC_RADIUS = 25.f;
const float EXPLOSION_RADIUS = 30.f;
const float WIN_ANIMATION_LAST_FRAME=50;
const float DEATH_ANIMATION_LAST_FRAME=50;

class Entity
	{
	public: //constructor
		Entity(ENTITY::TYPE etype, const Vector2& pos,float radius,const Vector2& vel,float orideg);

	public: // static factories
		static Entity* createShip(const Vector2& pos);
		static Entity* createBullet(const Vector2& bulletpos,const Vector2& bulletvel, float bulletorientationDegrees);
		static Entity* createEnemy(const Vector2& pos);
		static Entity* createExplosion(const Vector2& pos, float radius);
		static Entity* createWinAnimation();		
		static Entity* createDeathAnimation();


	public: // accessors
		Vector2& getpos();
		void setVelocity(const Vector2& newVelocity);
		float getMass();
		void setRadii(float radius);
		float getGraphicsRadius();
		Vector2 getVelocity();
		float getSpeed();
		float getOrientationDegrees();
		ENTITY::TYPE gettype() { return m_type; }
		void setTarget(Entity* newtarget);
		Disc2 calcCosmeticDisc();
	public: // methods
		// rendering the entity
		virtual void render() const;
			void drawVelocityVector() const;
			void drawGraphicsShell() const;
			void drawPhysicsShell() const;
			void drawSprite() const;
				AABB2 calcGraphicsBounds() const;
				void drawScreenAnimationFrame() const;
				void drawUnderglow() const;
			

		// updating the entity
		virtual void update(float deltaSeconds);
			void updateEnemyAI();
			void updateAnimation(float deltaSeconds);
			void move(float deltaSeconds);
			void rotate(float deltaSeconds);
			void updateInput(float deltaSeconds);
				void updateTurningInput();
				void updateShootingInput(float deltaSeconds);


// 		void wrapAround();
		bool isCompletelyOutOfBounds();
		bool isPartiallyOutOfBounds();
		void teleportToOtherSide();
		void setSpeedFromCurrentHeading(float speed);
		void setSpeed(float speed);
		Vector2 calcHeadingVector();
		bool doesCollideWith(const Entity& other);
		Vector2 calcMomentum();
		void incOutOfBoundsCounter(float deltaSeconds);
		void resetOutOfBoundsCounter();
		bool hasBeenOutOfBoundsTooLong();
		void adjustRotation(float degrees);
		void adjustPosition(const Vector2& displacement);
		void setDrawScale(float scale);

		void markForDeath();
		bool isDead();

	public: // friend functions
		friend void separateEntities(Entity& a, Entity& b);
		friend void collideTwoEntities(Entity& a, Entity& b);

	public: //constants
		const int AMT_BOUNDS_CIRCLE_EDGES = 24;
		const float MAX_ENTITY_OUT_OF_BOUNDS_TIME = 10.f;
	protected: // members
		ENTITY::TYPE m_type;
		Vector2 m_pos;
		Vector2 m_vel;
		float m_orientationDegrees;
/*		float m_angularVelocity;*/
		float m_graphicsRadius;
		float m_physicsRadius;
		float m_mass;
		float m_outOfBoundsCounterSeconds;
		bool m_shouldKeepInWorld;

		float m_drawScale;
		int m_spriteIndex;

		//bullet shooting
		float m_shotCooldownDurationSeconds;
		float m_shotCooldownCountdownSeconds;

		//entity death
		bool m_isEntityMarkedForDeath;

		//animation
		float m_animationFrameTimerSeconds;
		float m_animationFrameDurationSeconds;
		bool m_isAnimated;

		//enemies
		Entity* m_target;
	public:
	private:
		void keepInWorld();
	};