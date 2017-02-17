#include "Game/Map.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Game/GameCommon.hpp"
#include "Game/PlayerTank.hpp"
#include "Game/EnemyTank.hpp"
#include "Game/EnemyTurret.hpp"
#include "Game/PlayerBullet.hpp"
#include "Game/EnemyBullet.hpp"
#include "Game/Particle.hpp"
#include "Game/Explosion.hpp"
#include "Game/FloatingText.hpp"
#include <cassert>
#include <sstream>
#include <string>
#include <stack>
#include <algorithm>

ColorCodeMap<TileType> Map::s_tercolors;
ColorCodeMap<EntityType> Map::s_charcolors;

Map::Map()
	: m_playerTankCount(0)
	, m_enemyTankCount(0)
	, m_bulletCount(0)
	, m_renderScale(1.f)
	, m_occupancyMap(8,8)
	, m_tileTextures(nullptr)
	, m_level(1)
	, m_amtTiles(8,8)
	, m_levelStartPos(0.f,0.f)
	, m_levelEndPos(0.f,0.f)
	{
	initMapColorCodes();
	initTileTextures();
	initMap();
	}

Map::~Map()
	{
	delete m_tileTextures;
	m_tileTextures=nullptr;
	}

void Map::render()
	{
	renderAllTiles();
	renderEntities();
	}

void Map::placeTiles()
	{
	const int PCT_OF_INTERIOR_TILES_THAT_ARE_STONE = 15;
	for (int ty = 0; ty < m_amtTiles.y; ty++) 
		{
		for (int tx = 0; tx < m_amtTiles.x ; tx++)
			{
			TileType ttype=TILE_TYPE_GRASS;
			//int itile=getTileIndex(tx,ty);
			if (isBoundaryTile(tx,ty) || isPercentChance(PCT_OF_INTERIOR_TILES_THAT_ARE_STONE))
				{
				ttype=TILE_TYPE_STONE;
				}
			Tile newtile(ttype,IntVector2(tx,ty));
			m_tiles.push_back(newtile);
			}
		}
	updateTileOccupancyMap();
	}

void Map::spawnPlayerTank()
	{
	m_playerTankCount++;
	Tile freeTile = findSomeOpenTile();
	Vector2 tankPos=calcTileCenter(freeTile);
	spawnPlayerTankAt(tankPos);
	}

void Map::spawnEnemyTank()
	{
	m_enemyTankCount++;
	Tile freeTile = findSomeOpenTile();
	Vector2 tankPos = calcTileCenter(freeTile);
	spawnEnemyTankAt(tankPos);
	}

void Map::spawnEnemyTanks(int amtTanks)
	{
	for (int i = 0; i < amtTanks ; i++)
		{
		spawnEnemyTank();
		}
	}

void Map::spawnEnemyTurret()
	{
	m_enemyTankCount++;
	Tile freeTile = findSomeOpenTile();
	Vector2 turretPos = calcTileCenter(freeTile);
	spawnEnemyTurretAt(turretPos);
	}

void Map::spawnEnemyTurrets(int amtTurrets)
	{
	for (int i = 0; i < amtTurrets; i++)
		{
		spawnEnemyTurret();
		}
	}

void Map::update(float deltaSeconds)
	{
	updateTileOccupancyMap();
	//processInput(deltaSeconds);
	updateEntities(deltaSeconds);
	preventEntityCollisionsWithSolidTiles();
	centerViewportToPlayerTank();
	updateInput(deltaSeconds);
	//respawnEntitiesWhenReady();
	separateAllEntities();
	m_playerTankCount=countAllLivingPlayerTanks();
	if(m_playerTankCount==0  && g_theApp->getGame()->getGameState() == GAME_STATE_PLAYING
	   && g_theApp->getGame()->getAnticipatedGameState() == NEXT_GAME_STATE_KEEP_CURRENT)
		g_theApp->getGame()->startStateTransition(GAME_STATE_GAME_OVER/*,0.2f*/);
	checkForLevelVictory();
	};

void Map::centerViewportToPlayerTank()
	{
	PlayerTank* tank=findFirstLivingPlayerTank();
	if(tank==nullptr)
		return;
	float left,top,right,bottom;
	float scaledWidth= m_renderScale*g_screenWidth;
	float scaledHeight= m_renderScale*g_screenHeight;
	Vector2 tankpos=tank->getpos();
	left = maxOf(tankpos.x-scaledWidth*0.5f,0.f);
	top = maxOf(tankpos.y- scaledHeight*0.5f,0.f);
	right = minOf(left + scaledWidth, m_amtTiles.x*MAP_TILE_SIZE);
	bottom = minOf(top + scaledHeight, m_amtTiles.y*MAP_TILE_SIZE);
	top = minOf(bottom- scaledHeight,top);
	left = minOf(right- scaledWidth,left);
	m_viewportTopLeft=Vector2(left,top);
	}

