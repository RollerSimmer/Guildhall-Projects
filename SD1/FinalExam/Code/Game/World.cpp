#include "Game/World.hpp"

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Goal.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Game/Turret.hpp"
#include "Game/Bullet.hpp"
#include <stack>


World::World(int level)
	: m_level(level)
	, m_thePlayer(nullptr)
	, m_theGoal(nullptr)
	{
	buildMap();
	spawnPlayer();
	spawnGoal();
	spawnEnemies();	
	}

void World::render() const
	{
	renderTiles();
	renderEntities();
	}

void World::update(float deltaSeconds)
	{
	m_thePlayer->update(deltaSeconds);
	m_theGoal->update(deltaSeconds);
	keepPlayerOnSand();
	updateEnemies(deltaSeconds);
	updateBullets(deltaSeconds);

	}

void World::buildMap()
	{
	for (int ty = 0; ty < AMT_TILES_Y ; ty++)
		{
		for (int tx = 0; tx < AMT_TILES_X ; tx++)
			{
			Tile& curtile=m_tiles[ty][tx];
			curtile.settype(TILE_TYPE_SAND);
			curtile.setpos(IntVector2(tx,ty));
			curtile.m_isOffLimits=(tx<=4&&ty<=4)||(tx>=13&&ty>=6);
			if(isEdgeTile(tx,ty))
				{
				curtile.settype(TILE_TYPE_WALL);
				curtile.m_isOffLimits=true;
				}
			
			}
		}
	// set starting box
	m_tiles[4][1].settype(TILE_TYPE_WALL);
	m_tiles[4][2].settype(TILE_TYPE_WALL);
	m_tiles[1][4].settype(TILE_TYPE_WALL);
	m_tiles[2][4].settype(TILE_TYPE_WALL);
	m_tiles[2][2].settype(TILE_TYPE_WALL);
	
	}

IntVector2 World::calcMapTilePosFromWorldCoords(const Vector2& worldPos) const
	{
	return IntVector2((int)(worldPos.x / MAP_TILE_SIZE),(int)(worldPos.y / MAP_TILE_SIZE));
	}

Vector2 World::calcWorldPosFromTileCoords(const IntVector2& tilepos) const
	{
	return Vector2((float)tilepos.x*MAP_TILE_SIZE,(float)tilepos.y*MAP_TILE_SIZE);
	}

void World::drawTile(const Tile& tile) const
	{
	TileDef tdef=tile.getdef();
	Rgba color = g_doShowDebugGraphics&&tile.m_isOffLimits?  Rgba::YELLOW : Rgba::WHITE;

	Vector2 topleft=calcWorldPosFromTileCoords(tile.getpos());
	Vector2 btmright=topleft+Vector2(MAP_TILE_SIZE,MAP_TILE_SIZE);
	AABB2 tbounds(topleft,btmright);
	float orientationDegrees=0.f;
	g_theRenderer->drawTexturedAABB2(tbounds,*tdef.m_texture,Vector2(0.f,0.f),Vector2(1.f,1.f),color,orientationDegrees);
	}

void World::renderTiles() const
	{
	for (int ty = 0; ty < AMT_TILES_Y ; ty++)
		{
		for (int tx = 0; tx < AMT_TILES_X ; tx++)
			{
			Tile curtile=m_tiles[ty][tx];
			drawTile(curtile);
			}
		}
	}

void World::renderEntities() const
	{
	if (m_theGoal)
		m_theGoal->render();
	if(m_thePlayer)
		m_thePlayer->render();

	renderAllEnemies();
	renderAllBullets();
	}

bool World::isEdgeTile(int tx,int ty)
	{
	return tx==0 || ty==0 || tx==AMT_TILES_X-1 || ty == AMT_TILES_Y-1;
	}

void World::spawnEnemies()
	{
	spawnSingleDummyTurret();
	spawnSingleDummyTurret();
	spawnSingleAimingTurret();
	spawnSingleAimingTurret();
	spawnSingleHomingTurret();
	spawnSingleHomingTurret();
	}

void World::spawnPlayer()
	{
	if (m_thePlayer)
		destroyPlayer();
	const Vector2 PLAYER_TANK_STARTING_POS=calcCenteredWorldPosFromTileCoords(IntVector2(1,1));
	const float PLAYER_TANK_RADIUS = 25.f;
	const float PLAYER_TANK_STARTING_SPEED = 0.f;
	const int PLAYER_TANK_HP_CAPACITY = 1;
	m_thePlayer=new PlayerTank(PLAYER_TANK_STARTING_POS,PLAYER_TANK_RADIUS,PLAYER_TANK_STARTING_SPEED,PLAYER_TANK_HP_CAPACITY);
	}

