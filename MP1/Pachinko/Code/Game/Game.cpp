#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Game/Ball.hpp"
#include "Game/BUmper.hpp"
#include "Game/Wall.hpp"

void Game::cleanUpProjectiles()
	{
	for (int i = 0; i < (int)m_projectiles.size(); i++)
		{
		if (m_projectiles[i])
			{
			delete m_projectiles[i];
			m_projectiles[i] = nullptr;
			}
		}

	m_projectiles.clear();
	}

void Game::cleanUpObstacles()
	{
	for (int i = 0; i < (int)m_obstacles.size(); i++)
		{
		if (m_obstacles[i])
			{
			delete m_obstacles[i];
			m_obstacles[i] = nullptr;
			}
		}
	m_obstacles.clear();
	}

void Game::togglePause()
	{
	if (m_isPaused)
		m_isPaused = false;
	else
		m_isPaused = true;
	}

void Game::updateKeyInput(float& deltaSeconds)
	{
	if (g_theInput->wasKeyJustPressed(KEYCODE_PAUSE) || g_theInput->wasKeyJustPressed('P') || g_theInput->getXboxController(XCONTROLLER_0).wasXboxButtonJustPressed(XBUTTON_START))
		togglePause();
	if (m_isPaused)
		deltaSeconds = 0.f;

	if (g_theInput->isKeyDown('T'))
		{
		float slowfactor = 0.1f;
		if (g_theInput->isKeyDown(KEYCODE_SHIFT))
			deltaSeconds *= slowfactor*slowfactor;
		else
			deltaSeconds *= slowfactor;
		}

	const int BALL_AMOUNT = 50;
	
	if (g_theInput->wasKeyJustPressed('I') || g_theInput->getXboxController(XCONTROLLER_0).wasXboxButtonJustPressed(XBUTTON_Y))
		{
		if(g_theInput->isKeyDown(KEYCODE_SHIFT)) 
			spawnBalls(BALL_AMOUNT);
		else
			spawnBall();

		}

	if (g_theInput->wasKeyJustPressed('O') || g_theInput->getXboxController(XCONTROLLER_0).wasXboxButtonJustPressed(XBUTTON_X))
		{
		if (g_theInput->isKeyDown(KEYCODE_SHIFT))
			destroyManyBalls(BALL_AMOUNT);
		else
			destroyAnyBall();
		}
	if (g_theInput->wasKeyJustPressed('K'))
		g_worldGravitationalAcceleration *= 1.1f;
	if (g_theInput->wasKeyJustPressed('J'))
		g_worldGravitationalAcceleration *= (1.f / 1.1f);
	if (g_theInput->wasKeyJustPressed('M'))
		changeGlobalElasticityBy(0.05f);
	if (g_theInput->wasKeyJustPressed('N'))
		changeGlobalElasticityBy(-0.05f);
	if (g_theInput->wasKeyJustPressed(KEYCODE_OEM_4)) //'['
		scaleAllProjectilesBy(1.1f);
	if (g_theInput->wasKeyJustPressed(KEYCODE_OEM_6)) //']'
		scaleAllProjectilesBy(1.f/1.1f);
	if(g_theInput->wasKeyJustPressed('W'))
		toggleWaterRendering();
	}

void Game::destroyAnyBall()
	{
	if (m_projectiles.size() > 0)
		{
		int i=m_projectiles.size()-1;
		destroySingleProjectile(i);
		m_projectiles.pop_back();
		}
	}

void Game::destroySingleProjectile(int i)
	{
	delete m_projectiles[i];
	m_projectiles[i] = nullptr;
	}

void Game::destroyManyBalls(int amount)
	{
	for (int iball = 0; iball < amount ; iball++)
		{
		destroyAnyBall();
		}
	}

void Game::processProjectileObstacleCollisions()
	{
	for (int iProjectile = 0; iProjectile < (int)m_projectiles.size() ; iProjectile++)
		{
		if (m_projectiles[iProjectile])
			{
			for (int iObstacle = 0; iObstacle < (int)m_obstacles.size() ; iObstacle++)
				{
				if (m_obstacles[iObstacle])
					{
					if(m_obstacles[iObstacle]->doesCollideWith(m_projectiles[iProjectile]))
						processSingleObstacleProjectileCollision(iObstacle,iProjectile);
					}
				}
			}
		}
	}

void Game::processProjectileProjectileCollisions()
	{
	for (int iProjectile = 0; iProjectile < (int)m_projectiles.size(); iProjectile++)
		{
		if (m_projectiles[iProjectile])
			{
			for (int iOther = 0; iOther < (int)m_projectiles.size(); iOther++)
				{
				if (iProjectile != iOther && m_projectiles[iOther])
					{
					if(m_projectiles[iProjectile]->doesCollideWithOtherProjectile(*m_projectiles[iOther]))
						processSingleProjectileProjectileCollision(iProjectile,iOther);
					}			
				}
			}
		}
	}

void Game::processSingleProjectileProjectileCollision(int iproj,int iother)
	{
	m_projectiles[iproj]->pullApartFrom(*m_projectiles[iother]);
	m_projectiles[iproj]->transferMomentumWithOtherProjectile(*m_projectiles[iother]);
	}