int Map::getTileIndex(int tx, int y)
	{
	return y*m_amtTiles.x+tx;
	}

bool Map::isBoundaryTile(int tx, int y)
	{
	return tx==0||y==0||tx==m_amtTiles.x-1||y==m_amtTiles.y-1;
	}

Vector2 Map::calcWorldPosFromTileCoords(int tileX, int tileY)
	{
	return Vector2((float)tileX*MAP_TILE_SIZE,(float)tileY*MAP_TILE_SIZE);
	}

IntVector2 Map::calcMapTilePosFromWorldCoords(float worldX, float worldY) const
	{
	return IntVector2((int)(worldX / MAP_TILE_SIZE), (int)(worldY / MAP_TILE_SIZE));
	}

IntVector2 Map::calcMapTilePosFromWorldCoords(const Vector2& worldPos) const
	{
	return calcMapTilePosFromWorldCoords(worldPos.x,worldPos.y);
	}

Tile Map::findSomeOpenTile()
	{
	Tile result;
	int tx;
	int ty;
	do 	{
		tx = getBellRandInt(0, m_amtTiles.x - 1, 2);
		ty = getBellRandInt(0, m_amtTiles.y - 1, 2);
		int itile = getTileIndex(tx, ty);
		result = m_tiles[itile];
		}  while (m_occupancyMap.isTileOccupied(tx,ty));
	return result;
	}

Vector2 Map::calcTileCenter(const Tile& freeTile) const
	{
	IntVector2 tilepos = freeTile.getpos();
	float cx = ((float)tilepos.x + 0.5f) * MAP_TILE_SIZE;
	float cy = ((float)tilepos.y + 0.5f) * MAP_TILE_SIZE;
	return Vector2(cx,cy);
	}

AABB2 Map::calcViewport()
	{
	AABB2 result;
	result.m_mins=m_viewportTopLeft;
	result.m_maxs=m_viewportTopLeft+Vector2(g_screenWidth*m_renderScale,g_screenHeight*m_renderScale);
	return result;
	}

AABB2 Map::calcTileBounds(const Tile& sometile)
	{
	float l,t,r,b;
	IntVector2 tilepos=sometile.getpos();
	l = (float)tilepos.x*MAP_TILE_SIZE;
	t = (float)tilepos.y*MAP_TILE_SIZE;
	r = l + MAP_TILE_SIZE;
	b = t + MAP_TILE_SIZE;
	AABB2 result(l,t,r,b);
	return result;
	}

PlayerTank* Map::findFirstLivingPlayerTank()
{
	for (int i = 0; i < (int)m_entities.size() ; i++)
		{
		if (m_entities[i] && !m_entities[i]->isDead())
			{
			if (m_entities[i]->gettype() == ENTITY_TYPE_PLAYER_TANK)
				{
				return (PlayerTank*)m_entities[i];
				}
			}
		}
	return nullptr;
	}

bool Map::isTileInViewport(const Tile& tile)
	{
	AABB2 tilebounds = calcTileBounds(tile);
	AABB2 viewport = calcViewport();
	return tilebounds.doesOverlapWith(viewport);
	}

bool Map::isEntityInViewport(const Entity* entity)
	{
	assert(entity!=nullptr);
	Disc2 entityBounds=entity->calcCosmeticBounds();
	AABB2 viewport = calcViewport();
	return viewport.doesOverlapWith(entityBounds);
	}

void Map::renderSingleTile(const Tile& tile)
	{
	AABB2 tilebounds = calcTileBounds(tile);
	TileDef tdef = tile.getdef();
	TileType ttype=tdef.m_type;
	Rgba fillcolor=tdef.m_color;
	Rgba outlinecolor=Rgba::BLACK;
	if (m_tileTextures)
		{
		AABB2 texCoords(Vector2(0.f,1.f),Vector2(1.f,0.f));
		if(ttype<AMT_TILE_TYPES)
			texCoords = m_tileTextures->getTexCoordsForSpriteIndex(ttype);
		if(m_tileTextures->getTexture())
			g_theRenderer->drawTexturedAABB2(tilebounds, *m_tileTextures->getTexture(), texCoords, fillcolor);
		if(g_doShowDebugGraphics)
			g_theRenderer->drawBox(tilebounds, outlinecolor);
		}
	else
		g_theRenderer->drawBoxWithOutline(tilebounds, fillcolor, outlinecolor);
	}


void Map::updateEntities(float deltaSeconds)
	{
	for (int ientity = 0; ientity < (int)m_entities.size() ; ientity++)
		{
		if(m_entities[ientity])
			m_entities[ientity]->update(deltaSeconds);
		}
	buryDeadEntities();
	}