void World::destroyPlayer()
	{
	if (m_thePlayer)
		{
		delete m_thePlayer;
		m_thePlayer=nullptr;
		}
	}

void World::updateEnemies(float deltaSeconds)
	{
	for (int i = 0; i < (int)m_enemies.size() ; i++)
		{
		Entity* e=m_enemies.at(i);
		if (e)
			{
			e->update(deltaSeconds);
			}
		}
	}
void World::updateBullets(float deltaSeconds)
	{
	for (int i = 0; i < (int)m_bullets.size() ; i++)
		{


		Bullet* b=m_bullets.at(i);
		if (b)
			{
			b->update(deltaSeconds);
			}
		}
	killBulletsThatHitWalls();

	}

void World::spawnGoal()
	{
	if (m_theGoal==nullptr)
		{
		m_theGoal=new Goal();
		}
	}

void World::keepPlayerOnSand()
	{
	if(m_thePlayer==nullptr)
		return;
	IntVector2 ppos=calcMapTilePosFromWorldCoords(m_thePlayer->getpos());
	checkAndSeparatePlayerFromTile(IntVector2(ppos.x,ppos.y));
	checkAndSeparatePlayerFromTile(IntVector2(ppos.x,ppos.y-1));
	checkAndSeparatePlayerFromTile(IntVector2(ppos.x,ppos.y+1));
	checkAndSeparatePlayerFromTile(IntVector2(ppos.x-1,ppos.y));
	checkAndSeparatePlayerFromTile(IntVector2(ppos.x+1,ppos.y+1));
	checkAndSeparatePlayerFromTile(IntVector2(ppos.x+1,ppos.y-1));
	checkAndSeparatePlayerFromTile(IntVector2(ppos.x-1,ppos.y+1));
	checkAndSeparatePlayerFromTile(IntVector2(ppos.x-1,ppos.y+1));
	}

void World::checkAndSeparatePlayerFromTile(const IntVector2& tilepos)
	{
	if(tilepos.x<0 || tilepos.y<0 || tilepos.x >=AMT_TILES_X || tilepos.y>=AMT_TILES_Y)
		return;
	Tile tile=m_tiles[tilepos.y][tilepos.x];
	Disc2 playerDisc=m_thePlayer->calcPhysicsDisc();
	TileDef tdef=tile.getdef();
	if(tdef.m_canPassThru)
		return;
	AABB2 tilebounds= calcTileBounds(tilepos); 
	if(playerDisc.doesOverlapWith(tilebounds))
		m_thePlayer->pushAwayFrom(tilebounds);
	}

Tile World::findOpenTile()
	{
	Tile result;
	do
		{
		IntVector2 tc(GetRandomIntInRange(1,14),GetRandomIntInRange(1,8));
		result = m_tiles[tc.y][tc.x];
		} while(result.m_isOffLimits);
	return result;
	}

bool World::isLevelOver()
	{
	return m_thePlayer->doesCollideWith(*m_theGoal);
	}

void World::spawnSingleDummyTurret()
{
	Tile opentile=findOpenTile();
	Vector2 tpos=calcCenteredWorldPosFromTileCoords(opentile.getpos());
	Turret* turret=new Turret(tpos);
	setSelfAndNeighborsToOffLimits(opentile);

	m_enemies.push_back(turret);
	}

bool World::isPlayerHit()
	{
	if(m_thePlayer==nullptr)
		return false;
	for (int i = 0; i < (int)m_bullets.size() ; i++)
		{
		Bullet* bullet=m_bullets.at(i);
		if(bullet && m_thePlayer->doesCollideWith(*bullet))
			return true;
		}
	return false;
	}

void World::spawnBullet(const Vector2& bpos, float odegs, const Rgba& color, bool canSeek, float radius)
	{
	/*if (m_bullets.size()<10)*/
		{
		Bullet* bullet=new Bullet(bpos,odegs,color,canSeek,radius);
		m_bullets.push_back(bullet);
		}
	}

void World::killBulletsThatHitWalls()
	{
	std::stack<int> killList;
	for (int i=0;i<(int)m_bullets.size();i++)
		{
		Bullet* bullet=m_bullets.at(i);
		if(bullet&&bulletHitsWall(bullet))
			killList.push(i);
		}
	while (!killList.empty())
		{
		int i=killList.top();
		killList.pop();
		m_bullets.erase(m_bullets.begin()+i);
		g_theAudio->playSound(g_theAudio->createOrGetSound("Data/Audio/Explosion.ogg"),0.3f);
		}
	}

void World::renderAllEnemies() const
	{
	for (int i = 0; i < (int)m_enemies.size() ; i++)
		{
		Entity* e=m_enemies.at(i);
		if (e)
			{
			e->render();
			}
		}
	}

