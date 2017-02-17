#pragma once

#include "Game/Asteroid.hpp"
#include "Game/Ship.hpp"
#include "Game/Bullet.hpp"
#include "Game/Starfield.hpp"

const int AMT_ASTEROID_SLOTS = 1000;
const int AMT_BULLET_SLOTS = 1000;
const int AMT_PLANET_SIDES=64;

enum PlayStages
	{
	PLAY_STAGE_0=0,
	PLAY_STAGE_1,
	PLAY_STAGE_2,
	PLAY_STAGE_3,
	AMT_PLAY_STAGES
	};

const float SCREEN_SHAKE_AMOUNT=10.f;
const float SCREEN_SHAKE_TIME_SPAN = 0.5f;


class Game
	{
	private:
		Asteroid* m_asteroids[AMT_ASTEROID_SLOTS];
		
		Bullet* m_bullets[AMT_BULLET_SLOTS];
		int m_timescale;
		Ship* m_theShip;
		Starfield* m_starField;
		bool m_isPaused;
		int m_gameState;
		float m_endFrameProgression;
		Disc2* m_thePlanet;
		bool m_isQuittingGame;
		int m_playStage;
		float m_currentShakeAmount;
		float m_currentShakeTimeLeft;
	public:
		Game();
		void init();
		void update(float deltaSeconds);
			void updateScreenShakeTimer(float deltaSeconds);
			void progressEndSequence(float deltaSeconds);
				bool isShipAtCenterForWarp();
				void moveShipTowardCenter(float deltaSeconds);
				void scaleShipByGameOverStage();
		void render() const;
			void doScreenShake() const;
			void renderGameplay() const;
			void renderShipCentering() const;
			void renderStarLines(int frame) const;
			void renderPlanet(int frame) const;
			void renderGameOver() const;
				void renderFadeToBlack(int iframe) const;
			void renderHUD() const;
				void renderRank() const;
		void spawnNewAsteroid();
		void destroyAsteroid(int iasteroid);
		void deleteAsteroid(int iasteroid);
		void deleteFurthestAsteroid();
		void deleteAllAsteroids();
		void deleteOutOfBoundsAsteroids(float deltaSeconds);
		void rotateAsteroids(float deltaSeconds);
		void moveAsteroids(float deltaSeconds);
		int findFurthestAsteroid(Vector2 center);
		int findEmptyAsteroidSlot();
		int findEmptyAsteroidSlotOrDestroyFurthest();
		void spawnTheShip();
		void processCollisions();
		void processShipCollisions();
			void processShipCollisionWithAsteroid(int iasteroid);
		void startScreenShake();
		void processSingleAsteroidCollision(Asteroid* a,Asteroid* b);
		void destroyTheShip();
		void toggleDebugGraphics();
		void processAsteroidCollisions();
		void processBulletCollisionsWithAsteroids();
		void processSingleBulletCollisionWithAsteroid(int ibullet,int iasteroid);
		void destroyBullet(int ibullet);
		void shootBulletFromShip();
		int findFreeBulletSlot();
		int freeOldestBullet();
		void togglePause();
		void progressPlayStage();
		void startNewStage();
		bool areAllAsteroidsDestroyed();
		void spawnThePlanet();
		bool isQuitting();
	};
