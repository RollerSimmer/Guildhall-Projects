#pragma once

#include "Game/FixedObstacle.hpp"
#include "Game/Projectile.hpp"

#include <vector>

const float INITIAL_BALL_SCALE=15.f;

class Game
	{
	private:
		std::vector<FixedObstacle*> m_obstacles;
		std::vector<Projectile*> m_projectiles;
		float m_ballscale;
		bool m_isPaused;
		float m_wallsize;
	public:
		Game();
		~Game();
		void init();
			void initObstacles();
				void spawnBumper(float x, float y, float radius);
				void spawnWall(float x, float y, float xradius, float yradius);
		void update(float deltaSeconds);
			void updateKeyInput(float& deltaSeconds);
			void processProjectileProjectileCollisions();
				void processSingleProjectileProjectileCollision(int iproj,int iother);
		void render();
		void toggleDebugGraphics();
		void spawnBall();
		void spawnBalls(int amount);
		void cleanUpProjectiles();
		void cleanUpObstacles();
		void togglePause();
		void destroyAnyBall();
			void destroySingleProjectile(int i);
		void destroyManyBalls(int amount);
		void processProjectileObstacleCollisions();
			void processSingleObstacleProjectileCollision(int iObstacle, int iProjectile);
		void changeGlobalElasticityBy(float changeValue);
		void scaleAllProjectilesBy(float scale);
		void toggleWaterRendering();
	private:
	};
