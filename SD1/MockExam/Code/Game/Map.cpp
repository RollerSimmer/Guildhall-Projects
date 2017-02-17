#include "Game/Map.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Ship.hpp"
#include "Game/EnemyShip.hpp"
#include "Game/App.hpp"
// #include "Game/EnemyTurret.hpp"
#include "Game/PlayerBullet.hpp"
// #include "Game/EnemyBullet.hpp"
// #include "Game/Particle.hpp"
// #include "Game/Explosion.hpp"
// #include "Game/FloatingText.hpp"
#include "Game/App.hpp"
 #include <cassert>
#include <sstream>
#include <string>
#include <stack>
#include <algorithm>

Map::Map()
	: m_playerShipCount(0)
	, m_enemyShipCount(0)
	, m_bulletCount(0)
	, m_renderScale(1.f)
	, m_backgroundTexture(nullptr)
	, m_enemyKillCount(0)
	{
	initBackgroundTexture();
	initMap();
	}

Map::~Map()
	{

	}

int Map::countEnemyShips()
	{
	int result=0;
	for (std::size_t ientity = 0; ientity < m_entities.size() ; ientity++)
		{
		EnemyShip* eship=(EnemyShip*)m_entities[ientity];
		if(eship && eship->gettype()==ENTITY_TYPE_ENEMY_SHIP)
			result++;
		}
	return result;
	}

void Map::render()
	{
	renderBackground();
	renderEntities();
	}

// void Map::placeTiles()
// 	{
// 	const int PCT_OF_INTERIOR_TILES_THAT_ARE_STONE = 15;
// 	for (int ty = 0; ty < AMT_TILES_Y; ty++) 
// 		{
// 		for (int tx = 0; tx < AMT_TILES_X ; tx++)
// 			{
// 			TileType ttype=TILE_TYPE_GRASS;
// 			//int itile=getTileIndex(tx,ty);
// 			if (isBoundaryTile(tx,ty) || isPercentChance(PCT_OF_INTERIOR_TILES_THAT_ARE_STONE))
// 				{
// 				ttype=TILE_TYPE_STONE;
// 				}
// 			Tile newtile(ttype,IntVector2(tx,ty));
// 			m_tiles.push_back(newtile);
// 			}
// 		}
// 	updateTileOccupancyMap();
// 	}
// 
void Map::spawnPlayerShip()
	{
	m_playerShipCount++;
	Vector2 shipPos=g_theWorldBounds->CalcCenter();
    Ship* ship= new Ship(m_entities.size(),shipPos);
	ship->setactive(m_playerShipCount==1);
	m_entities.push_back(ship);
	}

void Map::spawnEnemyShip()
	{
	if(m_enemyKillCount>=100)
		return;
	m_enemyShipCount++;
	Vector2 shipPos=g_theWorldBounds->GenerateEdgePoint(ENEMY_SHIP_STARTING_RADIUS);
	EnemyShip* ship = new EnemyShip(m_entities.size(), shipPos);
	ship->setactive(false);
	m_entities.push_back(ship);
	}

void Map::spawnEnemyShips(int amtShips)
	{
	for (int i = 0; i < amtShips ; i++)
		{
		spawnEnemyShip();
		}
	m_enemyShipCount=amtShips;
	}

void Map::update(float deltaSeconds)
	{
// 	//processInput(deltaSeconds);
	updateEntities(deltaSeconds);
// 	preventTankCollisionsWithSolidObjects();
// 	centerViewportToPlayerTank();
// 	updateInput(deltaSeconds);
// 	respawnEntitiesWhenReady();
// 	separateAllEntities();
	processEntityCollisions();
	}

Ship* Map::findThePlayerShip()
	{
	for (std::size_t ientity = 0; ientity < m_entities.size() ; ientity++)
		{
		Entity* entity = m_entities.at(ientity);
		if (entity&& entity->gettype()==ENTITY_TYPE_PLAYER_SHIP)
			{
			Ship* ship=(Ship*)entity;
			return ship;
			}
		}
	return nullptr;
	}

// 
// PlayerTank* Map::findFirstLivingPlayerTank()
// {
// 	for (int i = 0; i < (int)m_entities.size() ; i++)
// 		{
// 		if (m_entities[i] && !m_entities[i]->isDead())
// 			{
// 			if (m_entities[i]->gettype() == ENTITY_TYPE_PLAYER_TANK)
// 				{
// 				return (PlayerTank*)m_entities[i];
// 				}
// 			}
// 		}
// 	return nullptr;
// 	}
// 

