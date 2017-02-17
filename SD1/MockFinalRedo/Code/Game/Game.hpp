#pragma once
#include "Engine\Renderer\Texture.hpp"
#include "Entity.hpp"
#include <vector>


const float SCREEN_SHAKE_AMOUNT=10.f;
const float SCREEN_SHAKE_TIME_SPAN = 0.5f;
const int AMT_INITIAL_ENEMIES = 5;
const int MAX_ENEMIES = 300;

class Game
	{
	public:
		Game();
		void init();

		// updating the game
		void update(float deltaSeconds);
			void updateEntities(float deltaSeconds);
				void updateEnemyTargets();
				void collideAllEntities();
				void buryDeadEntities();
				void stopEnemySpawningAtLimit();
				void updatePlayerDeath(float deltaSeconds);


		// rendering the game
		void render() const;
			void doScreenShake() const;
			void renderBackground() const;
			void renderTrauma() const;
			void renderEntities() const;
		
		// trauma
		void decreaseTrauma(float deltaSeconds);
		void increaseTraumaBy(float tinc);
		void clampTrauma();
		void changeScreenShakeDueToTrauma();
		bool isTraumaCritical() { return m_trauma>=1.f; }

		// player ship methods
		void spawnThePlayerShip();
		int tallyPlayerShips();
		Entity* findThePlayerShip() const;

		// player bullet methods
		void spawnBullet(const Vector2& bulletpos,const Vector2& bulletvel, float bulletOrientation);

		// enemy methods
		void spawnEnemy();
		void spawnEnemies(int amt);
		int countEnemies();


		// explosion methods
		void spawnExplosionAt(const Vector2& pos,float radius);

		// spawnEndAnimation
		void spawnWinAnimation();
		void spawnDeathAnimation();


		// others
		void toggleDebugGraphics();
		void togglePause();
		bool isQuitting();

	private:
		int m_timescale;
		bool m_isPaused;
		int m_gameState;
		float m_endFrameProgression;
		bool m_isQuittingGame;
		float m_currentShakeAmount;

		Entity* m_winAnimation;
		Entity* m_deathAnimation;

		Texture* m_bgtex;

		std::vector<Entity*> m_entities;

		float m_totalTimeSeconds;

		// TODO: change trauma each time a player gets hit
		float m_trauma;

		// enemy limit reached
		bool m_wasEnemyLimitEverReached;
	};