void Map::renderAllTiles()
{
	for (int itile = 0; itile < (int)m_tiles.size() ; itile++)
		{
		if(isTileInViewport(m_tiles[itile]))
			renderSingleTile(m_tiles[itile]);
		}
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
			if (isEntityInViewport(entityRenderList[ientity]))
				entityRenderList[ientity]->render();
			}		
			
		}
	}

void Map::preventEntityCollisionsWithSolidTiles()
{
	for (int ientity = 0; ientity < (int)m_entities.size(); ientity++)
		{
		if (m_entities[ientity] && !m_entities[ientity]->isDead())
			{
			preventSolidTileCollisonsForSingleEntity(m_entities[ientity]);
			}
		}
	}

Tile Map::getTile(int x, int y)
	{
	int itile = getTileIndex(x, y);
	Tile result = m_tiles[itile];
	return result;
	}

TileDef Map::getTileDef(int x, int y)
	{
	Tile tile = getTile(x,y);
	TileDef result = tile.getdef();
	return result;
	}

TileType Map::getTileType(int x, int y)
	{
	TileDef tiledef=getTileDef(x,y);
	return tiledef.m_type;
	}

void Map::spawnPlayerBullet(const Vector2& heading, const Vector2& gunTipPos, float orientationDegrees)
	{
	m_bulletCount++;
	Vector2 startingVel=heading;
	startingVel.setLength(PLAYER_BULLET_STARTING_SPEED);
	PlayerBullet* newbullet=new PlayerBullet(m_entities.size(),gunTipPos,PLAYER_BULLET_RADIUS,startingVel,orientationDegrees);
	m_entities.push_back(newbullet);
	}

void Map::spawnEnemyBullet(const Vector2& heading, const Vector2& gunTipPos, float orientationDegrees)
	{
	m_bulletCount++;
	Vector2 startingVel = heading;
	startingVel.setLength(PLAYER_BULLET_STARTING_SPEED);
	EnemyBullet* newbullet = new EnemyBullet(m_entities.size(), gunTipPos, PLAYER_BULLET_RADIUS, startingVel, orientationDegrees);
	m_entities.push_back(newbullet);
	}

void Map::spawnFloatingTextAt(const Vector2& pos, const std::string& text, const Rgba& color)
	{
	if (!FloatingText::haveColorsBeenInitialized())
		FloatingText::initColors();
	FloatingText* newFloatingText = new FloatingText(m_entities.size(),pos,text,color);
	m_entities.push_back(newFloatingText);
	}

void Map::spawnFloatingScoreAt(const Vector2& pos, int score)
	{
	if (!FloatingText::haveColorsBeenInitialized())
		FloatingText::initColors();
	if(score==0)
		return; // don't add zero text if there was no score change.
	while(score>=0)
		break;
	Rgba color = score >= 0? FLOATING_TEXT_POSITIVE_SCORE_COLOR: FLOATING_TEXT_NEGATIVE_SCORE_COLOR;
	std::stringstream textStream;
	textStream.str("");
	textStream << score;
	std::string text = textStream.str();
	spawnFloatingTextAt(pos,text,color);
	}

bool Map::doesEntityHitAnySolidTile(const Entity* entity, Axis2& axisOfHit, AABB2& tilebounds)
	{
	if (entity)
		{
		Disc2 entityDisc(entity->getpos(),entity->getPhysicsRadius());
		Vector2 epos=entity->getpos();
		IntVector2 entityTilePos=this->calcMapTilePosFromWorldCoords(epos.x,epos.y);
		int txCenter=entityTilePos.x;
		int tyCenter=entityTilePos.y;
		int txStart=maxOf(txCenter-1,0);
		int tyStart=maxOf(tyCenter-1,0);
		int txEnd = minOf(txCenter+1, m_amtTiles.x - 1);
		int tyEnd = minOf(tyCenter+1, m_amtTiles.y - 1);
		for (int tx = txStart; tx <= txEnd; tx++)
			{
			for (int ty = tyStart; ty <= tyEnd; ty++)
				{
				Tile& tile=m_tiles[getTileIndex(tx,ty)];
				TileDef tiledef=tile.getdef();
				if(tiledef.m_canPassThru)
					continue;
				AABB2 curtilebounds = calcTileBounds(tile);
				Axis2 curAxisOfHit;
				if (entityDisc.doesOverlapWith(curtilebounds, curAxisOfHit))
					{
					axisOfHit = curAxisOfHit;
					tilebounds=curtilebounds;
					return true;
					}
				}
			}
		}
	return false;
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
	if (m_entities[ientity])
		{
		delete m_entities[ientity];
		m_entities[ientity]=nullptr;
		}
	m_entities.erase(m_entities.begin()+ientity);
	}