void Map::updateEntities(float deltaSeconds)
	{
	for (int ientity = 0; ientity < (int)m_entities.size() ; ientity++)
		{
		if(m_entities[ientity])
			m_entities[ientity]->update(deltaSeconds);
		}
	m_enemyShipCount=countEnemyShips();
	buryDeadEntities();
	while(m_enemyShipCount<countEnemyShips()  && m_enemyShipCount<100)
		spawnEnemyShip();
	}


void Map::renderEntities()
	{
	std::vector<Entity*> entityRenderList=m_entities;

	//TODO: for speed, possibly make entityRenderList a permanent member of Map called m_entityRenderList

	std::sort(entityRenderList.begin(),entityRenderList.end(),Entity::compareEntitiesForSort);

	for (int ientity = 0; ientity < (int)entityRenderList.size() ; ientity++)
		{
		if (entityRenderList[ientity])
			{
			while(entityRenderList[ientity]->getElevation()!=0.f)
				break;
			entityRenderList[ientity]->render();
			}		
		}
	}

void Map::spawnPlayerBullet(const Vector2& heading, const Vector2& gunTipPos, float orientationDegrees)
	{
	m_bulletCount++;
	Vector2 startingVel=heading;
	startingVel.setLength(PLAYER_BULLET_STARTING_SPEED);
	PlayerBullet* newbullet=new PlayerBullet(m_entities.size(),gunTipPos,PLAYER_BULLET_RADIUS,startingVel,orientationDegrees);
	m_entities.push_back(newbullet);
	}



void Map::buryDeadEntities()
	{
	std::stack<int> burylist;
	for (int ientity = 0; ientity < (int)m_entities.size() ; ientity++)
		{
		Entity* curentity= m_entities[ientity];
		if (curentity)
			{
			if(curentity->isDead() && curentity->shouldBeDiscardedAtTimeOfDeath())
				burylist.push(ientity);				
			}
		}
	while (!burylist.empty())
		{
		int iEntityToBury=burylist.top();
		burylist.pop();
		destroyEntity(iEntityToBury);
		}
	}

void Map::destroyEntity(int ientity)
	{
	if (ientity >= (int)m_entities.size() || ientity <= 0)
		return;
	bool shouldspawn=false;
	Entity* entity=m_entities[ientity];
	if (entity && entity->isDead() && entity->shouldBeDiscardedAtTimeOfDeath())
		{

		EntityType etype=entity->gettype();
		switch (etype)
			{
			case ENTITY_TYPE_ENEMY_SHIP: 
				m_enemyKillCount++; 
				m_enemyShipCount--;
				break;
			case ENTITY_TYPE_PLAYER_SHIP: 
				//g_theApp->m_game->setIsQuitting(true);
				break;
			}
		delete entity;
		entity=nullptr;
		}
	m_entities.erase(m_entities.begin()+ientity);
	}

