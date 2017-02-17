#pragma once

#include "Game/Entity.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Game/Ship.hpp"
// #include "Game/PlayerBullet.hpp"
// #include "Game/FloatingText.hpp"
// #include "Game/OccupancyMap.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include <vector>

class EnemyShip;
class EnemyBullet;

const int AMT_TILES_X = 48;
const int AMT_TILES_Y = 48;
const float MAP_TILE_SIZE=64.f;
const float MAP_ZOOM_RATE = 0.5f;
const int AMT_ENEMY_SHIPS_ON_MAP_TO_START = 5;
const int AMT_ENEMY_TURRETS_ON_MAP_TO_START = 0;
const int AMT_GROUND_TEXTURE_TILES_x = 2;
const int AMT_GROUND_TEXTURE_TILES_Y = 1;
const float SHOCKWAVE_PARTICLE_OPACITY = 0.2f;

// struct RaycastResult
// 	{
// 	bool m_didImpact;
// 	Vector2 m_impactPos;
// 	float m_impactDist;
// 	//float m_impactFraction;
// 	Vector2 m_impactNormal;
// 	Tile* m_impactTile;
// 	};


class Map
	{
	public:
		Map();
		~Map();
		int countEnemyShips();
		void render();
		    void renderBackground();
			void renderEntities();
// 		void placeTiles();
		void spawnPlayerShip();
		void spawnEnemyShip();
		void spawnEnemyShips(int amtShips);
		Ship* findThePlayerShip();
		void update(float deltaSeconds);
// 			void updateInput(float deltaSeconds);
// 			void separateAllEntities();
			void updateEntities(float deltaSeconds);
// 		void centerViewportToPlayerTank();
// 		int getTileIndex(int x,int y);
// 		bool isBoundaryTile(int x, int y);
// 		Vector2 calcWorldPosFromTileCoords(int tileX,int tileY);
// 		IntVector2 calcMapTilePosFromWorldCoords(float worldX, float worldY) const;
// 		IntVector2 calcMapTilePosFromWorldCoords(const Vector2& worldPos) const;
// 		Tile findSomeOpenTile();
// 		Vector2 calcTileCenter(const Tile& freeTile) const;
// 		AABB2 calcViewport();
// 		AABB2 calcTileBounds(const Tile& sometile);
// 		
// 		PlayerTank* findFirstLivingPlayerTank();
// 		bool isTileInViewport(const Tile& tile);
// 		bool isEntityInViewport(const Entity* entity);
// 		void renderSingleTile(const Tile& tile);
// 		void preventTankCollisionsWithSolidObjects();
// 		Tile getTile(int x,int y);
// 		TileDef getTileDef(int x,int y);
// 		TileType getTileType(int x,int y);
		void spawnPlayerBullet(const Vector2& heading, const Vector2& gunTipPos, float orientationDegrees);
// 		void spawnEnemyBullet(const Vector2& heading, const Vector2& gunTipPos, float orientationDegrees);
// 		void spawnFloatingTextAt(const Vector2& pos, const std::string& text, const Rgba& color=FLOATING_TEXT_DEFAULT_COLOR);
// 		void spawnFloatingScoreAt(const Vector2& pos, int score);
// 		bool doesEntityHitAnySolidTile(const Entity* entity, Axis2& axisOfHit, AABB2& tilebounds);
		void buryDeadEntities();
		void destroyEntity(int ientity);
// 		void initTileTextures();
// 		OccupancyMap m_occupancyMap;
// 		void updateTileOccupancyMap();
// 			void updateTileOccupancyForTileTypes();
// 			void updateTileOccupancyForEntities();
// 		bool hasLineOfSight(const Vector2& startPos, const Vector2& endPos, float maxDistance);
// // 		RaycastResult Raycast(const Vector2& startPos,
// // 			    const Vector2& direction, float maxDistance) const;
// 		std::vector<PlayerTank*> findAllLivingPlayerTanks();
// 		bool doesBulletHitAnyTank(PlayerBullet* pbullet, EnemyTank*& hitTank);
// 		void respawnEntitiesWhenReady();
// 
// 		bool doesBulletHitAnyPlayerTank(EnemyBullet* enemybullet, PlayerTank*& hitTank);
// 		void spawnAnimatedExplosion(const Vector2& pos,float radius,float durationSeconds);
// 		void spawnExplosion(const Vector2& pos, float size, float durationSeconds);
// 			void spawnParticleShockwave(const Vector2& pos, float size, float durationSeconds);
// 
// 		int getEntityAmount() { return m_entities.size(); }
		void initMap();
// 		void resetMap();
// 		void clearMap();
		void spawnExplosion(const Vector2& pos,float size,float durationSeconds);
		void initBackgroundTexture();
		void processEntityCollisions();


	private:

// 		std::vector<Tile> m_tiles;
 		std::vector<Entity*> m_entities;
		Vector2 m_viewportTopLeft;
		int m_playerShipCount;
		int m_enemyShipCount;
		int m_bulletCount;
		float m_renderScale;
		Texture* m_backgroundTexture;
		int m_enemyKillCount;

	public:
	private:
// 		bool isEntityOnTile(Entity* curentity, int tx, int ty);
// 		Vector2 generateFreePosition();
// 		bool isBullet(std::size_t ientity);
	};