void Map::initTileTextures()
	{
	m_tileTextures = new SpriteSheet(g_theRenderer, "Data/Images/groundTextures.png", AMT_GROUND_TEXTURE_TILES_x, AMT_GROUND_TEXTURE_TILES_Y);
	}

void Map::updateTileOccupancyMap()
	{
	m_occupancyMap.clearAndResize(m_amtTiles.x,m_amtTiles.y);
	updateTileOccupancyForTileTypes();
	updateTileOccupancyForEntities();
	}

void Map::updateTileOccupancyForTileTypes()
	{
	for (int ty = 0; ty < m_amtTiles.y ; ty++)
		{
		for (int tx = 0; tx < m_amtTiles.x ; tx++)
			{
			Tile curtile=getTile(tx,ty);
			TileDef curTileDef=curtile.getdef();
			if (!curTileDef.m_canPassThru)
				{
				m_occupancyMap.setTileOccupancy(tx,ty,true);
				}
			}
		}
	}

void Map::updateTileOccupancyForEntities()
	{
	for (int ientity = 0; ientity < (int)m_entities.size() ; ientity++)
		{
		Entity* curentity=m_entities[ientity];
		if (curentity && !curentity->isDead())
			{
			if(curentity->gettype()!=ENTITY_TYPE_PLAYER_TANK && curentity->gettype()!=ENTITY_TYPE_ENEMY_TANK)
				continue;				
			Vector2 worldCoords=curentity->getpos();
			IntVector2 tilepos=calcMapTilePosFromWorldCoords(worldCoords.x,worldCoords.y);
			int xstart = maxOf(tilepos.x - 1, 0);
			int xend = minOf(tilepos.x + 1, m_amtTiles.x - 1);
			int ystart = maxOf(tilepos.y - 1, 0);
			int yend = minOf(tilepos.y + 1, m_amtTiles.y - 1);
			for (int tx = xstart; tx <= xend ; tx++)
				{
				for (int ty = ystart; ty <= yend ; ty++)
					{
					if(isEntityOnTile(curentity,tx,ty))
						m_occupancyMap.setTileOccupancy(tx,ty,true);
					}
				}
			}
		}
	}

bool Map::hasLineOfSight(const Vector2& startPos, const Vector2& endPos, float maxDistance)
	{
	const float LOS_TINY_STEP_LENGTH = MAP_TILE_SIZE/16.f; 
	Vector2 journey = endPos-startPos;
	if(journey.CalcLength() > maxDistance)
		return false;
	int amtSteps=(int)(journey.CalcLength()/LOS_TINY_STEP_LENGTH+0.5f);
	Vector2 tinyStep=journey.calcPortionOfLength(LOS_TINY_STEP_LENGTH);
	Vector2 curpos=startPos;
	IntVector2 oldTilePos = calcMapTilePosFromWorldCoords(curpos);
	IntVector2 tilePos;
	Tile curtile=getTile(tilePos.x,tilePos.y);
	bool canPassThru = curtile.getdef().m_canPassThru;
	for (int istep = 0; istep < amtSteps ; istep++)
		{
		tilePos=calcMapTilePosFromWorldCoords(curpos);
		if (tilePos != oldTilePos)
			{
			curtile=getTile(tilePos.x,tilePos.y);
			canPassThru = curtile.getdef().m_canPassThru;
			if(!canPassThru)
				return false;			
			}		
		oldTilePos=tilePos;
		if(istep>=amtSteps-2)
			curpos=endPos;
		else
			curpos+=tinyStep;
		}
	return true;
	}

bool Map::isPlayerInVictoryTile()
	{
	std::vector<PlayerTank*> playerTanks=findAllLivingPlayerTanks();
	for (int itank = 0; itank < (int)playerTanks.size() ; itank++)
		{
		PlayerTank* tank=playerTanks.at(itank);
		Disc2 tankDisc(tank->getpos(),tank->getPhysicsRadius());
		Vector2 epos=tank->getpos();
		IntVector2 tankTilePos=this->calcMapTilePosFromWorldCoords(epos.x,epos.y);
		int txCenter=tankTilePos.x;
		int tyCenter=tankTilePos.y;
		int txStart=maxOf(txCenter-1,0);
		int tyStart=maxOf(tyCenter-1,0);
		int txEnd = minOf(txCenter+1,m_amtTiles.x - 1);
		int tyEnd = minOf(tyCenter+1,m_amtTiles.y - 1);
		for (int tx = txStart; tx <= txEnd; tx++)
			{
			for (int ty = tyStart; ty <= tyEnd; ty++)
				{
				Tile& tile=m_tiles[getTileIndex(tx,ty)];
				TileDef tiledef=tile.getdef();
				if (!tiledef.m_isEndTrigger)
					continue;
				AABB2 curtilebounds = calcTileBounds(tile);
				if (tankDisc.doesOverlapWith(curtilebounds))
					return true;
				}
			}
		}
	return false;
	}

