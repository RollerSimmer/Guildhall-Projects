#pragma once

#include "Game/Tile.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Game/PlayerTank.hpp"
#include "Goal.hpp"
#include "Engine/Math/IntVector2.hpp"
#include <vector>
#include "Bullet.hpp"
#include "Turret.hpp"

class IntVector2;

const int AMT_TILES_X =16;
const int AMT_TILES_Y =9;
const float MAP_TILE_SIZE=100;

class World
	{
	public:
		World(int level);
		void render() const;
		void update(float deltaSeconds);
		void buildMap();

		IntVector2 calcMapTilePosFromWorldCoords(const Vector2& worldPos) const;
		Vector2 calcWorldPosFromTileCoords(const IntVector2& tilepos) const;
		Vector2 calcCenteredWorldPosFromTileCoords(const IntVector2& tilepos);

		PlayerTank* m_thePlayer;
		Goal* m_theGoal;

		void drawTile(const Tile& tile) const;
		void renderTiles() const;
		void renderEntities() const;
		bool isEdgeTile(int tx,int ty);

		void spawnPlayer();
		void spawnEnemies();
		void spawnGoal();
		void spawnSingleDummyTurret();
		void spawnSingleAimingTurret();
		void spawnSingleHomingTurret();

		void setSelfAndNeighborsToOffLimits(Tile tile);

		bool isLevelOver();
		void destroyPlayer();
		void updateEnemies(float deltaSeconds);
		void updateBullets(float deltaSeconds);
		void keepPlayerOnSand();
		void checkAndSeparatePlayerFromTile(const IntVector2& tilepos);
		Tile findOpenTile();
		bool isPlayerHit();
		void spawnBullet(const Vector2& bpos, float odegs, const Rgba& color, bool canSeek, float radius);

		void killBulletsThatHitWalls();
		void renderAllEnemies() const;
		void renderAllBullets() const;
		bool bulletHitsWall(Bullet* bullet);


		bool doesTurretHaveLineOfSightToPlayer(const Turret& turret);

		bool doesBulletHitWall(Bullet* bullet,const IntVector2& tilepos);
		AABB2 calcTileBounds(const IntVector2& tilepos);

		int m_level;
		Tile m_tiles[9][16];
		AABB2 m_basebounds;
		AABB2 m_portalbounds;		
		bool s_areTileSpritesInitialized;
		std::vector<Entity*> m_enemies;
		std::vector<Bullet*> m_bullets;
	private:
	};