void World::renderAllBullets() const
	{
	for (int i = 0; i < (int)m_bullets.size() ; i++)
		{
		Bullet* bullet=m_bullets.at(i);
		if(bullet)
			bullet->render();
		}
	}

bool World::bulletHitsWall(Bullet* bullet)
	{
	if (bullet==nullptr)
		return false;
	IntVector2 bpos=calcMapTilePosFromWorldCoords(bullet->getpos());
	return false
		|| doesBulletHitWall(bullet,IntVector2(bpos.x,bpos.y))
		|| doesBulletHitWall(bullet,IntVector2(bpos.x,bpos.y-1))
		|| doesBulletHitWall(bullet,IntVector2(bpos.x,bpos.y+1))
		|| doesBulletHitWall(bullet,IntVector2(bpos.x-1,bpos.y))
		|| doesBulletHitWall(bullet,IntVector2(bpos.x+1,bpos.y+1))
		|| doesBulletHitWall(bullet,IntVector2(bpos.x+1,bpos.y-1))
		|| doesBulletHitWall(bullet,IntVector2(bpos.x-1,bpos.y+1))
		|| doesBulletHitWall(bullet,IntVector2(bpos.x-1,bpos.y+1));
	}

bool World::doesBulletHitWall(Bullet* bullet, const IntVector2& tilepos)
	{
	if(tilepos.x<0 || tilepos.y<0 || tilepos.x>15 || tilepos.y>8)
		return false;
	Tile tile=m_tiles[tilepos.y][tilepos.x];
	TileDef tdef=tile.getdef();
	if(tdef.m_canPassThru)
		return false; 
	Disc2 bulletDisc=bullet->calcPhysicsDisc();
	bool result=bulletDisc.doesOverlapWith(calcTileBounds(tilepos));
	while(result)
		break;
	return result;
	}


AABB2 World::calcTileBounds(const IntVector2& tilepos)
	{
	AABB2 result=AABB2(calcWorldPosFromTileCoords(IntVector2(tilepos.x,tilepos.y)),
					   calcWorldPosFromTileCoords(IntVector2(tilepos.x+1,tilepos.y+1)));
	return result;
	}

void World::setSelfAndNeighborsToOffLimits(Tile tile)
	{
	IntVector2 tpos=tile.getpos();
	int tileleft=tpos.x>0?tpos.x-1:tpos.x;
	int tileright=tpos.x<15?tpos.x+1:tpos.x;
	int tileup=tpos.y<9?tpos.y+1:tpos.y;
	int tiledown=tpos.y>0?tpos.y-1:tpos.y;
	for (int y=tiledown;y<=tileup;y++)
		{
		for (int x=tileleft;x<=tileright;x++)
			{
			m_tiles[y][x].m_isOffLimits=true;
			}
		}

	}

bool World::doesTurretHaveLineOfSightToPlayer(const Turret& turret)
	{
	if(m_thePlayer==nullptr)
		return false;
	Vector2 tpos=turret.getpos();
	Vector2 ppos=m_thePlayer->getpos();
	Vector2 d=ppos-tpos;
	float dist=d.CalcLength();
	if(dist==0.f)
		return true;
	float stepDist=MAP_TILE_SIZE*0.1f;
	Vector2 step=d;
	step.setLength(stepDist);
	Vector2 curpos=tpos;
	float distTravelled=0.f;
	while(distTravelled<dist)	
		{
		IntVector2 tilepos=calcMapTilePosFromWorldCoords(curpos);
		Tile tile=m_tiles[tilepos.y][tilepos.x];
		TileDef tdef=tile.getdef();
		if(!tdef.m_canSeeThru)
			return false;
		curpos+=step;
		distTravelled+=stepDist;
		}
	return true;
	}

void World::spawnSingleHomingTurret()
	{
	Tile opentile=findOpenTile();
	Vector2 tpos=calcCenteredWorldPosFromTileCoords(opentile.getpos());
	Turret* turret=Turret::createHomingTurret(tpos);
	setSelfAndNeighborsToOffLimits(opentile);

	m_enemies.push_back(turret);
	}

void World::spawnSingleAimingTurret()
	{
	Tile opentile=findOpenTile();
	Vector2 tpos=calcCenteredWorldPosFromTileCoords(opentile.getpos());
	Turret* turret=Turret::createAimingTurret(tpos);
	setSelfAndNeighborsToOffLimits(opentile);
	m_enemies.push_back(turret);
	}
Vector2 World::calcCenteredWorldPosFromTileCoords(const IntVector2& tilepos)
	{
	Vector2 result=calcWorldPosFromTileCoords(tilepos);
	result+=Vector2(MAP_TILE_SIZE*0.5f,MAP_TILE_SIZE*0.5f);
	return result;
	}


;