void Map::checkForLevelVictory()
	{
	if (isPlayerInVictoryTile())
		{
		progressLevel();
		}
	}

void Map::progressLevel()
	{
	if(m_level<LAST_PLAY_LEVEL)
		{
		m_level++;
		resetMap();
		}
	else
		{
		startVictory();
		}
	}

void Map::startVictory()
	{
	if (g_theApp->getGame()->getAnticipatedGameState()!=GAME_STATE_VICTORY)
		g_theApp->getGame()->startStateTransition(GAME_STATE_VICTORY,0.5f);
	}

void Map::respawnTheOnlyPlayerTank()
{
	bool didFindPlayerTank=false;
	for (int ientity = 0; ientity < (int)m_entities.size(); ientity++)
		{
		PlayerTank* ptank=(PlayerTank*)m_entities[ientity];
		if (ptank->gettype()==ENTITY_TYPE_PLAYER_TANK && ptank->isDead())
			{
			ptank->respawn(ptank->getpos());
			didFindPlayerTank=true;
			break;
			}
		}
	if (!didFindPlayerTank)
		{
		//TODO: create a new tank to replace the missing one.  (it shouldn't be missing in the first place!)
		}
	m_playerTankCount=countAllLivingPlayerTanks();
	}

// RaycastResult Map::Raycast(const Vector2& startPos, const Vector2& direction, float maxDistance) const
// 	{
// 	RaycastResult result;
// 
// 	// TODO: process ray cast
// 
// 	return result;
// 	}

std::vector<PlayerTank*> Map::findAllLivingPlayerTanks()
{
	std::vector<PlayerTank*> result;
	for (int ientity = 0; ientity < (int)m_entities.size() ; ientity++)
		{
		Entity* curentity = m_entities[ientity];
		if (curentity && !curentity->isDead() && curentity->gettype()==ENTITY_TYPE_PLAYER_TANK)
			result.push_back((PlayerTank*)m_entities[ientity]);
		}
	return result;
	}

int Map::countAllLivingPlayerTanks()
	{
	std::vector<PlayerTank*> livingPlayerTanks=findAllLivingPlayerTanks();
	return (int)livingPlayerTanks.size();
	}

bool Map::isEntityOnTile(Entity* curentity, int tx, int ty)
	{
	if(curentity == nullptr)
		return false;
	Disc2 entitybounds=curentity->calcPhysicsBounds();
	AABB2 tilebounds=calcTileBounds(getTile(tx,ty));
	return tilebounds.doesOverlapWith(entitybounds);
	}

Vector2 Map::generateFreePosition()
	{
	Vector2 result;
	Tile freeTile = findSomeOpenTile();
	result = calcTileCenter(freeTile);
	return result;
	}

void Map::loadTerrainFromFile(const std::string& mapImageFilePath)
	{
	Bitmap32Bit* terbmp=g_theRenderer->createOrGetBitmap(mapImageFilePath);
	if(terbmp==nullptr)
		return;
	terbmp->flipVertically();
	m_amtTiles.x=terbmp->getWidth();
	m_amtTiles.y=terbmp->getHeight();
	for (int ty = 0; ty < m_amtTiles.y ; ty++)
		{
		for (int tx=0;tx<m_amtTiles.x;tx++)
			{
			Rgba pixel=terbmp->getPixel(tx,ty);
			TileType* ttype=s_tercolors.lookupKey(pixel);
			Tile newtile;
			const unsigned char LOW_ALPHA=128;
			if (ttype  && pixel.a >= LOW_ALPHA)
				{
				setStartOrEnd(*ttype,tx,ty);
				newtile=Tile(*ttype,IntVector2(tx,ty));
				}
			else
				newtile=Tile(TILE_TYPE_STONE,IntVector2(tx,ty));
			m_tiles.push_back(newtile);
			}
		}
	updateTileOccupancyMap();
	}