// void Map::initTileTextures()
// 	{
// 	m_tileTextures = new SpriteSheet(g_theRenderer, "Data/Images/groundTextures.png", AMT_GROUND_TEXTURE_TILES_x, AMT_GROUND_TEXTURE_TILES_Y);
// 	}
// 
// void Map::updateTileOccupancyMap()
// 	{
// 	m_occupancyMap.clearAllTiles();
// 	updateTileOccupancyForTileTypes();
// 	updateTileOccupancyForEntities();
// 	}
// 
// void Map::updateTileOccupancyForTileTypes()
// 	{
// 	for (int tx = 0; tx < AMT_TILES_X ; tx++)
// 		{
// 		for (int ty = 0; ty < AMT_TILES_Y ; ty++)
// 			{
// 			Tile curtile=getTile(tx,ty);
// 			TileDef curTileDef=curtile.getdef();
// 			if (!curTileDef.m_canPassThru)
// 				{
// 				m_occupancyMap.setTileOccupancy(tx,ty,true);
// 				}
// 			}
// 		}
// 	}
// 
// void Map::updateTileOccupancyForEntities()
// 	{
// 	for (int ientity = 0; ientity < (int)m_entities.size() ; ientity++)
// 		{
// 		Entity* curentity=m_entities[ientity];
// 		if (curentity && !curentity->isDead())
// 			{
// 			if(curentity->gettype()!=ENTITY_TYPE_PLAYER_TANK && curentity->gettype()!=ENTITY_TYPE_ENEMY_SHIP)
// 				continue;				
// 			Vector2 worldCoords=curentity->getpos();
// 			IntVector2 tilepos=calcMapTilePosFromWorldCoords(worldCoords.x,worldCoords.y);
// 			int xstart = maxOf(tilepos.x - 1, 0);
// 			int xend = minOf(tilepos.x + 1, AMT_TILES_X - 1);
// 			int ystart = maxOf(tilepos.y - 1, 0);
// 			int yend = minOf(tilepos.y + 1, AMT_TILES_Y - 1);
// 			for (int tx = xstart; tx <= xend ; tx++)
// 				{
// 				for (int ty = ystart; ty <= yend ; ty++)
// 					{
// 					if(isEntityOnTile(curentity,tx,ty))
// 						m_occupancyMap.setTileOccupancy(tx,ty,true);
// 					}
// 				}
// 			}
// 		}
// 	}
// 
// bool Map::hasLineOfSight(const Vector2& startPos, const Vector2& endPos, float maxDistance)
// 	{
// 	const float LOS_TINY_STEP_LENGTH = MAP_TILE_SIZE/16.f; 
// 	Vector2 journey = endPos-startPos;
// 	if(journey.CalcLength() > maxDistance)
// 		return false;
// 	int amtSteps=(int)(journey.CalcLength()/LOS_TINY_STEP_LENGTH+0.5f);
// 	Vector2 tinyStep=journey.calcPortionOfLength(LOS_TINY_STEP_LENGTH);
// 	Vector2 curpos=startPos;
// 	IntVector2 oldTilePos = calcMapTilePosFromWorldCoords(curpos);
// 	IntVector2 tilePos;
// 	Tile curtile=getTile(tilePos.x,tilePos.y);
// 	bool canPassThru = curtile.getdef().m_canPassThru;
// 	for (int istep = 0; istep < amtSteps ; istep++)
// 		{
// 		tilePos=calcMapTilePosFromWorldCoords(curpos);
// 		if (tilePos != oldTilePos)
// 			{
// 			curtile=getTile(tilePos.x,tilePos.y);
// 			canPassThru = curtile.getdef().m_canPassThru;
// 			if(!canPassThru)
// 				return false;			
// 			}		
// 		oldTilePos=tilePos;
// 		if(istep>=amtSteps-2)
// 			curpos=endPos;
// 		else
// 			curpos+=tinyStep;
// 		}
// 	return true;
// 	}
// 
// // RaycastResult Map::Raycast(const Vector2& startPos, const Vector2& direction, float maxDistance) const
// // 	{
// // 	RaycastResult result;
// // 
// // 	// TODO: process ray cast
// // 
// // 	return result;
// // 	}
// 
// std::vector<PlayerTank*> Map::findAllLivingPlayerTanks()
// {
// 	std::vector<PlayerTank*> result;
// 	for (int ientity = 0; ientity < (int)m_entities.size() ; ientity++)
// 		{
// 		Entity* curentity = m_entities[ientity];
// 		if (curentity && !curentity->isDead() && curentity->gettype()==ENTITY_TYPE_PLAYER_TANK)
// 			result.push_back((PlayerTank*)m_entities[ientity]);
// 		}
// 	return result;
// 	}
// 
// bool Map::isEntityOnTile(Entity* curentity, int tx, int ty)
// 	{
// 	if(curentity == nullptr)
// 		return false;
// 	Disc2 entitybounds=curentity->calcPhysicsBounds();
// 	AABB2 tilebounds=calcTileBounds(getTile(tx,ty));
// 	return tilebounds.doesOverlapWith(entitybounds);
// 	}
// 
// Vector2 Map::generateFreePosition()
// 	{
// 	Vector2 result;
// 	Tile freeTile = findSomeOpenTile();
// 	result = calcTileCenter(freeTile);
// 	return result;
// 	}
// 
// void Map::clearMap()
// 	{
// 	m_entities.clear();
// 	m_tiles.clear();
// 	}
// 
void Map::initMap()
	{
	spawnEnemyShips(AMT_ENEMY_SHIPS_ON_MAP_TO_START);
	spawnPlayerShip();
	}