void Game::processSingleObstacleProjectileCollision(int iObstacle, int iProjectile)
	{
	m_obstacles[iObstacle]->processCollision(m_projectiles[iProjectile]);
	}

void Game::changeGlobalElasticityBy(float changeValue)
	{
	g_projectileElasticity = clampFloat(g_projectileElasticity + changeValue, 0.f, 1.f);
	g_obstacleElasticity = clampFloat(g_obstacleElasticity + changeValue, 0.f, 1.f);
	}

void Game::scaleAllProjectilesBy(float scale)
	{
	m_ballscale*=scale;
	for (int iProjectile = 0; iProjectile < (int)m_projectiles.size() ; iProjectile++)
		{
		if(m_projectiles[iProjectile])
			m_projectiles[iProjectile]->scaleBy(scale);
		}
	}

void Game::toggleWaterRendering()
	{
	switch (g_projectileRenderType)
		{
		case PROJ_RENDER_TYPE_LIQUID: g_projectileRenderType = PROJ_RENDER_TYPE_SOLID;  break;
		case PROJ_RENDER_TYPE_SOLID:  g_projectileRenderType = PROJ_RENDER_TYPE_LIQUID; break;
		}
	}

void Game::spawnBalls(int amount)
	{
	for (int iball = 0; iball < amount ; iball++)
		{
		spawnBall();
		}
	}

void Game::spawnWall(float x, float y, float xradius, float yradius)
	{
	m_obstacles.push_back(new Wall(x,y,xradius,yradius));
	}

Game::Game()
	: m_obstacles()
	, m_projectiles()
	, m_ballscale(INITIAL_BALL_SCALE)
	, m_isPaused(false)
	{
	init();
	}


Game::~Game()
	{
	cleanUpProjectiles();
	cleanUpObstacles();
	}

void Game::init()
	{
	initObstacles();
	spawnBall();
	g_doShowDebugGraphics = false;
	}

void Game::initObstacles()
	{
	float bumperRadius;
	float x,y;
	// make top ring of bumpers
	float degreeStep=10.f;
	bumperRadius=10.f;
	float ringRadius=75.f;
	Vector2 ringCenter(400.f,200.f);
	for (float degrees = 0.f; degrees <= 360.f; degrees += degreeStep)
		{
		x = ringCenter.x + ringRadius*cosDegrees(degrees);
		y = ringCenter.y + ringRadius*sinDegrees(degrees);
		spawnBumper(x, y, bumperRadius);
		}
	// make top row
	bumperRadius=25.f;
	for (int i = 1; i < 7; i++)
		{
		if(i == 3 || i==4)
			continue;
		x = static_cast<float>(i)*100.f + 50.f;
		y = 200.f;
		spawnBumper(x, y, bumperRadius);
		}
	// make middle row
	bumperRadius=50.f;
	for (int i = 1; i < 4; i++)
		{
		x = static_cast<float>(i)*200.f;
		y = 400.f;
		spawnBumper(x, y, bumperRadius);
		}
	// make bottom row
	bumperRadius=75.f;
	for (int i = 0; i < 4; i++)
		{
		x = static_cast<float>(i)*200.f+100.f;
		y = 600.f;
		spawnBumper(x, y, bumperRadius);
		}
	// make walls
	float xradius=60.f;
	spawnWall(0.5f, 0.5f*WORLD_HEIGHT, xradius, 3.f*WORLD_HEIGHT);
	spawnWall(WORLD_WIDTH-1.f, 0.5f*WORLD_HEIGHT, xradius, 3.f*WORLD_HEIGHT);
	m_wallsize = xradius*2.f;
	}


void Game::spawnBumper(float x, float y, float radius)
	{
	m_obstacles.push_back(new Bumper(Disc2(x,y,radius)));
	}

void Game::update(float deltaSeconds)
	{
	updateKeyInput(deltaSeconds);

	if (g_theInput->wasKeyJustPressed(KEYCODE_SPACE))
		toggleDebugGraphics();

	processProjectileObstacleCollisions();
	processProjectileProjectileCollisions();

	//TODO: update all obstacles
	for (int i = 0; i < (int)m_projectiles.size() ; i++)
		{
		if (m_projectiles[i])
			{
			m_projectiles[i]->update(deltaSeconds);
			}
		}

	}
void Game::render()
	{
	for (int i = 0; i < (int)m_projectiles.size(); i++)
		{
		if (m_projectiles[i])
			m_projectiles[i]->render();
		}
	for (int iObstacle = 0; iObstacle < (int)m_obstacles.size(); iObstacle++)
		{
		if(m_obstacles[iObstacle])
			m_obstacles[iObstacle]->render();
		}
	}

void Game::toggleDebugGraphics()
	{
// 	g_doShowDebugGraphics = !g_doShowDebugGraphics;
	}

void Game::spawnBall()
	{
	float ballradius=GetRandomFloatInRange(0.85f*m_ballscale,1.15f*m_ballscale);
	float yball = g_theWorldBounds->m_mins.x-ballradius;
	//TODO: change yball back to ballradius after testing
	float xball = GetRandomFloatInRange(g_theWorldBounds->m_mins.x+m_wallsize+ballradius,g_theWorldBounds->m_maxs.x-m_wallsize-ballradius);
	m_projectiles.push_back(new Ball(xball,yball,ballradius));
	}