void Map::loadCharacterPlacementsFromFile(const std::string& mapImageFilePath)
	{
	Bitmap32Bit* charBmpMap = g_theRenderer->createOrGetBitmap(mapImageFilePath);
	if (charBmpMap==nullptr)
		return;
	charBmpMap->flipVertically();
	for (int ty = 0; ty < charBmpMap->getHeight(); ty++)
		{
		for (int tx = 0; tx < charBmpMap->getWidth() ; tx++)
			{
			Rgba pixel=charBmpMap->getPixel(tx,ty);
			EntityType* etypePtr=s_charcolors.lookupKey(pixel);
			const unsigned char LOW_ALPHA=128;
			if(etypePtr && pixel.a>LOW_ALPHA)
				{
				EntityType etype=*etypePtr;
				Tile tile=getTile(tx,ty);
				Vector2 pos=calcTileCenter(tile);
				switch (etype)
					{
					case ENTITY_TYPE_PLAYER_TANK: 
						spawnPlayerTankAt(pos);
						break;
					case ENTITY_TYPE_ENEMY_TANK: 
						spawnEnemyTankAt(pos);
						break;
					case ENTITY_TYPE_ENEMY_TURRET: 
						spawnEnemyTurretAt(pos);
						break;
					}
				}
			}
		}
	updateTileOccupancyMap();
	m_playerTankCount=countAllLivingPlayerTanks();
	}

void Map::initMapColorCodes()
	{
	if (s_tercolors.isEmpty())
		{
		s_tercolors.add(Rgba::GREEN,TILE_TYPE_GRASS);
		s_tercolors.add(Rgba::GRAY,TILE_TYPE_STONE);
		s_tercolors.add(Rgba::BLACK,TILE_TYPE_START_PLATFORM);
		s_tercolors.add(Rgba::WHITE,TILE_TYPE_END_PLATFORM);
		}
	if (s_charcolors.isEmpty())
		{
		s_charcolors.add(Rgba::BLUE,ENTITY_TYPE_PLAYER_TANK);
		s_charcolors.add(Rgba::RED,ENTITY_TYPE_ENEMY_TANK);
		s_charcolors.add(Rgba::ORANGE,ENTITY_TYPE_ENEMY_TURRET);
		}
	}

void Map::setStartOrEnd(TileType ttype,int tx,int ty)
	{
	if (ttype==TILE_TYPE_START_PLATFORM)
		m_levelStartPos=calcTileCenterFromCoords(IntVector2(tx,ty));
	else if (ttype==TILE_TYPE_END_PLATFORM)
		m_levelEndPos=calcTileCenterFromCoords(IntVector2(tx,ty));
	}

Vector2 Map::calcTileCenterFromCoords(const IntVector2& tilecoords)
	{
	float cx = ((float)tilecoords.x + 0.5f) * MAP_TILE_SIZE;
	float cy = ((float)tilecoords.y + 0.5f) * MAP_TILE_SIZE;
	return Vector2(cx,cy);
	}

void Map::preventSolidTileCollisonsForSingleEntity(Entity* entity)
	{
	if(entity==nullptr)
		return;
	EntityType etype=entity->gettype();
	std::vector<Tile> entityNeighbors=getNeighborsOfEntity(entity);
	if (etype == ENTITY_TYPE_PLAYER_TANK || etype == ENTITY_TYPE_ENEMY_TANK)
		{
		for (int ineighbor = 0; ineighbor < (int)entityNeighbors.size() ; ineighbor++)
			{
			Tile tile = entityNeighbors.at(ineighbor);
			TileDef tiledef = tile.getdef();
			if (!tiledef.m_canPassThru)
				{
				AABB2 tilebounds = calcTileBounds(tile);
				entity->checkAndPushAwayFrom(tilebounds);
 				}
			}
		}
	}

std::vector<Tile> Map::getNeighborsOfEntity(Entity* entity)
	{
	Vector2 epos = entity->getpos();
	IntVector2 eposTile = calcMapTilePosFromWorldCoords(epos.x,epos.y);
	bool couldHaveLeftNeighbors = eposTile.x - 1 >= 0;
	bool couldHaveRightNeighbors = eposTile.x + 1 <= m_amtTiles.x - 1;
	bool couldHaveTopNeighbors = eposTile.y - 1 >= 0;
	bool couldHaveBottomNeighbors = eposTile.y + 1 <= m_amtTiles.y - 1;
	std::vector<Tile> result;
	int top=eposTile.y-1;
	int bottom=eposTile.y+1;
	int left=eposTile.x-1;
	int right=eposTile.x+1;
	int x=eposTile.x;
	int y=eposTile.y;
	// push center tile
	result.push_back(getTile(x,y));
	// push orthogonal tiles
	if(couldHaveTopNeighbors)
		result.push_back(getTile(x,top));
	if(couldHaveBottomNeighbors)
		result.push_back(getTile(x,bottom));
	if(couldHaveLeftNeighbors)
		result.push_back(getTile(left,y));
	if(couldHaveRightNeighbors)
		result.push_back(getTile(right,y));
	// push diagonal tiles
	if (couldHaveTopNeighbors)
		{
		if (couldHaveLeftNeighbors)
			result.push_back(getTile(left,top));
		if (couldHaveRightNeighbors)
			result.push_back(getTile(right,top));
		}
	if (couldHaveBottomNeighbors)
		{
		if (couldHaveLeftNeighbors)
			result.push_back(getTile(left,bottom));
		if (couldHaveRightNeighbors)
			result.push_back(getTile(right,bottom));
		}
	return result;
	}