// 
// void Map::resetMap()
// 	{
// 	clearMap();
// 	initMap();
// 	}
// 
// void Map::spawnParticleShockwave(const Vector2& pos, float size, float durationSeconds)
// 	{
// 	const int AMT_EXPLOSION_DEGREE_STEPS=8;
// 	const float EXPLOSION_DEGREE_STEP=360.f/(float)AMT_EXPLOSION_DEGREE_STEPS;
// 	Particle* newparticle;
// 	Vector2 particleEndPos;
// 	Vector2 particleEndDisplacement;
// 	Vector2 particleVel;
// 	float pvelScale;
// 	Rgba particleColors[] = { Rgba(0xc0c0c010),Rgba(0xc0c0c010) };
// 	const int AMT_PARTICLE_RINGS = sizeof(particleColors)/sizeof(particleColors[0]);
// 	for (int i = 0; i < AMT_PARTICLE_RINGS;i++)
// 		particleColors[i].setAlphaAsFloat(SHOCKWAVE_PARTICLE_OPACITY);
// 	float sizeStep = size / (float)AMT_PARTICLE_RINGS;
// 	float particleRadius=size*0.2f;
// 	int iring=0;
// 	int amtParticles=0;
// 	float invDurationSeconds = durationSeconds == 0.f ? 0 : 1.f / durationSeconds;
// 	for (float distance = size; distance >= 0.f; distance -= sizeStep)
// 		{
// 		float startDegrees=GetRandomFloatInRange(0.f, EXPLOSION_DEGREE_STEP);
// 		for (float degrees = startDegrees; degrees <= 360.f; degrees += EXPLOSION_DEGREE_STEP)
// 			{
// 			particleEndDisplacement = Vector2(distance*cosDegrees(degrees), distance*sinDegrees(degrees));
// 			particleEndPos = pos + particleEndDisplacement;
// 			particleVel = particleEndDisplacement * invDurationSeconds;
// 			pvelScale=GetRandomFloatInRange(0.2f,5.f);
// 			particleVel*=pvelScale;
// 			int ientity=m_entities.size();
// 			newparticle = new Particle(ientity,pos, particleVel, particleRadius,durationSeconds, particleColors[iring]);
// 			m_entities.push_back(newparticle);
// 			if (iring < AMT_PARTICLE_RINGS - 1)
// 				iring++;
// 			amtParticles++;
// 			}
// 		}
// 	while(amtParticles>0)
// 		break;
// 	}
// 
// bool Map::isBullet(std::size_t ientity)
// 	{
// 	Entity* entity = m_entities[ientity];
// 	if(entity==nullptr) 
// 		return false;
// 	EntityType etype = entity->gettype();
// 	bool result = etype == ENTITY_TYPE_PLAYER_BULLET || etype == ENTITY_TYPE_ENEMY_BULLET;
// 	return result;
// 	}
// 
// void Map::spawnAnimatedExplosion(const Vector2& pos,float radius,float durationSeconds)
// 	{
// 	int indexOfNewEntity=m_entities.size();
// 	SpriteSheet* theSpriteSheet = g_theRenderer->createOrGetSpriteSheet(
// 		std::string("Data/Images/Explosion_5x5.png"),
// 					EXPLOSION_SPRITE_SHEET_AMT_TILES_WIDE,
// 					EXPLOSION_SPRITE_SHEET_AMT_TILES_HIGH);
// 	if (theSpriteSheet)
// 		{
// 		Explosion* explosion=new Explosion(indexOfNewEntity,*theSpriteSheet,pos,radius,durationSeconds);
// 		m_entities.push_back(explosion);
// 		}
// 	}
// 
// bool Map::doesBulletHitAnyTank(PlayerBullet* pbullet, EnemyTank*& hitTank)
// 	{
// 	if(pbullet==nullptr)
// 		return false;
// 	Disc2 pbulletBounds=pbullet->calcPhysicsBounds();
// 	EnemyTank* etank;
// 	Disc2 etankBounds;
// 	for (int ientity = 0; ientity < (int)m_entities.size() ; ientity++)
// 		{
// 		Entity* curentity=m_entities[ientity];
// 		if (curentity && !curentity->isDead() && curentity->gettype() == ENTITY_TYPE_ENEMY_SHIP)
// 			{
// 			etank=(EnemyTank*)curentity;
// 			etankBounds=etank->calcPhysicsBounds();
// 			if (pbulletBounds.doesOverlapWith(etankBounds))
// 				{
// 				hitTank=etank;
// 				return true;
// 				}
// 			}
// 		}
// 	return false;
// 
// 
// 	}
// 
// void Map::respawnEntitiesWhenReady()
// 	{
// 	for (int ientity = 0; ientity < (int)m_entities.size() ; ientity++)
// 		{
// 		Entity* curentity=m_entities.at(ientity);
// 		if (curentity && curentity->isDead() && curentity->shouldBeRespawned())
// 			{
// 			curentity->respawn(generateFreePosition());
// 			}
// 		}
// 	}
// 
// bool Map::doesBulletHitAnyPlayerTank(EnemyBullet* enemybullet, PlayerTank*& hitTank)
// 	{
// 	if (enemybullet == nullptr)
// 		return false;
// 	Disc2 ebulletBounds = enemybullet->calcPhysicsBounds();
// 	PlayerTank* ptank;
// 	Disc2 ptankBounds;
// 	for (int ientity = 0; ientity < (int)m_entities.size(); ientity++)
// 		{
// 		Entity* curentity = m_entities[ientity];
// 		if (curentity && !curentity->isDead() && curentity->gettype() == ENTITY_TYPE_PLAYER_TANK)
// 			{
// 			ptank = (PlayerTank*)curentity;
// 			ptankBounds = ptank->calcPhysicsBounds();
// 			if (ebulletBounds.doesOverlapWith(ptankBounds))
// 				{
// 				hitTank = ptank;
// 				return true;
// 				}
// 			}
// 		}
// 	return false;
// 	}
// 
void Map::spawnExplosion(const Vector2& /*pos*/, float /*size*/, float /*durationSeconds*/)
	{
// 	spawnParticleShockwave(pos,size, durationSeconds);
// 	spawnAnimatedExplosion(pos,size*0.5f,durationSeconds*1.25f);
// 	//TODO: change this with SpriteAnim explosion once SpriteAnim is made.
	}

