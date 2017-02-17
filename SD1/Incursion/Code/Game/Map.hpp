#pragma once

#include "Game/Tile.hpp"
#include "Game/Entity.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Game/PlayerTank.hpp"
#include "Game/PlayerBullet.hpp"
#include "Game/FloatingText.hpp"
#include "Game/OccupancyMap.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/ColorCodeMap.hpp"
#include <vector>

class EnemyTank;
class EnemyBullet;

// const int AMT_TILES_X = 48;
// const int AMT_TILES_Y = 48;
const float MAP_TILE_SIZE=64.f;
const float MAP_ZOOM_RATE = 0.5f;
const int AMT_ENEMY_TANKS_ON_MAP_TO_START = 30;
const int AMT_ENEMY_TURRETS_ON_MAP_TO_START = 30;
const int AMT_GROUND_TEXTURE_TILES_x = 4;
const int AMT_GROUND_TEXTURE_TILES_Y = 1;
const float SHOCKWAVE_PARTICLE_OPACITY = 0.2f;
const int LAST_PLAY_LEVEL=1;

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
		//rendering
		void render();
			void renderAllTiles();
			void renderEntities();
			void renderSingleTile(const Tile& tile);

		//tiles
		void placeTiles();
		int getTileIndex(int x,int y);
		bool isBoundaryTile(int x,int y);
		Vector2 calcWorldPosFromTileCoords(int tileX,int tileY);
		IntVector2 calcMapTilePosFromWorldCoords(float worldX,float worldY) const;
		IntVector2 calcMapTilePosFromWorldCoords(const Vector2& worldPos) const;
		Tile findSomeOpenTile();
		Vector2 calcTileCenter(const Tile& freeTile) const;
		AABB2 calcViewport();
		AABB2 calcTileBounds(const Tile& sometile);
		bool isTileInViewport(const Tile& tile);
		Tile getTile(int x,int y);
		TileDef getTileDef(int x,int y);
		TileType getTileType(int x,int y);
		void initTileTextures();
		void updateTileOccupancyMap();
		void updateTileOccupancyForTileTypes();
		void updateTileOccupancyForEntities();

		//entities
		void spawnPlayerTankAt(const Vector2& tankPos);
		void spawnPlayerTank();
		void spawnEnemyTankAt(const Vector2& tankPos);
		void spawnEnemyTank();
		void spawnEnemyTanks(int amtTanks);
		void spawnEnemyTurretAt(const Vector2& turretPos);
		void spawnEnemyTurret();
		void spawnEnemyTurrets(int amtTurrets);
		void spawnAnimatedExplosion(const Vector2& pos,float radius,float durationSeconds);
		void spawnExplosion(const Vector2& pos,float size,float durationSeconds);
		void spawnParticleShockwave(const Vector2& pos,float size,float durationSeconds);
		PlayerTank* findFirstLivingPlayerTank();
		bool isEntityInViewport(const Entity* entity);
		void preventEntityCollisionsWithSolidTiles();
			void preventSolidTileCollisonsForSingleEntity(Entity* entity);
				std::vector<Tile> getNeighborsOfEntity(Entity* entity);
		void spawnPlayerBullet(const Vector2& heading,const Vector2& gunTipPos,float orientationDegrees);
		void spawnEnemyBullet(const Vector2& heading,const Vector2& gunTipPos,float orientationDegrees);
		void spawnFloatingTextAt(const Vector2& pos,const std::string& text,const Rgba& color=FLOATING_TEXT_DEFAULT_COLOR);
		void spawnFloatingScoreAt(const Vector2& pos,int score);
		bool doesEntityHitAnySolidTile(const Entity* entity,Axis2& axisOfHit,AABB2& tilebounds);
		bool doesBulletHitAnyPlayerTank(EnemyBullet* enemybullet,PlayerTank*& hitTank);
		bool doesBulletHitAnyTank(PlayerBullet* pbullet,EnemyTank*& hitTank);
		void respawnEntitiesWhenReady();
		void buryDeadEntities();
		void destroyEntity(int ientity);
		std::vector<PlayerTank*> findAllLivingPlayerTanks();
		int countAllLivingPlayerTanks();
		int getEntityAmount() { return m_entities.size(); }

		//updates
		void update(float deltaSeconds);
			void updateInput(float deltaSeconds);
			void separateAllEntities();
			void updateEntities(float deltaSeconds);
		void centerViewportToPlayerTank();
		
		//misc
		bool hasLineOfSight(const Vector2& startPos, const Vector2& endPos, float maxDistance);
// 		RaycastResult Raycast(const Vector2& startPos,
// 			    const Vector2& direction, float maxDistance) const;

		//game progression
		void checkForLevelVictory();
		bool isPlayerInVictoryTile();
		void progressLevel();
		void startVictory();

		//map general
		void respawnTheOnlyPlayerTank();
		void initMap();
		void resetMap();
		void clearMap();
		void loadMap();

	private:
		bool isBullet(std::size_t ientity);
		bool isEntityOnTile(Entity* curentity, int tx, int ty);
		Vector2 generateFreePosition();
		void loadTerrainFromFile(const std::string& mapImageFilePath);
		void loadCharacterPlacementsFromFile(const std::string& mapImageFilePath);
		void initMapColorCodes();

		void setStartOrEnd(TileType ttype,int tx,int ty);
		Vector2 calcTileCenterFromCoords(const IntVector2& tilecoords);
	private:
		SpriteSheet* m_tileTextures;
		std::vector<Tile> m_tiles;
		std::vector<Entity*> m_entities;
		OccupancyMap m_occupancyMap;
		Vector2 m_viewportTopLeft;
		int m_playerTankCount;
		int m_enemyTankCount;
		int m_bulletCount;
		float m_renderScale;
		int m_level;
		IntVector2 m_amtTiles;
		static ColorCodeMap<TileType> s_tercolors;
		static ColorCodeMap<EntityType> s_charcolors;
		Vector2 m_levelStartPos;
		Vector2 m_levelEndPos;
		

	public:
	};

;