void Map::spawnPlayerTankAt(const Vector2& tankPos)
	{
	m_playerTankCount++;
	PlayerTank* tank= new PlayerTank(m_entities.size(),tankPos);
	tank->setactive(m_playerTankCount==1);
	m_entities.push_back(tank);
	updateTileOccupancyMap();
	}

void Map::spawnEnemyTankAt(const Vector2& tankPos)
	{
	EnemyTank* tank = new EnemyTank(m_entities.size(),tankPos);
	tank->setactive(false);
	m_entities.push_back(tank);
	updateTileOccupancyMap();
	}

void Map::spawnEnemyTurretAt(const Vector2& turretPos)
	{
	EnemyTurret* turret = new EnemyTurret(m_entities.size(),turretPos);
	turret->setactive(false);
	m_entities.push_back(turret);
	updateTileOccupancyMap();
	}

void Map::clearMap()
	{
	m_entities.clear();\
	m_playerTankCount=0;
	m_enemyTankCount=0;
	m_tiles.clear();
	}

void Map::loadMap()
	{
	//if (m_level==1)
		{
		loadTerrainFromFile(std::string("Data/Maps/Level1_terrain.png"));
		loadCharacterPlacementsFromFile(std::string("Data/Maps/Level1_characters.png"));
		}
	}


void Map::initMap()
	{
	loadMap();
	updateTileOccupancyMap();
	}

void Map::resetMap()
	{
	clearMap();
	initMap();
	}

void Map::spawnParticleShockwave(const Vector2& pos, float size, float durationSeconds)
	{
	const int AMT_EXPLOSION_DEGREE_STEPS=8;
	const float EXPLOSION_DEGREE_STEP=360.f/(float)AMT_EXPLOSION_DEGREE_STEPS;
	Particle* newparticle;
	Vector2 particleEndPos;
	Vector2 particleEndDisplacement;
	Vector2 particleVel;
	float pvelScale;
	Rgba particleColors[] = { Rgba(0xc0c0c010),Rgba(0xc0c0c010) };
	const int AMT_PARTICLE_RINGS = sizeof(particleColors)/sizeof(particleColors[0]);
	for (int i = 0; i < AMT_PARTICLE_RINGS;i++)
		particleColors[i].setAlphaAsFloat(SHOCKWAVE_PARTICLE_OPACITY);
	float sizeStep = size / (float)AMT_PARTICLE_RINGS;
	float particleRadius=size*0.2f;
	int iring=0;
	int amtParticles=0;
	float invDurationSeconds = durationSeconds == 0.f ? 0 : 1.f / durationSeconds;
	for (float distance = size; distance >= 0.f; distance -= sizeStep)
		{
		float startDegrees=GetRandomFloatInRange(0.f, EXPLOSION_DEGREE_STEP);
		for (float degrees = startDegrees; degrees <= 360.f; degrees += EXPLOSION_DEGREE_STEP)
			{
			particleEndDisplacement = Vector2(distance*cosDegrees(degrees), distance*sinDegrees(degrees));
			particleEndPos = pos + particleEndDisplacement;
			particleVel = particleEndDisplacement * invDurationSeconds;
			pvelScale=GetRandomFloatInRange(0.2f,5.f);
			particleVel*=pvelScale;
			int ientity=m_entities.size();
			newparticle = new Particle(ientity,pos, particleVel, particleRadius,durationSeconds, particleColors[iring]);
			m_entities.push_back(newparticle);
			if (iring < AMT_PARTICLE_RINGS - 1)
				iring++;
			amtParticles++;
			}
		}
	while(amtParticles>0)
		break;
	}

bool Map::isBullet(std::size_t ientity)
	{
	Entity* entity = m_entities[ientity];
	if(entity==nullptr) 
		return false;
	EntityType etype = entity->gettype();
	bool result = etype == ENTITY_TYPE_PLAYER_BULLET || etype == ENTITY_TYPE_ENEMY_BULLET;
	return result;
	}

void Map::spawnAnimatedExplosion(const Vector2& pos,float radius,float durationSeconds)
	{
	int indexOfNewEntity=m_entities.size();
	SpriteSheet* theSpriteSheet = g_theRenderer->createOrGetSpriteSheet(
		std::string("Data/Images/Explosion_5x5.png"),
					EXPLOSION_SPRITE_SHEET_AMT_TILES_WIDE,
					EXPLOSION_SPRITE_SHEET_AMT_TILES_HIGH);
	if (theSpriteSheet)
		{
		Explosion* explosion=new Explosion(indexOfNewEntity,*theSpriteSheet,pos,radius,durationSeconds);
		m_entities.push_back(explosion);
		}
	}