void Map::initBackgroundTexture()
	{
	if(g_theRenderer)
		m_backgroundTexture = g_theRenderer->CreateOrGetTexture("Data/Images/Background.png");
	}

void Map::processEntityCollisions()
	{
	for (std::size_t ientity = 0; ientity < m_entities.size() ; ientity++)
		{
		Entity* entity = m_entities[ientity];
		if (entity)
			{
			for (std::size_t iother = 0; iother < m_entities.size() ; iother++)
				{
				Entity* other=m_entities[iother];
				if (other && iother!=ientity)
					{
					checkAndCollideTwoEntities(*entity,*other);
					}
				}
			}
		}
	}

void Map::renderBackground()
	{
	if(m_backgroundTexture==nullptr)
		return;
	AABB2 bounds=*g_theWorldBounds;
	AABB2 texcoords(Vector2(0.f,0.f),Vector2(1.f,1.f));
	Rgba tint = Rgba::WHITE;
 	g_theRenderer->drawTexturedAABB2(bounds,*m_backgroundTexture,texcoords,tint );	
	}

// 
// void Map::updateInput(float deltaSeconds)
// 	{
// 	XboxController& xbc = g_theInput->getXboxController(XCONTROLLER_0);
// 	if (g_theInput->isKeyDown(KEYCODE_OEM_PLUS) || g_theInput->isKeyDown(KEYCODE_ADD) || xbc.isXBoxButtonPressed(XBUTTON_RB))
// 		{
// 		if(m_renderScale>0.f)
// 			m_renderScale -= MAP_ZOOM_RATE*deltaSeconds;
// 		}
// 	if (g_theInput->isKeyDown(KEYCODE_OEM_MINUS) || g_theInput->isKeyDown(KEYCODE_SUBTRACT) || xbc.isXBoxButtonPressed(XBUTTON_LB))
// 		{
// 		if(m_renderScale<10.f)
// 			m_renderScale += MAP_ZOOM_RATE*deltaSeconds;
// 		}
// 	if (g_theInput->isKeyDown(KEYCODE_CONTROL))
// 		{
// 		if(g_theInput->wasKeyJustPressed('R'))
// 			resetMap();
// 		}
// 	}
// 
// void Map::separateAllEntities()
// 	{
// 	for (std::size_t ientity = 0; ientity < m_entities.size() ; ientity++)
// 		{
// 		Entity* entity=m_entities[ientity];
// 		if (entity)
// 			{
// 			if (entity == nullptr)
// 				continue;
// 			if (entity->canOverlap())
// 				continue;
// 			for (std::size_t iother = 0; iother < m_entities.size(); iother++)
// 				{
// 				Entity* other=m_entities[iother];
// 				if (other == nullptr)
// 					continue;
// 				if (other->canOverlap())
// 					continue;
// 				if (entity != other)
// 					separateTwoEntities(*entity,*other);
// 				}
// 			}
// 		}
// 	}