bool Map::doesBulletHitAnyTank(PlayerBullet* pbullet, EnemyTank*& hitTank)
	{
	if(pbullet==nullptr)
		return false;
	Disc2 pbulletBounds=pbullet->calcPhysicsBounds();
	EnemyTank* etank;
	Disc2 etankBounds;
	for (int ientity = 0; ientity < (int)m_entities.size() ; ientity++)
		{
		Entity* curentity=m_entities[ientity];
		if (curentity && !curentity->isDead() && (   curentity->gettype() == ENTITY_TYPE_ENEMY_TANK
												  || curentity->gettype() == ENTITY_TYPE_ENEMY_TURRET))
			{
			etank=(EnemyTank*)curentity;
			etankBounds=etank->calcPhysicsBounds();
			if (pbulletBounds.doesOverlapWith(etankBounds))
				{
				hitTank=etank;
				return true;
				}
			}
		}
	return false;


	}

void Map::respawnEntitiesWhenReady()
	{
	for (int ientity = 0; ientity < (int)m_entities.size() ; ientity++)
		{
		Entity* curentity=m_entities.at(ientity);
		if (curentity && curentity->isDead() && curentity->shouldBeRespawned())
			{
			EntityType etype=curentity->gettype();
			switch (etype)
				{
				case ENTITY_TYPE_PLAYER_TANK:
					curentity->respawn(m_levelStartPos);
					break;
				case ENTITY_TYPE_ENEMY_TANK:
				case ENTITY_TYPE_ENEMY_TURRET:
					curentity->respawn(curentity->getpos());
					break;
				}
			}
		}
	}

bool Map::doesBulletHitAnyPlayerTank(EnemyBullet* enemybullet, PlayerTank*& hitTank)
	{
	if (enemybullet == nullptr)
		return false;
	Disc2 ebulletBounds = enemybullet->calcPhysicsBounds();
	PlayerTank* ptank;
	Disc2 ptankBounds;
	for (int ientity = 0; ientity < (int)m_entities.size(); ientity++)
		{
		Entity* curentity = m_entities[ientity];
		if (curentity && !curentity->isDead() && curentity->gettype() == ENTITY_TYPE_PLAYER_TANK)
			{
			ptank = (PlayerTank*)curentity;
			ptankBounds = ptank->calcPhysicsBounds();
			if (ebulletBounds.doesOverlapWith(ptankBounds))
				{
				hitTank = ptank;
				return true;
				}
			}
		}
	return false;
	}

void Map::spawnExplosion(const Vector2& pos, float size, float durationSeconds)
	{
	spawnParticleShockwave(pos,size, durationSeconds);
	spawnAnimatedExplosion(pos,size*0.5f,durationSeconds*1.25f);
	}

void Map::updateInput(float deltaSeconds)
	{
	XboxController& xbc = g_theInput->getXboxController(XCONTROLLER_0);
	if (g_theInput->isKeyDown(KEYCODE_OEM_PLUS) || g_theInput->isKeyDown(KEYCODE_ADD) || xbc.isXBoxButtonPressed(XBUTTON_RB))
		{
		if(m_renderScale>0.f)
			m_renderScale -= MAP_ZOOM_RATE*deltaSeconds;
		}
	if (g_theInput->isKeyDown(KEYCODE_OEM_MINUS) || g_theInput->isKeyDown(KEYCODE_SUBTRACT) || xbc.isXBoxButtonPressed(XBUTTON_LB))
		{
		if(m_renderScale<10.f)
			m_renderScale += MAP_ZOOM_RATE*deltaSeconds;
		}
	if (g_theInput->isKeyDown(KEYCODE_CONTROL))
		{
		if(g_theInput->wasKeyJustPressed('R'))
			resetMap();
		}
	}

void Map::separateAllEntities()
	{
	for (std::size_t ientity = 0; ientity < m_entities.size() ; ientity++)
		{
		Entity* entity=m_entities[ientity];
		if (entity)
			{
			if (entity == nullptr || entity->isDead())
				continue;
			if (entity->canOverlap())
				continue;
			for (std::size_t iother = 0; iother < m_entities.size(); iother++)
				{
				Entity* other=m_entities[iother];
				if (other == nullptr || other->isDead())
					continue;
				if (other->canOverlap())
					continue;
				if (entity != other )
					separateTwoEntities(*entity,*other);
				}
			}
		}
	}
